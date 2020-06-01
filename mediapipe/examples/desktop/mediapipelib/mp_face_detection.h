#include <vector>
#include <opencv2/opencv.hpp>

#include "mp_export.h"

class MEDIAPIPE_LIB_EXPORT MPEnv{
public:
  static void SetResourceEnv(const std::string& env);
};
class MEDIAPIPE_LIB_EXPORT MPFaceDetection{
public:
  bool Init(const std::string& calculator_graph_config_file);
  std::vector<cv::Rect> Detect(cv::Mat img);
  void Exit();
private:
  void* impl;
};
