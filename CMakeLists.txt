cmake_minimum_required(VERSION 3.22)
project(dev_cxx)

set(CMAKE_CXX_STANDARD 17)
set(Boost_NO_WARN_NEW_VERSIONS 1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
enable_testing()

add_executable(dev_cxx main.cpp MO_Learning.cpp MO_Learning.h Xoshiro256.h )
add_executable(tester Testing.cpp MO_Learning.cpp MO_Learning.h Xoshiro256.h)

set(BOOST_ROOT /usr/local/boost)
# set(BOOST_TEST_LOG_SINK stderr)
find_package(Boost COMPONENTS system filesystem unit_test_framework program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
target_compile_definitions(tester PRIVATE "BOOST_TEST_DYN_LINK=1")
target_link_libraries(tester ${Boost_LIBRARIES} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(dev_cxx ${Boost_LIBRARIES})

add_executable(mastermind main.cpp MO_Learning.cpp MO_Learning.h Xoshiro256.h)
target_link_libraries(mastermind ${Boost_LIBRARIES})
target_compile_definitions(mastermind PRIVATE "BUILD_TESTING=OFF")

find_package(fmt)
target_link_libraries(mastermind fmt::fmt)
target_link_libraries(dev_cxx fmt::fmt)
target_link_libraries(tester fmt::fmt)



