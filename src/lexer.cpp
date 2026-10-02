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
  return index < static_cast<std::int64_t> (data.length ());
}

std::optional<Token>
Lexer::is_token (const boost::regex &allowed_chars, const boost::regex &pat,
                 TokenType token_type)
{
  std::string buf = "";
  std::size_t start = index + 1;
  while (true)
    {
      std::println ("buf: {}", buf);
      const auto opt = next ();

      if (!opt
          || (opt.has_value ()
              && !boost::regex_match (std::string{ *opt }, allowed_chars)))
        break;

      buf += *opt;
    }

  if (boost::regex_match (buf, pat))
    {
      std::println ("Match.");
      return Token{
        .token_type = token_type,
        .string_value = std::move (buf),
        .start = static_cast<std::size_t> (start),
        .end = static_cast<std::size_t> (index),
      };
    }
  return {};
}

std::vector<Token>
Lexer::lex ()
{
  static const boost::regex integer_pat ("^(?:\\d+|0x[0-9A-Fa-f]+)$");
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

      if (is_number (ch))
        {
          static const boost::regex integer_allowed ("[0-9a-fA-Fx]");
          const auto token_opt
              = is_token (integer_allowed, integer_pat, TokenType::Integer);
          if (token_opt)
            tokens.push_back (*token_opt);
        }
      else
        throw std::out_of_range ("Failed to lex " + std::string{ ch });
    }
  return tokens;
}

std::vector<Token>
Lexer::lex_all (const std::string &input)
{
  data = input;
  index = -1;
  return lex ();
}