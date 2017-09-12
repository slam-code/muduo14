

#ifndef TMUDUO_LOGSTREAM_H
#define TMUDUO_LOGSTREAM_H

#include <muduo/base/StringPiece.h>
#include <muduo/base/Types.h>
#include <assert.h>
#include <string.h>
#include <string>

namespace muduo
{
    namespace detail
    {
        const int kSmallBuffer = 4000;//4k
        const int kLargeBuffer = 4000 * 4000;//4M

        template<int SIZE>
        class FixedBuffer : noncopyable
        {

        public:
            FixedBuffer() : cur_(data_)
            {
                setCookie(cookieStart);
            }

            ~FixedBuffer()
            {
                setCookie(cookieEnd);
            }

/*restrict表明指针是访问一个数据对象的唯一且所有修改内容的操作都必须通过该指针来修改*/
            void append(const char * /*restrict*/ buf, size_t len)
            {
                if (implicit_cast<size_t>(avail()) > len)
                {
                    memcpy(cur_, buf, len);
                    cur_ += len;
                }
            }

            const char *data() const
            { return data_; }

            int length() const
            { return static_cast<int >(cur_ - data_); }

            char *current()
            { return cur_; }

            int avail() const
            { return static_cast<int>(end() - cur_); }

            void add(size_t len)
            { cur_ += len; }


            void reset()
            { cur_ = data_; };

            void bzero()
            { ::bzero(data_, sizeof data_); }


            const char *debugString();

            void setCookie(void(*cookie)())
            {
                cookie_ = cookie;
            }

            std::string toString() const
            {
                //from sequence :string (const char* s, size_t n);
                return std::string(data_, length());
            }


            StringPiece toStringPiece() const
            {
                return StringPiece(data_, length());
            }

        private:
            const char *end() const
            { return data_ + sizeof data_; }

            static void cookieStart();

            static void cookieEnd();

            void (*cookie_)();

            char data_[SIZE];
            char *cur_;

        };
    }

    class LogStream : noncopyable
    {
        typedef LogStream self;
    public:
        typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;

        self &operator<<(bool v)
        {
            buffer_.append(v ? "1" : "0", 1);
            return *this;
        }

        self &operator<<(short);

        self &operator<<(unsigned short);

        self &operator<<(unsigned int);

        self &operator<<(int);

        self &operator<<(long);

        self &operator<<(unsigned long);

        self &operator<<(long long);

        self &operator<<(unsigned long long);

        self &operator<<(const void *);

        self &operator<<(float v)
        {
            *this << static_cast<double >(v);
            return *this;
        }

        self &operator<<(double);

        self &operator<<(char v)
        {
            buffer_.append(&v, 1);
            return *this;
        }

        self &operator<<(const char *str)
        {
            if (str)
                buffer_.append(str, strlen(str));
            else
                buffer_.append("(null)", 6);
            return *this;
        }

        self &operator<<(const unsigned char *str)
        {
            return operator<<(reinterpret_cast<const char *>(str));
        }

        self &operator<<(const std::string &v)
        {
            buffer_.append(v.c_str(), v.size());
            return *this;
        }

        self &operator<<(const StringPiece &v)
        {
            buffer_.append(v.data(), v.size());
            return *this;
        }

        self &operator<<(const Buffer &v)
        {
            *this << v.toStringPiece();
            return *this;
        }

        void append(const char *data, int len)
        {
            buffer_.append(data, len);
        }

        const Buffer &buffer() const
        { return buffer_; }

        void resetBuffer()
        {
            buffer_.reset();
        }


    private:

        void staticCheck();

        template<typename T>
        void formatInteger(T);

        Buffer buffer_;
        static const int kMaxNumbericSize = 32;
    };

    class Fmt
    {
    public:
        template<typename T>
        Fmt(const char *fmt, T val)
        {
            //算术
            static_assert(std::is_arithmetic<T>::value == true, "Must be arithmetic type");

            length_ = snprintf(buf_, sizeof buf_, fmt, val);

            assert(static_cast<size_t>(length_) < sizeof buf_);
        };

        const char *data() const
        { return buf_; }

        int length() const
        { return length_; }


    private:
        char buf_[32];
        int length_;
    };

    inline LogStream &operator<<(LogStream &s, const Fmt &fmt)
    {
        s.append(fmt.data(), fmt.length());
        return s;
    }
}
#endif //TMUDUO_LOGSTREAM_H
