cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(cmake_and_cuda LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS " -static-libgcc -static-libstdc++ -L. ")

#find_package(CUDA REQUIRED)
#list(APPEND CUDA_NVCC_FLAGS "-std=c++11 -g -G" )
#list(APPEND CUDA_NVCC_FLAGS "-std=c++11")
find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
   set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

include (${CMAKE_CURRENT_SOURCE_DIR}/opt/boost.cmake)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/opt/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/opt)
link_directories( ${CMAKE_CURRENT_LIST_DIR}/opt/lib )
set( BOOST_LIBS ${BOOST_LIBS} -Wl,-Bstatic boost_filesystem boost_system boost_program_options -Wl,-Bdynamic )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../StringAnylizerGUI)

add_executable(main main.cpp QBCParser.cpp ${CMAKE_CURRENT_LIST_DIR}/../StringAnylizerGUI/kernel.cpp)
add_dependencies(main boost)
target_link_libraries(main ${BOOST_LIBS})
