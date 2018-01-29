cmake_minimum_required(VERSION 3.5)
project(Watermark)

set(CMAKE_CXX_STANDARD 11)
set(OpenCV_DIR ~/opencv-2.4.13.4/build)
find_package(OpenCV REQUIRED)


set(SOURCE_FILES
        main.cpp RS/berlekamp.cpp RS/crcgen.cpp RS/ecc.h RS/galois.cpp RS/rs.cpp
        ConstW.h
        Watermark.cpp Watermark.h
        ImageInsert.cpp DetectBlind.cpp)
add_executable(Watermark ${SOURCE_FILES})
target_link_libraries(Watermark ${OpenCV_LIBS})