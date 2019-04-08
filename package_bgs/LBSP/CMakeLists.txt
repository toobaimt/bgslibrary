cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 11)
if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE "Release")
endif()
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse3")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
project( bgfg_segm )
find_package( OpenCV 3.4.5 REQUIRED )
set(SOURCE_FILES
	BackgroundSubtractorLBSP.cpp
	BackgroundSubtractorLBSP.h
	BackgroundSubtractorLBSP_.cpp
	BackgroundSubtractorLBSP_.h
	BackgroundSubtractorPAWCS.cpp
	BackgroundSubtractorPAWCS.h
	DistanceUtils.h
	LBSP.cpp
	LBSP.h
	LBSP_.cpp
	LBSP_.h
	RandUtils.h)
add_executable(bgfg_segm ${SOURCE_FILES} bgfg_segm.cpp)
target_link_libraries( bgfg_segm ${OpenCV_LIBS})
