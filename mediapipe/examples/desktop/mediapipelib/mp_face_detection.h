#include <vector>
#include <opencv2/opencv.hpp>
#define _MEDIAPIPE_LIB_EXPORT_
#include "mp_export.h"


class MEDIAPIPE_LIB_EXPORT MPFaceDetection{
public:
  bool Init(std::string calculator_graph_config_file);
  std::vector<cv::Rect> Detect(cv::Mat img);
  void Exit();
private:
  void* impl;
};
