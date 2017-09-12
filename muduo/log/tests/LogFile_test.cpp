

#include <muduo/log/LogFile.h>
#include <muduo/log/Logging.h>
#include <zconf.h>
#include <fcntl.h>

std::unique_ptr<muduo::LogFile> g_logfile_ptr;

void outputFunc(const char *msg, int len)
{
        g_logfile_ptr->append(msg,len);
}

void flushFunc()
{
    g_logfile_ptr->flush();
}

int main(int argc,char* argv[])
{
    char name[256];
    strncpy(name,argv[0],256);
    g_logfile_ptr.reset(new muduo::LogFile(::basename(name),200*1000));
    muduo::Logger::setOutput(outputFunc);
    muduo::Logger::setFlush(flushFunc);
    std::string line="1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    //using namespace muduo;
    for (int i = 0; i < 10; ++i)
    {
        LOG_INFO<<line<<i;
        usleep(1000);
    }

/*    std::string line="1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ\0 ";

    FILE* fp_=(::fopen("linetry.txt","ae"));

    char buff[line.size()];
    ::setbuffer(fp_, buff, sizeof buff);
    auto it=::fwrite_unlocked(line.c_str(), 1, line.size(), fp_);// thread unsafe*/
    return 0;
}
/*
logfile_test.log:

20170514 14:08:21.858487Z  9952 INFO  1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ 9327 - LogFile_test.cc:28
20170514 14:08:21.859559Z  9952 INFO  1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ 9328 - LogFile_test.cc:28
20170514 14:08:21.860620Z  9952 INFO  1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ 9329 - LogFile_test.cc:28
*/