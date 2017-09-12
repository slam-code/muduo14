
#include <muduo/log/LogStream.h>
#include <muduo/base/gtest.h>
#include <limits>
#include <stdint.h>
#include <string>

using namespace std;
//clang++ gtest_main.cc  -I./ libgtest.a -pthread -o gtest_main

GTEST_API_ int main(int argc, char **argv)
{
    std::cout << "Running main() from gtest_main.cc\n";

//InitGoogleTest会解析参数。RUN_ALL_TESTS会把整个工程里的
// TEST和TEST_F这些函数全部作为测试用例执行一遍。
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

}

TEST(LogStreamtest, bools)
{
    muduo::LogStream os;
    const muduo::LogStream::Buffer &buf = os.buffer();
    EXPECT_EQ(buf.toString(), string(""));
    os << true;
    EXPECT_EQ(buf.toString(), "1");
    os << '\n';
    EXPECT_EQ(buf.toString(), "1\n");
    os << false;
    EXPECT_EQ(buf.toString(), "1\n0");
}

TEST(LogStreamtest, INTeger)
{
    muduo::LogStream os;
    const muduo::LogStream::Buffer &buf = os.buffer();
    EXPECT_EQ(buf.toString(), string(""));
    os << 1;
    EXPECT_EQ(buf.toString(), string("1"));
    os << 0;
    EXPECT_EQ(buf.toString(), string("10"));
    os << -1;
    EXPECT_EQ(buf.toString(), string("10-1"));

    os.resetBuffer();

    os << 0 << " " << 123 << 'x' << 0x64;
    EXPECT_EQ(buf.toString(), string("0 123x100"));
}

TEST(LogStreamtest, INTLimits)
{
    muduo::LogStream os;
    const muduo::LogStream::Buffer &buf = os.buffer();
    os << -2147483647;
    EXPECT_EQ(buf.toString(), string("-2147483647"));

    os << static_cast<int>(-2147483647 - 1);

    EXPECT_EQ(buf.toString(), string("-2147483647-2147483648"));

    os << ' ';

    os << 2147483647;
    EXPECT_EQ(buf.toString(), string("-2147483647-2147483648 2147483647"));

    os.resetBuffer();
    os << std::numeric_limits<int16_t>::min();
    EXPECT_EQ(buf.toString(), string("-32768"));

    os.resetBuffer();

    os.resetBuffer();
    os << std::numeric_limits<int16_t>::max();
    EXPECT_EQ(buf.toString(), string("32767"));

    os.resetBuffer();

    os << std::numeric_limits<uint16_t>::min();
    EXPECT_EQ(buf.toString(), string("0"));
    os.resetBuffer();


    os << std::numeric_limits<uint16_t>::max();
    EXPECT_EQ(buf.toString(), string("65535"));
    os.resetBuffer();


    os << std::numeric_limits<int32_t>::min();
    EXPECT_EQ(buf.toString(), string("-2147483648"));
    os.resetBuffer();


    os << std::numeric_limits<int32_t>::max();
    EXPECT_EQ(buf.toString(), string("2147483647"));
    os.resetBuffer();

    os << std::numeric_limits<int64_t>::min();
    EXPECT_EQ(buf.toString(), string("-9223372036854775808"));
    os.resetBuffer();


    os << std::numeric_limits<uint64_t>::max();
    EXPECT_EQ(buf.toString(), string("18446744073709551615"));
    os.resetBuffer();

    int16_t a = 0;
    int32_t b = 0;
    int64_t c = 0;

    os << a;
    os << b;
    os << c;
    EXPECT_EQ(buf.toString(), string("000"));
}


TEST(LogStreamtest, Floats)
{
    muduo::LogStream os;
    const muduo::LogStream::Buffer &buf = os.buffer();
    os << 0.0;
    EXPECT_EQ(buf.toString(), string("0"));

    os.resetBuffer();
    os << 1.0;
    EXPECT_EQ(buf.toString(), string("1"));

    os.resetBuffer();
    os << 0.1;
    EXPECT_EQ(buf.toString(), string("0.1"));
    os.resetBuffer();

    os << 0.5;
    EXPECT_EQ(buf.toString(), string("0.5"));
    os.resetBuffer();

    os << 0.001;
    EXPECT_EQ(buf.toString(), string("0.001"));
    os.resetBuffer();

    double a = 0.1;
    os << a;
    EXPECT_EQ(buf.toString(), string("0.1"));
    os.resetBuffer();

    double b = 0.23;
    os << b;
    EXPECT_EQ(buf.toString(), string("0.23"));

    os.resetBuffer();
    os << a + b;
    EXPECT_EQ(buf.toString(), string("0.33"));

    os.resetBuffer();
    os << 1.23456789;
    EXPECT_EQ(buf.toString(), string("1.23456789"));
    os.resetBuffer();
    os << 1.234567890123456789;
    EXPECT_EQ(buf.toString(), string("1.23456789012"));//只保留12位.

    os.resetBuffer();
    os << -1.234567890123456789;
    EXPECT_EQ(buf.toString(), string("-1.23456789012"));
}

TEST(LogStreamtest, Void)
{
    muduo::LogStream os;

    const muduo::LogStream::Buffer &buf = os.buffer();

    os << static_cast<void *>(0);

    EXPECT_EQ(buf.toString(), string("0x0"));

    os.resetBuffer();
    os << reinterpret_cast<void *>(8888);
    EXPECT_EQ(buf.toString(), string("0x22B8"));//,hex(8888)=0x22b8。
}

TEST(LogStreamtest,String)
{
    muduo::LogStream os;
    const muduo::LogStream::Buffer & buf=os.buffer();
    os<<"Hello";
    EXPECT_EQ(buf.toString(),string("Hello"));

    string s=" \n\0muduo";
    os<<s;
    EXPECT_EQ(buf.toString(),"Hello"+s);
}


TEST(LogStreamtest,fmt)
{
    muduo::LogStream os;
    const muduo::LogStream::Buffer& buf=os.buffer();
    os<<muduo::Fmt("%4d",1);
    EXPECT_EQ(buf.toString(),string("   1"));
    os.resetBuffer();
    os<<muduo::Fmt("%4.2f",1.2);

    EXPECT_EQ(buf.toString(),string("1.20"));
    os<<muduo::Fmt("%5d",43);
    EXPECT_EQ(buf.toString(),string("1.20   43"));

    os<<muduo::Fmt("%2d",4321);
    EXPECT_EQ(buf.toString(),string("1.20   434321"));

  //  printf("%2d",4321);
  //  printf("%4d",43);

}


TEST(LogStreamtest,Long)
{
    muduo::LogStream os;
    const muduo::LogStream::Buffer &buf = os.buffer();
    for (int i = 0; i < 399; ++i)
    {
        os<<"123456789 ";
        EXPECT_EQ(buf.length(),10*(i+1));
        EXPECT_EQ(buf.avail(),4000-10*(i+1));
    }

    os<<"abcdefghi ";
    EXPECT_EQ(buf.length(),3990);
    EXPECT_EQ(buf.avail(),10);
    os<<"abcdefghi";
    EXPECT_EQ(buf.length(),3999);
    EXPECT_EQ(buf.avail(),1);
}
