
//http://coolshell.cn/articles/10478.html

#ifndef TMUDUO_STRING_H
#define TMUDUO_STRING_H

#include <vector>
#include <memory>

class String
{
public:
    String();

    String( const char * chars);


    String(const char * chars, size_t len);
    String(String &rhs);

    String(String &&rhs);

    String &operator=(String& rhs);
    String&operator=(String&& rhs);
    String&operator=(const char* chars);
    bool operator==(const char * chars)  const;
    bool operator==(const String & rhs)  const;

    char * c_str()
    {
        return &(*str_ptr_)[0];
    }
    ~String();

private:
    size_t len_ = 0;
    std::unique_ptr<std::vector<char> > str_ptr_= nullptr;

};

#endif //TMUDUO_STRING_H
