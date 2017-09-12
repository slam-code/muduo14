

#ifndef TMUDUO_GZIPFILE_H
#define TMUDUO_GZIPFILE_H

#include <muduo/base/noncopyable.h>
#include <muduo/base/StringPiece.h>
#include <zlib.h>

namespace muduo
{
    class GzipFile : noncopyable
    {
    public:
        GzipFile(GzipFile &&rhs)
        {
            rhs.file_ = nullptr;
        }

        ~GzipFile()
        {
            if (file_)
                ::gzclose(file_);
        }
        GzipFile&operator=(GzipFile&& rhs)
        {
            swap(rhs);
            return *this;
        }

        bool valid()const
        {
            return file_!= nullptr;
        }

        void swap(GzipFile& rhs)
        {
            std::swap(file_,rhs.file_);
        }

        bool setBuffer(int size)
        {
            return ::gzbuffer(file_,size)==0;
        }

        int read(void* buf,int len)
        {
           return  ::gzread(file_,buf,len);
        }

        int write(StringPiece buf)
        {
            return   ::gzwrite(file_,buf.data(),buf.size());
        }

        off_t  tell() const
        {
            return ::gztell(file_);
        }
        int flush(int f)
        {
            return ::gzflush(file_,f);
        }

        static GzipFile openForRead(StringArg filename)
        {
            return GzipFile(::gzopen(filename.c_str(),"rbe"));
        }

        static GzipFile openForAppend(StringArg filename)
        {
            return GzipFile(::gzopen(filename.c_str(),"abe"));
        }

        static GzipFile openForWriteExclusive(StringArg filename)
        {
            return GzipFile(::gzopen(filename.c_str(),"wbxe"));
        }

        static GzipFile openForWriteTruncate(StringArg filename)
        {
            return GzipFile(::gzopen(filename.c_str(),"wbe"));
        }

    private:
        explicit GzipFile(gzFile file) :
                file_(file)
        {}

        gzFile file_;
    };
}
#endif //TMUDUO_GZIPFILE_H
