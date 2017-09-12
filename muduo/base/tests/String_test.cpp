

#include <String.h>
#include <iostream>
#include <vector>

using namespace std;


#include "gtest.h"

GTEST_API_ int main(int argc, char **argv)
{
    std::cout << "Running main() from gtest_main.cc\n";

    testing::InitGoogleTest(&argc, argv);

    String s("123");

    //  printf("%s--\n",s.c_str());
    //  cout << (s == "123") << endl;

    return RUN_ALL_TESTS();
}


TEST(String_test, chars)
{
    String s("123");
    EXPECT_EQ(s, "123");
    String s2(s);
    EXPECT_EQ(s, s2);
    EXPECT_EQ(s2, "123");
    s2 = "321";
    EXPECT_EQ(s2, "321");
    String s3 = "321";
    EXPECT_EQ(s3, s2);

    s3 = "123";
    String s4 = "321";
    swap(s3, s4);
    EXPECT_EQ(s3, "321");
    EXPECT_EQ(s4, "123");
}