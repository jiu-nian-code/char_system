#include<iostream>
#include<gtest/gtest.h>

int add(int a, int b)
{
    return a + b;
}

TEST(add函数测试, 测试1)
{
    EXPECT_EQ(add(10, 20), 30);
    EXPECT_EQ(add(10, 10), 20);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}