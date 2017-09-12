#include <iostream>
using namespace std;
class Outer
{
public:
    Outer(){m_outerInt=0;}
    //内部类定义开始
    class Inner
    {
    public:
        Inner(){m_innerInt=1;}
        void DisplayIn(){cout<<m_innerInt<<endl;}

    private:
        int m_innerInt;
    } ;
    //内部类,end


    void DisplayOut(){cout<<m_outerInt<<endl;}


private:
    int m_outerInt;
};

int main()
{
    Outer out;
    Outer::Inner in;
    out.DisplayOut();
    in.DisplayIn();

    return 0;
}

/*
如上面代码所示，这种情况下，外部类与内部类其实联系并不大，外部类无非仅仅限定了内部类类名的作用域范围，
完全可以加上Outer限定之后像使用任何其他类一样来使用内部类，Outer于Inner而言仅仅是一种命名空间。
提问：上面代码中，内部类（Inner）成员函数（比如DisplayIn）如何访问外部类（Outer）数据成员呢？
答：问这个问题之前，先要明白一个事实：将来你是在一个Inner实例对象上调用Inner的成员函数的，而所谓的“访问外部类数据成员”
这种说法是不合理的，“外部类”及任何类，只是代码而已，是一种说明，从内存的角度来讲，程序运行起来之后，代码存储在代码区，
所以应该问“如何访问外部类实例的数据成员”，如此，你得先有一个外部类实例（或者实例的指针），然后才能谈访问。
*/


/*内部类如果想达到友元访问效果（直接通过实例或者实例指针来访问实例的非公有成员），是不需要另外再声明为friend的，
原因不言自明：都已经是自己人了。*/


/*

请问宏定义：
#define METHOD_PROLOGUE(theClass, localClass) \
3 theClass* pThis = ((theClass*)((char*)(this) - \
4 offsetof(theClass, m_x##localClass))); \

例子中的变量是存放在栈上的，如果存放在堆上是不是就需要改成
#define METHOD_PROLOGUE(theClass, localClass) \
3 theClass* pThis = ((theClass*)((char*)(this) + \
4 offsetof(theClass, m_x##localClass))); \


*/

