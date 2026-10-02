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

    /// @brief Like an iterator.
    std::optional<char> next ();

    std::optional<Token> is_token (const boost::regex &, const boost::regex &,
                                   TokenType);

    std::vector<Token> lex ();
    bool has_next ();
    std::int64_t index;
    std::size_t line;
    std::string data;
  };
}

#endif // CPPORTH__LEXER_HPP_