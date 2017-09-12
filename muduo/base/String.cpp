

#include <cstdio>
#include "String.h"

using namespace std;

String::String() :
        len_(0), str_ptr_(make_unique<vector<char >>())
{
}

String::String(const char *chars) : String()
{
    str_ptr_->clear();
    for (int i = 0; (chars + i) != nullptr && *(chars + i) != '\0'; ++i)
    {

        str_ptr_->push_back(*(chars + i));
    }
}

String::String(const char *chars, size_t len) : String()
{
    str_ptr_->clear();
    for (int i = 0; i < len; ++i)
    {
        str_ptr_->push_back(*(chars + i));
    }
}

String::String(String &rhs) : String()
{
    str_ptr_->clear();/*
    for (auto it = rhs.str_ptr_->begin(); it != rhs.str_ptr_->end(); ++it)
        str_ptr_->push_back(*it);*/
    (*str_ptr_)=(*rhs.str_ptr_);
}

String::String(String &&rhs) : String()
{

    str_ptr_ = std::move(rhs.str_ptr_);
    rhs.str_ptr_= nullptr;
}

String &String::operator=(String &rhs)
{
    str_ptr_->clear();/*
    for (auto it = rhs.str_ptr_->begin(); it != rhs.str_ptr_->end(); ++it)
        str_ptr_->push_back(*it);*/
    (*str_ptr_) = (*rhs.str_ptr_);
    return *this;
}

String &String::operator=(String &&rhs)
{
    // swap(str_ptr_,rhs.str_ptr_);
    str_ptr_ = std::move(rhs.str_ptr_);
    rhs.str_ptr_= nullptr;
    return *this;
}

String &String::operator=(const char *chars)
{
    str_ptr_->clear();
    for (int i = 0; (chars+i)!= nullptr && *(chars+i)!='\0'; ++i)
    {
        str_ptr_->push_back(*(chars+i));
    }
    return *this;
}

bool String::operator==(const char *chars) const
{
    for (int i = 0; i < str_ptr_->size(); ++i)
    {
        if (*(chars + i) != (*str_ptr_)[i])
            return false;
    }
    return true;
}

bool String::operator==(const String &rhs) const
{
    return (*str_ptr_) == (*rhs.str_ptr_);
}

String::~String()
{

}