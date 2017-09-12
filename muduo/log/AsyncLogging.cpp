
#include <muduo/log/AsyncLogging.h>
#include <stdio.h>
#include <muduo/log/LogFile.h>
#include <muduo/base/ChronoTimestamp.h>
#include <string>
#include <mutex>
#include <chrono>
using namespace muduo;
using namespace std;

AsyncLogging::AsyncLogging(const std::string &basename, size_t rollSize, int flushInterval)
        : flushInterval_(flushInterval),
          running_(false),
          basename_(basename),
          rollSize_(rollSize),
          currentBuffer_(new Buffer),
          nextBuffer_(new Buffer)
{

    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}

/*************************************************
 发送方(前端)将日志追加到(后端)中;使用多个缓冲区避免磁盘IO阻塞．前端生成一个日志时,调用appen(),
 如果当前缓冲剩余空间足够大,则直接写入．不唤醒后端线程
 否则先将已满缓冲移入后端,再交换当前缓冲与预备缓冲．并唤醒后端线程写入磁盘．

 当前端写入太快,导致2块缓冲已经用完时,则分配一块新的缓冲作为当前缓冲．
*************************************************/
void AsyncLogging::append(const char *logline, int len)
{
    std::lock_guard<std::mutex> g(mutex_);
    if (currentBuffer_->avail() > len)
        currentBuffer_->append(logline, len);
    else
    {
        buffers_.push_back(std::move(currentBuffer_));
        if (nextBuffer_)
            currentBuffer_ = std::move(nextBuffer_);
        else//when it happen ?
            currentBuffer_.reset(new Buffer);//Rarely happens

    currentBuffer_->append(logline, len);
    cv_.notify_one();
    }
}

/*************************************************
 1.前端负责往A填充日志消息,后端负责将B的消息写入磁盘文件.A写满之后,交换AB.
 2.后端将前述A的数据写入磁盘,前端往B填充日志消息．
 3.往复循环
 用２个buffer的好处是在添加日志的时候不必等待磁盘io,而是直接往内存缓冲区写入,同时避免每条日志触发唤醒后端(写入磁盘IO).
 前端不是将一条条日志分别发送给后端,而是多条日志整合成一个大的buffer传送给后端,减少线程唤醒的次数与开销.
 另,为避免长时间未写入磁盘,即使bufferA未满,也每3秒执行一次交换(swap A B),使得能及时写入磁盘.

*************************************************/


/*************************************************
threadFunc()内含后端接收方．
具体实现:分配２块缓冲区,目的是在临界区内与前端缓冲交换指针(swap),高效传递日志．
临界区内等待条件触发：1,超时,2,前端写满一个或者多个缓冲．

当条件满足时,将当前缓冲移入后端,并立刻将空闲缓冲移为当前缓冲,临界区后面的代码可以安全的访问后端：将日志数据写入磁盘.
临界区内最后用newBuffer2替换newBuffer2目的在于前端始终有一个预备缓冲可调用．

 运行时分析：
 1.前端写入速度低,３s超时后才将后端唤醒，此时执行swap操作.然后写入磁盘．
 2.前端写入速度适中,超时前已经写满当前缓冲,于是唤醒后端写入磁盘
 3.前端短时间写入较快,超时前append已经写满２个缓冲,于是append重新分配一个缓冲,
 4.前端写入非常快,log可能无法正常写入磁盘，于是丢弃消息．
*************************************************/
void AsyncLogging::threadFunc()
{
    assert(running_ == true);
    LogFile output(basename_, rollSize_, false);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector bufferToWrite;//B
    bufferToWrite.reserve(16);
    while (running_)
    {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(bufferToWrite.empty());

        {
            std::unique_lock<std::mutex> g(mutex_);
            std::chrono::seconds time_it(flushInterval_);
            if (buffers_.empty())
                cv_.wait_for(g,time_it);

            buffers_.push_back(std::move(currentBuffer_));//input to A
            currentBuffer_ = std::move(newBuffer1);
            bufferToWrite.swap(buffers_);//AB交换
            if (!nextBuffer_)
                nextBuffer_ = std::move(newBuffer2);
        }

        assert(!bufferToWrite.empty());

        if (bufferToWrite.size() > 25)//目的：解决消息堆积问题．生产者速度高于消费者,前端存在过多消息则需要丢弃,否则可能导致内存暴涨
        {
            char buf[256];
            snprintf(buf, sizeof(buf), "Drop log messages at %s,%zd larger buffers\n",
                     ChronoTimestamp::now().toFormattedString().c_str(), bufferToWrite.size() - 2);
            fputs(buf, stderr);
            output.append(buf, static_cast<int>(strlen(buf)));
            bufferToWrite.erase(bufferToWrite.begin() + 2, bufferToWrite.end());
        }
        for (int i = 0; i < bufferToWrite.size(); ++i)//写入磁盘io
        {
            output.append(bufferToWrite[i]->data(), bufferToWrite[i]->length());
        }
        if (bufferToWrite.size() > 2)
            bufferToWrite.resize(2);

        if (!newBuffer1)
        {
            assert(!bufferToWrite.empty());
            newBuffer1 = std::move(bufferToWrite.back());
            bufferToWrite.pop_back();//why this?
            newBuffer1->reset();
        }

        if (!newBuffer2)
        {
            assert(!bufferToWrite.empty());
            newBuffer2 = std::move(bufferToWrite.back());
            bufferToWrite.pop_back();
            newBuffer2.reset();
        }

        bufferToWrite.clear();
        output.flush();
    }
    output.flush();
}