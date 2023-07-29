cmake_minimum_required(VERSION 3.24)
project(cartographer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(cartographer src/main.cpp)

# Find OpenCV & Tesseract
find_package(OpenCV REQUIRED)
target_link_libraries(cartographer PUBLIC ${OpenCV_LIBS})
