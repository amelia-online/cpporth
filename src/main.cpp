#include <print>

#include <cpporth/lexer.hpp>

int
main ()
{
  std::string input = "include \"porth/std/std.porth\"\n"
                      "proc main in\n    \"Hello, world!\" puts\n end\n";

  cpporth::Lexer l;

  const auto tokens = l.lex_all (input);

  for (const auto &token : tokens)
    std::println ("{}", token.string_value);
}