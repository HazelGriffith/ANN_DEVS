cmake_minimum_required(VERSION 3.16)

project(ANN_DEVS)

#find_package(Python REQUIRED Development)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_BUILD_TYPE Debug)


# Add required libraries and include directories
#add_library(data_structures data_structures/CompsInfo.cpp)
target_include_directories(data_structures PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/data_structures  
)

add_executable(main.out main.cpp)

target_include_directories(main.out PRIVATE "." "include" $ENV{CADMIUM})

# Non-ESP32 specific compile options
target_compile_options(main.out PUBLIC -std=gnu++2b)

target_link_libraries(main.out data_structures)
#target_link_libraries(main.out data_structures Python::Python)
