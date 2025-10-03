#pragma once
#include <vector>
#include "MdppLibExport.h"

class _MDPP_LIB_API_ ImageInferenceTflite
{
public:
  ImageInferenceTflite();
  ~ImageInferenceTflite(void);
  void LoadNet(const std::string& netconfig,const std::string& input_layer = "Placeholder", const std::string& output_layer = "logits/BiasAdd");
  /*
   * 0 : CPU, 1 : GPU
   */
  void SetMode(int mode);
  void SetDevice(int device);
  //std::vector<float> Regression(const cv::Mat& src, const std::vector<float>& values);
  std::vector<float> Inference(const cv::Mat& src, bool use_scale = true, float scale = 1 / 255.0f);
  int Predict(const cv::Mat& src, bool use_scale, float scale, std::vector<float>& prob);
  void DumpModel();
  /*static void Train(const std::string& netconfig, const std::string& netresult = NULL, bool finetune_net = false);
  void InitNet(const std::string& input);
  void LoadTrainedLayer(const std::string& trainedfile);*/
private:
  void* m_Session;
  std::string input_layer;
  std::string output_layer;
};

