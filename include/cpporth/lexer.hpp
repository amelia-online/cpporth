#ifndef CPPORTH__LEXER_HPP_
#define CPPORTH__LEXER_HPP_

#include <cpporth/token.hpp>

#include <boost/regex.hpp>

#include <optional>
#include <vector>

namespace cpporth
{
  class Lexer
  {
  public:
    Lexer ();
    ~Lexer () = default;
    std::vector<Token> lex_all (const std::string &);

  private:
    std::optional<char> peek ();
    std::optional<char> prev ();

    /// @brief Like an iterator.
    std::optional<char> next ();

    std::optional<Token> to_token (const boost::regex &, const boost::regex &,
                                   TokenType);

    /// @brief Useful for tokens that share characters, ex. identifiers and
    /// keywords
    /// @param allowed range of allowed characters
    /// @return An uncategorized token
    Token collect (const boost::regex &);

    Token lex_string ();

    /// @brief Check if the next character (if it exists) is a newline.
    /// @return true if it's a newline, false otherwise.
    bool peek_newline ();

    std::vector<Token> lex ();
    bool has_next ();
    std::int64_t index;
    std::size_t line;
    std::string data;
  };
}

#endif // CPPORTH__LEXER_HPP_