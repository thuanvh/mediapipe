#pragma once
#include <opencv2/opencv.hpp>
#include <list>

#ifdef _MEDIAPIPE_LIB_EXPORT_
#define _MEDIAPIPE_LIB_API_   __declspec( dllexport ) 
#else
#define _MEDIAPIPE_LIB_API_   
#endif