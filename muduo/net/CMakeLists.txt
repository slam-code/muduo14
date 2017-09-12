set(net_SRE
        Acceptor.cpp
        Channel.cpp
        Callbacks.cpp
        EventLoop.cpp
        InetAddress.cpp
        Poller.cpp
        SocketsOps.cc
        )
add_subdirectory(poller)
add_library(muduo_net ${net_SRE})
target_link_libraries(muduo_net muduo_log muduo_base muduo_poller muduo_timer)

install(TARGETS muduo_net DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/muduo/net)
if (NOT CMAKE_BUILD_NO_EXAMPLES)
    add_subdirectory(tests)
endif ()