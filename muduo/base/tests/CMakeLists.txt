add_executable(Atomic_unittest Atomic_unittest.cpp)
target_link_libraries(Atomic_unittest muduo_base)

#add_executable(ChronoTimestamp_test ChronoTimestamp_test.cpp  ChronoTimestamp.cpp)
#add_test(NAME Atomic_unittest COMMAND Atomic_unittest)

add_executable(ChronoTimestamp_test ChronoTimestamp_test.cpp  )
target_link_libraries(ChronoTimestamp_test  muduo_base)

add_executable(ChronoTimestamp_test_benchmark ChronoTimestamp_test_benchmark.cpp )
target_link_libraries(ChronoTimestamp_test_benchmark  muduo_base)

add_executable(Singleton_test Singleton_test.cpp)
target_link_libraries(Singleton_test muduo_base)