//提问：内部类实例（作为外部类的数据成员）如何访问外部类实例的成员呢:
#include <iostream>
#define METHOD_PROLOGUE(theClass, localClass) \
    theClass* pThis = ((theClass*)((char*)(this) - \
    offsetof(theClass, m_x##localClass))); \

using namespace std;

class Outer
{
public:
    Outer(){m_outerInt=0;}
private:
    int m_outerInt;
public:
    //内部类定义开始
    class Inner
    {
    public:
        Inner(){m_innerInt=1;}
    private:
        int m_innerInt;
    public:
        void DisplayIn(){cout<<m_innerInt<<endl;}
        // 在此函数中访问外部类实例数据
        void setOut()
        {
            METHOD_PROLOGUE(Outer,Inner);
            pThis->m_outerInt=10;
        }
    } m_xInner;
    //End内部类
public:
    void DisplayOut(){cout<<m_outerInt<<endl;}
};

int main()
{
    Outer out;
    out.DisplayOut();
    out.m_xInner.setOut();
    out.DisplayOut();
    return 0;
}

/*看main函数：程序执行完main函数第一句后，内存中便有了一个数据块，它存储着out的数据，而m_xInner也在数据块中，
当然，&out和this指针（外部类）都指向该内存块的起始位置，而内部类代码中的this指针当然就指向m_xInner的起始内存了，
offsetof(theClass, m_x##localClass)获得的便是m_xInner在该内存块中与该内存块起始地址（这正是out的地址）的距离（偏移），
即内部类this-外部类this的差值（以字节为单位）这样，用内部类this减去其自身的偏移，便可得到pThis。有了out的地址，
基本上可以对其为所欲为了，至于为何要有char*强转，可以go to definition of offsetof，可以看到其实现中有个关于char的转换。*/



/*
提问：嵌套类为什么可以访问外围类的私有静态函数而不可访问非静态函数？
因为嵌套类默认是外部类的友元类，所以可以访问其私有成员，这其中就包含了外部类的静态成员。而不可以访问外部类的非静态成员，
这是因为访问非静态成员需要对象，在没有外部类对象的前提下，是不能够通过外部类对象来访问其成员的。如果在内部类中持有外部类的对象，
则可以通过该外部类对象访问外部类中的非静态成员。
*/


/*

C++嵌套类及对外围类成员变量的访问
　　在一个类中定义的类称为嵌套类，定义嵌套类的类称为外围类。

　　定义嵌套类的目的在于隐藏类名，减少全局的标识符，从而限制用户能否使用该类建立对象。这样可以提高类的抽象能力，并且强调了两个类(外围类和嵌套类)之间的主从关系。下面是一个嵌套类的例子：　　　

class A
　{
　
public:
　　　class B
　　　{
　　　public:
　　　　　…
　　　private:
　　　　　…
　　　};
　　void f();
　private:
　　　int a;
　}
　　

     其中，类B是一个嵌套类，类A是外围类，类B定义在类A的类体内。


　　对嵌套类的若干说明：

　　1、从作用域的角度看，嵌套类被隐藏在外围类之中，该类名只能在外围类中使用。如果在外围类的作用域内使用该类名时，需要加名字限定。

　　2、从访问权限的角度来看，嵌套类名与它的外围类的对象成员名具有相同的访问权限规则。不能访问嵌套类的对象中的私有成员函数，也不能对外围类的私有部分中的嵌套类建立对象。

　　3、嵌套类中的成员函数可以在它的类体外定义。

　　4、嵌套类中说明的成员不是外围类中对象的成员，反之亦然。嵌套类的成员函数对外围类的成员没有访问权，反之亦然。国此，在分析嵌套类与外围类的成员访问关系时，往往把嵌套类看作非嵌套类来处理。这样，上述的嵌套类可写成如下格式：

　　class A
　　{
　　public:
　　　　void f();
　　private:
　　　　int a;
　　};

　　class B
　　{
　　public:
　　　　…
　　private:
　　　　…
　　};

　　由引可见，嵌套类仅仅是语法上的嵌入。

　　5、在嵌套类中说明的友元对外围类的成员没有访问权。

　　6、如果嵌套类比较复杂，可以只在外围类中对嵌套类进行说明，关于嵌套的详细的内容可在外围类体外的文件域中进行定义。

      7、嵌套类可以访问外围类的静态成员变量，即使它的访问权限是私有的。

　　但是，如果一定要让嵌套类访问外围类，我们要采取什么样的办法？实际上实现的方法应该有很多种，这里介绍一种参见com实现中实现内嵌类访问外部类数据成员中所使用的方式，具体见《COM本质论》。

//嵌套类访问外围类中的成员变量
  

class GH_A{
　　const static int s_data = 20;
public:
　　int task_id;

　　class GH_B
　　{ 
　　public:
　　　　int attribute;
　　　　GH_A* parent;
　　　　GH_B(){
　　　　attribute = 5;
　　　　parent = (GH_A*)((char*)this-offsetof(GH_A,gh_b));
　　　　printf("外围类的private型静态变量：%d\n",s_data);
　　　　}
　　} gh_b; 
};

　　

//嵌套类访问外围类成员变量的示例
　　GH_A aaaa;
　　aaaa.gh_b.parent->task_id = 16;
　　printf("外围类成员变量值被嵌套类访问并修改之后的值:%d\n",aaaa.task_id);
　　aaaa.task_id = aaaa.gh_b.attribute;
　　printf("嵌套类访问到外围类成员变量值:%d\n",aaaa.gh_b.parent->task_id);

　　

输出结果：




这里，我们用到一个C++库函数，原型是：
size_t offsetof(
      structName,
      memberName
);
以下是MSDN上的一些介绍：

Parameters

structName
Name of the parent data structure.

memberName
Name of the member in the parent data structure for which to determine the offset.

Return Value
offsetof returns the offset in bytes of the specified member from the beginning of its parent data structure. It is undefined for bit fields.

 

此外，再来介绍一下局部类

　　在一个函数体内定义的类称为局部类。局部类中只能使用它的外围作用域中的对象和函数进行联系，因为外围作用域中的变量与该局部类的对象无关。局部类不能被外部所继承。在定义局部类时需要注意：局部类中不能说明静态成员函数，并且所有成员函数都必须定义在类体内。在实践中，局部类是很少使用的。下面是一个局部类的例子。

int a;
void fun()
{
static int s;
class A
{
　　public:
　　void init(int i)
 { s = i; }
};
A
 m;
m.init(10);
}
　　局部类的另一个用途是用来实现类型转化，如下代码：

class Interface
{
public:
   virtual void Fun()
 = 0;  
};
 
template <class T,
class P>
Interface*
 MakeAdapter(const T&
 obj, const P&
 arg)
{
    int x;
   class Local
 : public Interface
   {
   public:
      Local(const T&
 obj, const P&
 arg)
         :
 obj_(obj), arg_(arg) {}
      virtual void Fun()
      {
          x
 = 100;
         obj_.Call(arg_);
      }
   private:
      T
 obj_;
      P
 arg_;
   };
   return new Local(obj,
 arg);
}


*/