cmake_minimum_required(VERSION 3.15)
project(placeDBow2)
find_package(OpenCV REQUIRED)
find_package(DBoW2 REQUIRED)

set(OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/dll)
include_directories(${OpenCV_INCLUDE_DIRS} ${DBoW2_INCLUDE_DIRS})
add_library(hj_placeDBow2 SHARED src/main.cpp src/loopdetector.cpp )
target_include_directories(hj_placeDBow2 PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(hj_placeDBow2 ${OpenCV_LIBS} ${DBoW2_LIBS})
set_target_properties(hj_placeDBow2 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})