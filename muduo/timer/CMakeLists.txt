set(timer_SRE

        Timer.cpp
        TimerQueue.cpp
        )

add_library(muduo_timer ${timer_SRE})
target_link_libraries(muduo_timer muduo_net)

install(TARGETS muduo_timer DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/muduo/timer)
if (NOT CMAKE_BUILD_NO_EXAMPLES)
    add_subdirectory(tests)
endif ()