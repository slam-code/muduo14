set(base_SRE
        ChronoTimestamp.cpp

)
add_library(muduo_base ${base_SRE})
add_library(gtest gtest-all.cpp)
target_link_libraries(muduo_base pthread)

install(TARGETS muduo_base DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/muduo/base)
if(NOT CMAKE_BUILD_NO_EXAMPLES)
    add_subdirectory(tests)
endif()