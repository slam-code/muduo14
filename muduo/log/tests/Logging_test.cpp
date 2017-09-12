

#include <muduo/log/Logging.h>
#include <muduo/log/LogFile.h>
#include <stdio.h>
#include <thread>
#include <zconf.h>

int g_total;
FILE *g_file;

std::unique_ptr<muduo::LogFile> g_logfile_ptr;

void dummyOutput(const char *msg, int len)
{
    g_total += len;
    if (g_file)
        fwrite(msg, 1, len, g_file);
    else if (g_logfile_ptr)
        g_logfile_ptr->append(msg, len);
}

void bench(const char *type)
{
    muduo::Logger::setOutput(dummyOutput);
    muduo::ChronoTimestamp start(muduo::ChronoTimestamp::now());
    g_total = 0;
    int n = 1000 * 1000;
    const bool kLongLog = false;
    std::string empty=" ";
    std::string longStr(3000,'x');
    longStr+=" ";
    for (int i = 0; i < n; ++i)
    {
        LOG_INFO<<"Hello 0123456789"<<" abcdefghijklmnopqrstuvwxyz"<<(kLongLog?longStr:empty)<<i;
    }
    muduo::ChronoTimestamp end(muduo::ChronoTimestamp::now());
    double seconds=muduo::timeDifference(end,start);
    printf("%12s: %f seconds, %d bytes, %10.2f msg/s, %.2f MiB/s \n",
        type,seconds,g_total,n/seconds,g_total/seconds/(1204*1024));
}


void logInThread()
{
    LOG_INFO<<"logInThread";
    usleep(1000);
}

int main()
{
    getpid();
    LOG_TRACE<<"trace";
    LOG_DEBUG<<"debug";
    LOG_INFO<<"Hello";
    LOG_WARM<<"World";
    LOG_ERROR<<"Error";

    LOG_INFO<< sizeof(muduo::Logger);
    LOG_INFO<< sizeof(muduo::LogStream);

    LOG_INFO<< sizeof(muduo::Fmt);
    LOG_INFO<< sizeof(muduo::LogStream::Buffer);

    sleep(1);
    bench("nop");

    char buffer[64*1024];

    g_file=fopen("/dev/null","w");
    setbuffer(g_file,buffer, sizeof(buffer));
    bench("/dev/null");
    fclose(g_file);

    // g_file=fopen("/tmp/log","w");
    g_file=fopen("log","w");
    setbuffer(g_file,buffer, sizeof(buffer));
    bench("tmp/log");
    fclose(g_file);

    g_file=NULL;
    g_logfile_ptr.reset(new muduo::LogFile("test_log_st",500*1000*1000, false));//110MB翻滚一次．char 是一个字节．
    bench("test_log_st");

    g_logfile_ptr.reset(new muduo::LogFile("test_log_mt",500*1000*1000, true));
    bench("test_log_mt");//mutex
    g_logfile_ptr.reset();


    return 0;
}


/*
./logging_test
20170515 07:31:18.506456Z   594 INFO  Hello - Logging_test.cc:68
20170515 07:31:18.506591Z   594 WARN  World - Logging_test.cc:69
20170515 07:31:18.506600Z   594 ERROR Error - Logging_test.cc:70
20170515 07:31:18.506604Z   594 INFO  4056 - Logging_test.cc:71
20170515 07:31:18.506609Z   594 INFO  4024 - Logging_test.cc:72
20170515 07:31:18.506526Z   595 INFO  logInThread - Logging_test.cc:50
20170515 07:31:18.506469Z   596 INFO  logInThread - Logging_test.cc:50
20170515 07:31:18.506485Z   597 INFO  logInThread - Logging_test.cc:50
20170515 07:31:18.506489Z   599 INFO  logInThread - Logging_test.cc:50
20170515 07:31:18.506469Z   598 INFO  logInThread - Logging_test.cc:50
20170515 07:31:18.506625Z   594 INFO  36 - Logging_test.cc:73
20170515 07:31:18.506775Z   594 INFO  4016 - Logging_test.cc:74
         nop: 0.316091 seconds, 109888890 bytes, 3163645.91 msg/s, 331.54 MiB/s
   /dev/null: 0.279891 seconds, 109888890 bytes, 3572819.42 msg/s, 374.43 MiB/s
    /tmp/log: 0.321212 seconds, 109888890 bytes, 3113208.72 msg/s, 326.26 MiB/s
 test_log_st: 0.861579 seconds, 109888890 bytes, 1160659.67 msg/s, 121.64 MiB/s
 test_log_mt: 0.845119 seconds, 109888890 bytes, 1183265.32 msg/s, 124.00 MiB/s
20170515 15:31:23.163470   594 INFO  Hello CST - Logging_test.cc:106
20170515 15:31:23.163505   594 WARN  World CST - Logging_test.cc:107
20170515 15:31:23.163511   594 ERROR Error CST - Logging_test.cc:108
20170515 03:31:24.165244   594 INFO  Hello NYT - Logging_test.cc:115
20170515 03:31:24.165270   594 WARN  World NYT - Logging_test.cc:116
20170515 03:31:24.165273   594 ERROR Error NYT - Logging_test.cc:117
timezone nop: 0.314587 seconds, 108888890 bytes, 3178770.90 msg/s, 330.10 MiB/s

 */

/*
./Lo
20170515 07:42:26.886936Z 3032 INFO  Hello - Logging_test.cpp:57
20170515 07:42:26.887011Z 3032 WARN  main World - Logging_test.cpp:58
20170515 07:42:26.887016Z 3032 ERROR main Error - Logging_test.cpp:59
20170515 07:42:26.887019Z 3032 INFO  4056 - Logging_test.cpp:61
20170515 07:42:26.887022Z 3032 INFO  4024 - Logging_test.cpp:62
20170515 07:42:26.887025Z 3032 INFO  36 - Logging_test.cpp:64
20170515 07:42:26.887027Z 3032 INFO  4016 - Logging_test.cpp:65
         nop: 0.474831 seconds, 109888890 bytes, 2106012.45 msg/s, 187.71 MiB/s
   /dev/null: 0.428326 seconds, 109888890 bytes, 2334670.32 msg/s, 208.09 MiB/s
     tmp/log: 0.983630 seconds, 109888890 bytes, 1016642.44 msg/s, 90.61 MiB/s
 test_log_st: 1.029678 seconds, 109888890 bytes,  971177.40 msg/s, 86.56 MiB/s
 test_log_mt: 1.011526 seconds, 109888890 bytes,  988605.33 msg/s, 88.12 MiB/s
*/