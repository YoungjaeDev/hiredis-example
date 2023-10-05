cmake_minimum_required(VERSION 3.16)
project(HiredisProject)

set(CMAKE_BUILD_TYPE Debug)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
message("==> Using FetchContent")

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(nlohmann_json)

# Find the hiredis package using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(HIREDIS REQUIRED hiredis)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(LIBEVENT REQUIRED libevent)

# Include hiredis headers
include_directories(${HIREDIS_INCLUDE_DIRS}
                    ${OpenCV_INCLUDE_DIRS}
                    ${LIBEVENT_INCLUDE_DIRS})

# Add your main.cpp file
add_executable(MyProject main.cpp)

target_compile_options(MyProject PUBLIC ${HIREDIS_CFLAGS})

# Link against the hiredis library
target_link_libraries(MyProject ${HIREDIS_LIBRARIES} nlohmann_json::nlohmann_json Threads::Threads)

add_executable(redis-pub redis-pub-video.cpp)
target_compile_options(redis-pub PUBLIC ${HIREDIS_CFLAGS})
target_link_libraries(redis-pub ${HIREDIS_LIBRARIES} ${OpenCV_LIBS} nlohmann_json::nlohmann_json Threads::Threads)

add_executable(redis-sub redis-sub-video-async.cpp)
target_compile_options(redis-sub PUBLIC ${HIREDIS_CFLAGS})
target_link_libraries(redis-sub ${HIREDIS_LIBRARIES} ${OpenCV_LIBS} ${LIBEVENT_LIBRARIES} nlohmann_json::nlohmann_json Threads::Threads)

set(MEDIA_FILES
    sample.jpg
    eo.jpg
    race.mp4
    kids.mp4
    hannara.mp4
    ir_hannara.mp4
)

install(FILES ${MEDIA_FILES}
    DESTINATION ${CMAKE_BINARY_DIR}
)