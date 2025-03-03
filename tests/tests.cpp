#include <gtest/gtest.h>

TEST (TT, Tes) {
    ASSERT_TRUE(true);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest();
    RUN_ALL_TESTS();
}