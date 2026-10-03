#include <cpporth/lexer.hpp>

#include <print>

using namespace cpporth;

namespace
{
  bool
  is_number (char c)
  {
    static const std::string digits = "0123456789";
    return digits.contains (c);
  }

  bool
  char_in (char c, const std::string &pat_string)
  {
    boost::regex pat (pat_string);
    return boost::regex_match (std::string{ c }, pat);
  }
}

Lexer::Lexer () : index (0), line (1), data ("") {}

std::optional<char>
Lexer::next ()
{
  index += 1;
  if (static_cast<std::int64_t> (data.length ()) <= index)
    return {};
  return data.at (static_cast<std::size_t> (index));
}

std::optional<char>
Lexer::peek ()
{
  if (static_cast<std::int64_t> (data.length ()) <= index + 1)
    return {};
  return data.at (static_cast<std::size_t> (index + 1));
}

bool
Lexer::has_next ()
{
  return index < static_cast<std::int64_t> (data.length () - 1);
}

bool
Lexer::peek_newline ()
{
  return (peek ().has_value () && *peek () == '\n');
}

Token
Lexer::collect (const boost::regex &allowed)
{
  std::string buf = "";
  std::size_t start = index + 1;
  while (true)
    {
      // This approach skips spaces. Normally, that would be a problem, but
      // since this language is stack-based it doesn't really matter.
      // I do, however, want to keep track of newlines.
      if (peek_newline ())
        break;

      const auto opt = next ();

      if (!opt
          || (opt.has_value ()
              && !boost::regex_match (std::string{ *opt }, allowed)))
        break;

      buf.push_back (*opt);
    }
  return Token{ TokenType::NIL, buf, start, static_cast<std::size_t> (index) };
}

std::optional<Token>
Lexer::to_token (const boost::regex &allowed_chars, const boost::regex &pat,
                 TokenType token_type)
{
  std::string buf = "";
  std::size_t start = index + 1;
  while (true)
    {
      if (peek_newline ())
        break;

      const auto opt = next ();

      if (!opt
          || (opt.has_value ()
              && !boost::regex_match (std::string{ *opt }, allowed_chars)))
        break;

      buf.push_back (*opt);
    }
  if (boost::regex_match (buf, pat))
    {
      return Token{
        .token_type = token_type,
        .string_value = std::move (buf),
        .start = static_cast<std::size_t> (start),
        .end = static_cast<std::size_t> (index),
      };
    }

  return {};
}

std::optional<char>
Lexer::prev ()
{
  if (index < 1)
    return {};
  return data.at (index - 1);
}

Token
Lexer::lex_string ()
{
  int quote_count = 0;
  std::size_t start = index + 1;
  std::string buf = "";
  bool exit = false;
  while (!exit)
    {
      if (peek_newline ())
        throw std::invalid_argument ("Newline encountered in string.");

      auto opt = next ();
      if (!opt)
        throw std::out_of_range ("Encountered EOF in a string.");

      if (*opt == '\"')
        {

          if (!prev ().has_value ()
              || (prev ().has_value () && prev ().value () != '\\'))
            quote_count += 1;

          if (quote_count == 2)
            exit = true;
        }

      buf.push_back (*opt);
    }
  std::size_t end = index;
  return Token{ TokenType::StringLit, buf, start, end };
}

std::vector<Token>
Lexer::lex ()
{
  std::vector<Token> tokens;
  while (has_next ())
    {
      // Guaranteed.
      const auto ch = peek ().value ();

      if (ch == ' ')
        {
          index++;
          continue;
        }
      else if (ch == '\n')
        {
          tokens.emplace_back (Token{ TokenType::Newline, "NEWLINE",
                                      static_cast<std::size_t> (index + 1),
                                      static_cast<std::size_t> (index + 2) });
          line++;
          index++;
          continue;
        }

      if (is_number (ch))
        {
          static const boost::regex integer_pat ("^(?:\\d+|0x[0-9A-Fa-f]+)$");
          static const boost::regex integer_allowed ("[0-9a-fA-Fx]");
          const auto token_opt
              = to_token (integer_allowed, integer_pat, TokenType::Integer);
          if (token_opt)
            tokens.push_back (*token_opt);
        }
      else if (char_in (ch, "[a-zA-Z_]")) // Identifiers: cannot start with
                                          // anything but '_' or a letter.
        {
          // Keywords come first.
          static const boost::regex allowed_ident_chars (
              "[a-zA-Z0-9\\(\\)_\\-\\*]");
          static const boost::regex keyword_pat (
              "^(?:var|while|proc|in|let|memory|dup|swap|drop|rot|over|do|if|"
              "if\\*|else|end|const|reset|offset|assert|here|include|inline|"
              "addr|addr-of|call-like|peek|syscall[0-6])$");
          // Now I have to map each keyword string to each keyword enum value
          // :(
          auto almost_token = collect (allowed_ident_chars);
          if (boost::regex_match (almost_token.string_value, keyword_pat))
            {
              const auto tt = string_to_tokentype (almost_token.string_value);
              if (tt)
                almost_token.token_type = *tt;
            }
          else
            almost_token.token_type = TokenType::Identifier;
          tokens.push_back (almost_token);
        }
      else if (ch == '\"')
        tokens.push_back (lex_string ());

      else
        throw std::invalid_argument ("Invalid character: "
                                     + std::string{ ch });
    }

  tokens.emplace_back (Token{ TokenType::EndOfFile, "EOF",
                              static_cast<std::size_t> (index),
                              static_cast<std::size_t> (index) });
  return tokens;
}

std::vector<Token>
Lexer::lex_all (const std::string &input)
{
  data = input;
  index = -1;
  return lex ();
}