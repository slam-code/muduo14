
#include <muduo/base/ChronoTimestamp.h>
#include <muduo/log/Logging.h>
#include <muduo/log/AsyncLogging.h>
#include <sys/resource.h>

int kRollSize = 500 * 1000 * 1000;

muduo::AsyncLogging *g_asyncLog = NULL;

void asyncOutput(const char *msg, int len)
{
    g_asyncLog->append(msg,len);
}

void bench(bool logLog)
{
    muduo::Logger::setOutput(asyncOutput);
    int cnt =0;
    const int kBatch=1000;
    std::string longStr(3000,'x');
    longStr+=" ";
    std::string empty="";
    for (int i = 0; i < 30; ++i)
    {
        muduo::ChronoTimestamp start=muduo::ChronoTimestamp::now();
        for (int j = 0; j < kBatch; ++j)
        {
            LOG_INFO<<"Hello 0123456789"<<" abcdefghijklmnopqrstuvwxyz "
                    <<(logLog?longStr:empty)<<cnt;
            ++cnt;
        }

        muduo::ChronoTimestamp end=muduo::ChronoTimestamp::now();
        printf("one message need us= %f \n",muduo::timeDifference(end,start)*1000000/kBatch);//每写入1条日志花费多少微秒．
        struct  timespec ts={0,500*1000*1000};
        nanosleep(&ts,NULL);
    }
}

int main(int argc,char *argv[])
{
    {
        size_t kOneGB=1000*1024*1024;
        rlimit rl={2*kOneGB,2*kOneGB};//set max virtual memory to 2GB
        setrlimit(RLIMIT_AS,&rl);
    }

    printf("pid= %d \n",getpt());

    char name[256];
    strncpy(name,argv[0],256);
    muduo::AsyncLogging log(::basename(name),kRollSize);
    log.start();
    g_asyncLog=&log;
    bool longlog=argc>1;
    bench(longlog);
    return 0;
}