set(poll_src
        DefaultPoller.cpp
        PollPoller.cpp
        )
add_library(muduo_poller ${poll_src})
target_link_libraries(muduo_poller muduo_net)
