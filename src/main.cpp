#include <print>

#include <cpporth/lexer.hpp>

int
main ()
{
  cpporth::Lexer l;
  const auto tokens = l.lex_all ("0xffc1 0x442a");
  for (const auto &token : tokens)
    std::println ("{}", token.string_value);
}