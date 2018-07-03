cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)

set(OpenCV_DIR ~/opencv-2.4.13.4/build)
find_package(OpenCV REQUIRED)


file(GLOB_RECURSE SOURCE_FILES
        ConstW.h
        Watermark.cpp Watermark.h
        ImageInsert.cpp DetectBlind.cpp
        "RS/*"
        "QR/*"
        )
add_library(Watermark SHARED ${SOURCE_FILES})
install(TARGETS Watermark LIBRARY DESTINATION lib)
install(FILES Watermark.h DESTINATION include)

target_link_libraries(Watermark ${OpenCV_LIBS})
set(CMAKE_INSTALL_PREFIX /home/wxrui/clionPRO/testSO/build_ins)



