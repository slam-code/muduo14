
#include <muduo/log/FileUtil.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <zconf.h>
#include <string>

using namespace muduo;

FileUtil::AppendFile::AppendFile(StringArg filename)
        : fp_(::fopen(filename.c_str(), "ae")),//a:append,e:O_CLOEXEC
          writtenBytes_(0)
{
    assert(fp_);
    ::setbuffer(fp_, buffer_, sizeof buffer_);
}

FileUtil::AppendFile::~AppendFile()
{
    ::fclose(fp_);
}

void FileUtil::AppendFile::append(const char *logline, const size_t len)
{
    size_t n = write(logline, len);
    size_t remain = len - n;
    while (remain > 0)
    {
        size_t x = write(logline + n, remain);
        if (x == 0)
        {
            int err = ferror(fp_);
            if (err)
                fprintf(stderr, "AppendFile::append() FAILED %s \n", "??");

            break;
        }
        n += x;
        remain = len - n;
    }
    writtenBytes_ += len;
}

void FileUtil::AppendFile::flush()
{
    ::fflush(fp_);
}

size_t FileUtil::AppendFile::write(const char *logline, size_t len)
{
     auto it=::fwrite_unlocked(logline, 1, len, fp_);// thread unsafe
    return it;
}
//man 2 open
FileUtil::ReadSmallFile::ReadSmallFile(StringArg filename)
        :  err_(0)
{

    fd_=(::open(filename.c_str(), O_RDONLY | O_CLOEXEC));
    buf_[0] = '\0';
    if (fd_ < 0)
        err_ = errno;
}

FileUtil::ReadSmallFile::~ReadSmallFile()
{
    if (fd_ > 0)
        ::close(fd_);
}

template<typename STring>
int FileUtil::ReadSmallFile::readToString(int maxSize, STring *content, int64_t *fileSize, int64_t *modifyTime,
                                          int64_t *createTime)
{
    static_assert(sizeof(off_t) == 8, "_FILE_OFFSET_BITS = 64");
    assert(content != NULL);
    int err = err_;
    if (fd_ >= 0)
    {
        content->clear();
        if (fileSize)
        {
            struct stat statbuf;
            if (::fstat(fd_, &statbuf) == 0)
            {
                if (S_ISREG(statbuf.st_mode))
                {
                    *fileSize = statbuf.st_size;
                    content->reserve(static_cast<int >
                                     (std::min(implicit_cast<int64_t>(maxSize),
                                               *fileSize)));
                } else if (S_ISDIR((statbuf.st_mode)))
                {
                    err = EISDIR;
                }
                if (modifyTime)
                {
                    *modifyTime = statbuf.st_mtime;
                }
                if (createTime)
                {
                    *createTime = statbuf.st_ctime;
                }
            } else
            {
                err = errno;
            }
        }
        while (content->size() < implicit_cast<size_t>(maxSize))
        {
            size_t toRead = std::min(implicit_cast<size_t>(maxSize) - content->size(),
                                     sizeof(buf_));
            ssize_t n = ::read(fd_, buf_, toRead);
            if (n > 0)
                content->append(buf_, n);
            else
            {
                if (n < 0)
                { err = errno; }
                break;
            }
        }
    }
    return err;//返回错误码
}

int FileUtil::ReadSmallFile::readToBuffer(int *size)
{
    int err = err_;
    if (fd_ >= 0)
    {
        ssize_t n = ::pread(fd_, buf_, sizeof(buf_) - 1, 0);
        if (n >= 0)
        {
            if (size)
            {
                *size = static_cast<int>(n);
            }
            buf_[n] = '\0';
        } else
            err = errno;
    }
    return err;
}

//特化std::string
template int FileUtil::readFile(StringArg filename,
                                int maxSize,
                                std::string *content,
                                int64_t *fileSize = NULL,
                                int64_t *modifyTime = NULL,
                                int64_t *createTime = NULL);

template
int FileUtil::ReadSmallFile::readToString(int maxSize,
                                          std::string *content,
                                          int64_t *fileSize,
                                          int64_t *modifyTime,
                                          int64_t *createTime);

