
#include <muduo/log/FileUtil.h>
#include <stdio.h>
#include <inttypes.h>
#include <string>
#include <iostream>
using namespace muduo;
using namespace std;

/*
 * err兼容c的含义．
 * */
int main()
{
    string result;
    int64_t size=0;
    int err = FileUtil::readFile("/proc/self",1024,&result,&size);//Is a directory
    printf("%d %zd %ld\n",err,result.size(),size);

    err=FileUtil::readFile("/proc/self",1024,&result,NULL);////Is a directory
    printf("%d %zd %ld\n",err,result.size(),size);

    err = FileUtil::readFile("/proc/self/cmdline", 1024, &result, &size);//命令行参数长度大小
    printf("%d %zd %ld\n", err, result.size(), size);


    err = FileUtil::readFile("/dev/null", 1024, &result, &size);
    printf("%d %zd %ld\n", err, result.size(), size);

    err = FileUtil::readFile("/dev/zero", 1024, &result, &size);
    printf("%d %zd %ld\n", err, result.size(), size);

    err = FileUtil::readFile("/notexist", 1024, &result, &size);//No such file or directory
    printf("%d %zd %ld\n", err, result.size(), size);

    err = FileUtil::readFile("/dev/zero", 102400, &result, &size);
    printf("%d %zd %ld\n", err, result.size(), size);

    err = FileUtil::readFile("/dev/zero", 102400, &result, NULL);
    printf("%d %zd %ld\n", err, result.size(), size);

    err = FileUtil::readFile("FileUtil_test.cpp", 102400, &result, &size);
    printf("%d %zd %ld\n--\n", err, result.size(), size);

    std::cout<<result<<endl;
}