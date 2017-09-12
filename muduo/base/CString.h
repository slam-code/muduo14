
#ifndef TMUDUO_CSTRING_H
#define TMUDUO_CSTRING_H

#include <string.h>
#include <utility>
#include <stdio.h>

class CString
{

public:
    CString() : data_(new char[1])
    {
        *data_ = '\0';
    };

    CString(const char *rhs)
    {
        data_ = new char[strlen(rhs) + 1];//why +1?
        strcpy(data_, rhs);
    }

    CString(const char *rhs, size_t len)
    {
        data_ = new char[len + 1];
        strncpy(data_, rhs, len);
    }

    CString(const CString &rhs)
    {
        // delete[]data_;
        data_ = new char[rhs.len() + 1];
        strcpy(data_, rhs.data_);
    }

    CString(CString &&rhs)
    {
        // swap(rhs.data_);
        data_ = rhs.data_;
        rhs.data_ = nullptr;
    }

    CString &operator=(const char *rhs)
    {
        delete[]data_;
        data_ = new char[strlen(rhs) + 1];
        printf("==\n");
        if (data_ == rhs)
            return *this;
        strcpy(data_, rhs);
        return *this;
    }

    /*有memory leak，用valgrind查了以后证明的确有，在复制一个String对象的时候，
     * 假如接收的String本身data_不为空，你的String(const char *str, std::size_t len)函数会直接把data_
     * 指向了新allocate的memory而没有释放之前allocate的memory。*/

    CString &operator=(CString rhs)//不用证同测试,why?
    {
        swap(rhs);
       // delete[]rhs.data_;//
       // rhs.data_ = nullptr;
        return *this;
    }

    CString &operator=(CString &&rhs) noexcept
    {
        // swap(data_,rhs.data_);
        data_ = rhs.data_;
        rhs.data_ = nullptr;
        return *this;
    }

    bool operator==(const char *rhs) const
    {
        return strcmp(data_, rhs) == 0;
    }

    bool operator==(CString &rhs) const
    {
        return strcmp(data_, rhs.data_) == 0;
    }

    char *c_str() //const
    {
        return data_;
    }

    const size_t len() const
    {
        return strlen(data_);
    }

    ~CString() noexcept
    {
        delete[] data_;
    }

    void swap(CString &rhs) noexcept
    {
        std::swap(data_, rhs.data_);
    }

private:
    char *data_;
};

#endif //TMUDUO_CSTRING_H

//delegating constructor 委托构造函数