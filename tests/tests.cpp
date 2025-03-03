#include <gtest/gtest.h>
#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/helper.h"

TEST (CPPorth, EnvSetPath) {
    std::string fullpath = "porth/std/std.porth";
    Env e;
    e.setPath(fullpath);
    ASSERT_EQ(e.path, "porth/std/");
    fullpath = "std.porth";
    e.setPath(fullpath);
    ASSERT_EQ(e.path, "");
}

TEST (CPPorth, Include1)
{
    std::string code = "include \"porth/std/std.porth\"\n\n";
                code += "proc main in end\n";
    
    Lexer l(code);
    Parser p(l.lex());

    Stack s;
    Env e;
    auto asts = p.parse();
    interp(asts, s, e);

    ASSERT_TRUE(e.containsKey("stderr"));
    ASSERT_EQ(e.getVar("stderr").getValue(), 2);

    ASSERT_TRUE(e.containsKey("sizeof(u64)"));
    ASSERT_EQ(e.getVar("sizeof(u64)").getValue(), 8);

    p.cleanup(asts);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}