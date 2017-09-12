set(log_SRE
        AsyncLogging.cpp
        LogFile.cpp
        Logging.cpp
        LogStream.cpp
        FileUtil.cpp
        ProcessInfo.cpp
)
add_library(muduo_log ${log_SRE})
target_link_libraries(muduo_log muduo_base )

install(TARGETS muduo_log DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/muduo/log)
if(NOT CMAKE_BUILD_NO_EXAMPLES)
    add_subdirectory(tests)
endif()