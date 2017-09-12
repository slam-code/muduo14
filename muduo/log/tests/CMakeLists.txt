
add_executable(LogStream_test LogStream_test.cpp)
target_link_libraries(LogStream_test muduo_base gtest muduo_log)


add_executable(LogStream_bench LogStream_bench.cpp)
target_link_libraries(LogStream_bench muduo_base muduo_log)


add_executable(BlockingQueue__test BlockingQueue_test.cpp)
target_link_libraries(BlockingQueue__test muduo_base muduo_log)

add_executable(BoundedBlockingQueue_test BoundedBlockingQueue_test.cpp)
target_link_libraries(BoundedBlockingQueue_test muduo_base muduo_log)


add_executable(BlockingQueue__bench BlockingQueue_bench.cpp)
target_link_libraries(BlockingQueue__bench muduo_base muduo_log)


add_executable(FileUtil_test FileUtil_test.cpp)
target_link_libraries(FileUtil_test muduo_log)


add_executable(LogFile_test LogFile_test.cpp)
target_link_libraries(LogFile_test muduo_log)

add_executable(ProcessInfo_test ProcessInfo_test.cpp )
target_link_libraries(ProcessInfo_test muduo_base muduo_log)

add_executable(Logging_test Logging_test.cpp )
target_link_libraries(Logging_test muduo_base muduo_log)





add_executable(AsyncLogging_test AsyncLogging_test.cpp  )
target_link_libraries(AsyncLogging_test   muduo_base muduo_log )


