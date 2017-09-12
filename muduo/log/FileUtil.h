

#ifndef TMUDUO_FILEUTIL_H
#define TMUDUO_FILEUTIL_H


#include <muduo/base/StringPiece.h>

namespace muduo
{
    namespace FileUtil
    {
        class ReadSmallFile : noncopyable
        {
        public:
            ReadSmallFile(StringArg filename);

            ~ReadSmallFile();

            template<typename STring>
            int readToString(int maxSize,
                             STring *content, int64_t *fileSize,
                             int64_t *modifyTime,
                             int64_t *createTime);

            int readToBuffer(int *size);

            static const int kBufferSize = 64 * 1024;
        private:
            int fd_;
            int err_;
            char buf_[kBufferSize];
        };

        template<typename STring>
        int readFile(StringArg filename, //欲打开的文件名称．
                     int maxSize,//最大读取字节
                     STring *content,//储存地点．
                     int64_t *fileSize = NULL,//返回file的真实大小
                     int64_t *modifyTime = NULL,//返回修改的日期．
                     int64_t *createTime = NULL)//创建日期．
        {
            ReadSmallFile file(filename);
            return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
        }

        class AppendFile : noncopyable
        {
        public:
            explicit AppendFile(StringArg filename);

            ~AppendFile();

            void append(const char *logline, const size_t len);

            void flush();

            size_t writtenBytes() const
            { return writtenBytes_; }

        private:
            size_t write(const char *logline, size_t len);

            FILE *fp_;
            char buffer_[64 * 1024];
            size_t writtenBytes_;
        };
    }


}

#endif //TMUDUO_FILEUTIL_H
