

add_executable(EventLoop_test_simple0 EventLoop_test_simple0.cpp)
target_link_libraries(EventLoop_test_simple0 muduo_net  )


add_executable(EventLoop_test_simple1 EventLoop_test_simple1.cpp)
target_link_libraries( EventLoop_test_simple1 muduo_net)

add_executable(EventLoop_test_simple2 EventLoop_test_simple2.cpp)
target_link_libraries( EventLoop_test_simple2 muduo_net)


add_executable(GzipFile_test GzipFile_test.cpp )
target_link_libraries(GzipFile_test  z muduo_net)
