

#include <CString.h>
#include <iostream>
#include <vector>

/*int main()
{
    CString s1;
    s1=std::move("123");
    return 0;
}*/
//#define CString std::string

void foo(CString x)
{
}

void bar(const CString& x)
{
}

CString baz()
{
    CString ret("world");
    return ret;
}

int main()
{
    CString s0;
    CString s1("hello");
    CString s2(s0);
    CString s3 = s1;
    s2 = s1;

    foo(s1);
    bar(s1);
    foo("temporary");
    bar("temporary");
    CString s4 = baz();

    std::vector<CString> svec;
    svec.push_back(s0);
    svec.push_back(s1);
    svec.push_back(baz());
    svec.push_back("good job");
}