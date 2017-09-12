

#ifndef TMUDUO_PROCESSINFO_H
#define TMUDUO_PROCESSINFO_H

#include <muduo/base/StringPiece.h>
#include <muduo/base/Types.h>
#include <muduo/base/ChronoTimestamp.h>
#include <string>
#include <vector>
#include <sys/times.h>

namespace muduo
{
    namespace ProcessInfo
    {
        pid_t pid();

        string pidString();

        uid_t uid();

        std::string username();

        uid_t euid();

        ChronoTimestamp startTime();

        int clockTicksPerSecond();

        int pageSize();

        bool isDebugBuild();


        std::string hostname();

        std::string procname();

        StringPiece procname(const std::string &stat);


        // read /proc/self/stats
        std::string procStatus();

        // read /proc/self/stat
        std::string procStat();

        // read /proc/self/task/tid/stat
        std::string threadStat();

        std::string exePath();

        int openedFiles();

        int maxOpenFiles();

        class CpuTime
        {
        public:

            CpuTime() :
                    userSeconds(0.0), systemSeconds(0.0)

            {}
            double userSeconds;
            double systemSeconds;
        };

        CpuTime cpuTime();
        int numThreads();
        std::vector<pid_t >threads();
    }
}
#endif //TMUDUO_PROCESSINFO_H
