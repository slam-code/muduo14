

#include <vector>
#include <iostream>

using namespace std;

static int count = 0;

/*class test
{
public:
    test(int i = 0)
    {
        value_ = i;
        ++count;
        cout << "test(),i="<<value_<<" " << count << " \n";
    }

    ~test()
    {

        cout << "~test(),i="<<value_<<" " << count << " \n";
        --count;
    }

    test(test& test1)
    {
        this->value_=test1.value_;
        cout<<"test(test& test) \n";
    }
    test(test&& test1 )
    {
       *this=std::move(test1);
        *this=std::move(test1);
        cout<<"test(test&& test) \n";
    }

    test& operator =(const  test& test2)
    {
        this->value_=test2.value_;
        cout<<"operator=(test& test1) \n";
        return *this;
    }

    test& operator =( test&& test2)
    {
        *this=std::move(test2);
        cout<<"operator=(test& test1) \n";
        return *this;
    }

    int value_ = 0;
};
 int main()
{
    test t1()
    vector<test> v1{1, 2, 3};
    vector<test> v2{9, 8, 7};

    test i(0);
    test j(-1);

    cout<<"**--**\n\n";
    v1.push_back(std::move(j));//此后,对j的值不应该做任何假设．

    i = std::move(v2.back());//此后,对v2.back()的值不应该做任何假设．move没有影响v2的size()大小．


    cout<<"**print**\n\n";
    for (auto &it:v1)
        cout << it.value_ << " ,";
    cout << "\n";
    for (auto &it:v2)
        cout << it.value_ << " ,";
    cout << "i=" << i.value_ << " j=" << j.value_ << endl;


    cout << "------\n";

    v2.back();

    for (auto &it:v1)
        cout << it.value_ << " ,";
    cout << "\n";
    for (auto &it:v2)
        cout << it.value_ << " ,";
    cout << "i=" << i.value_ << " j=" << j.value_ << endl;

    cout << "v2.size()= " << v2.size() << endl;
}
 */


class test
{
public:
    test()
    {
        ++count;
        cout << "test()" << value_ << " " << count << " \n";
    }

    test(int i = 0)
    {
        value_ = i;
        ++count;
        cout << "test(int ),i=" << value_ << " " << count << " \n";
    }

    ~test()
    {

        cout << "~test(),i=" << value_ << " " << count << " \n";
        --count;
    }

    test(const test &test1)
    {
        this->value_ = test1.value_;
        cout << "test(test& test) \n";
    }

    test(test &&test1) noexcept
    {
        //*this = std::move(test1);
        this->value_=(test1.value_);
        cout << "test(test&& test) \n";
    }

    test &operator=(const test &test2)
    {
        if(&test2==this)
            return *this;
        this->value_ = test2.value_;
        cout << "operator=(test& test1) \n";
        return *this;
    }

    test &operator=(test &&test2) noexcept
    {
        if(&test2==this)
            return *this;
        this->value_ = std::move(test2.value_);
        cout << "operator=(test&& test1) \n";
        return *this;
    }

    int value_ = 0;
};

int main()
{
    test t1(1), t2(2), t3(3), t9(9), t8(8), t7(7);

    vector<test> v1;
    v1.push_back(t1);
    v1.push_back(t2);
    v1.push_back(t3);
    vector<test> v2;
    v2.push_back(t9);
    v2.push_back(t8);
    v2.push_back(t7);

    test i(0);
    test j(-1);

    cout << "**--**\n\n";
    v1.push_back(std::move(j));//此后,对j的值不应该做任何假设．

    i = std::move(v2.back());//此后,对v2.back()的值不应该做任何假设．move没有影响v2的size()大小．


    cout << "**print**\n\n";
    for (auto &it:v1)
        cout << it.value_ << " ,";
    cout << "\n";
    for (auto &it:v2)
        cout << it.value_ << " ,";
    cout << "i=" << i.value_ << " j=" << j.value_ << endl;


    cout << "------\n";

    v2.back();

    for (auto &it:v1)
        cout << it.value_ << " ,";
    cout << "\n";
    for (auto &it:v2)
        cout << it.value_ << " ,";
    cout << "i=" << i.value_ << " j=" << j.value_ << endl;

    cout << "v2.size()= " << v2.size() << endl;
}

/*std::vector 要求保存的类型要可以调用”拷贝构造函数“和”赋值运算符函数”。
4.解决办法
1、不要在类中用const 成员
2、不要企图在vector中，使用含有const成员的类型
3、使用支持C++2011标准的编译器，C++2011标准要求，类型可以“赋值”，或者可以”拷贝“即可。
 */


int main1()
{
    vector<int> v1{1, 2, 3};
    vector<int> v2{9, 8, 7};

    int i = 0;
    int j = -1;

    v1.push_back(std::move(j));//此后,对j的值不应该做任何假设．

    i = std::move(v2.back());//此后,对v2.back()的值不应该做任何假设．move没有影响v2的size()大小．

    for (auto &it:v1)
        cout << it << " ,";
    cout << "\n";
    for (auto &it:v2)
        cout << it << " ,";
    cout << "i=" << i << " j=" << j << endl;


    cout << "------\n";

    v2.back();

    for (auto &it:v1)
        cout << it << " ,";
    cout << "\n";
    for (auto &it:v2)
        cout << it << " ,";
    cout << "i=" << i << " j=" << j << endl;

    cout << "v2.size()= " << v2.size() << endl;
    return 0;
}