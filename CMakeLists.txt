cmake_minimum_required(VERSION 3.10)
project( Video-Chunk-Compression)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find OpenCV
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( Video-Scene-Change scene_change.cpp )
add_executable( Video-Chunking video_chunk.cpp )
add_executable( video_quality_metrics video_metrics.cpp )
add_executable( video_sizes video_sizes.cpp)
add_executable(  video_frames video_frames.cpp)
add_executable( compare_video compare_video.cpp)
# add_executable( plot plot.cpp)

target_link_libraries( Video-Scene-Change ${OpenCV_LIBS} )
target_link_libraries( Video-Chunking ${OpenCV_LIBS} )
target_link_libraries( video_quality_metrics ${OpenCV_LIBS} )
target_link_libraries( video_frames ${OpenCV_LIBS} )
target_link_libraries( compare_video ${OpenCV_LIBS})
# target_link_libraries( plot ${OpenCV_LIBS})

