#include <iostream>
#include "ImageInferenceTflite.h"

int main(int argc, char** argv) {
  std::cout << "Load model" << std::endl;
  ImageInferenceTflite tf1;
  tf1.LoadNet("model.tflite", "input/x-input", "softmax_tensor");
  std::cout << "Load end load model" << std::endl;
  tf1.DumpModel();
  return 0;
}