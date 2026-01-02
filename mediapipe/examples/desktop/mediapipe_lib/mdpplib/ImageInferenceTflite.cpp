#include "ImageInferenceTflite.h"
// #include "VirtualMakeover/utility/StringUtils.h"
#include <fstream>
#include <vector>
//#include <eigen/Dense>

#ifdef _MSC_VER
#define COMPILER_MSVC
#define NOMINMAX
#endif


#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/optional_debug_tools.h"

namespace {
class Log {
  std::stringstream stream_;

 public:
  explicit Log(const char* severity) { stream_ << severity << ": "; }
  std::stringstream& Stream() { return stream_; }
  ~Log() { std::cerr << stream_.str() << std::endl; }
};

}

#define LOG(severity) Log(#severity).Stream()

#define TFLITE_MINIMAL_CHECK(x)                              \
  if (!(x)) {                                                \
    fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
    exit(1);                                                 \
  }

ImageInferenceTflite::ImageInferenceTflite():m_Session(NULL)
{
  input_layer = "Placeholder";
  output_layer = "logits/BiasAdd";
}

ImageInferenceTflite::~ImageInferenceTflite(void)
{
  if(m_Session)
    delete (tflite::FlatBufferModel*)m_Session;
}

bool run_predict_impl(const uint8_t* pixel_data, int width, int height, std::unique_ptr<tflite::FlatBufferModel>& model, std::vector<float>& pvec,
  const std::string& input_layer, const std::string& output_layer, bool use_scale, float scale) {
  
  const int wanted_width = width;
  const int wanted_height = height;
  const int wanted_channels = 3;
  
  tflite::ops::builtin::BuiltinOpResolver resolver;
  tflite::InterpreterBuilder builder(*model, resolver);
  std::unique_ptr<tflite::Interpreter> interpreter;
  builder(&interpreter);
  TFLITE_MINIMAL_CHECK(interpreter != nullptr);

  // Allocate tensor buffers.
  TFLITE_MINIMAL_CHECK(interpreter->AllocateTensors() == kTfLiteOk);

  // Fill input buffers
  float* input = interpreter->typed_input_tensor<float>(0);
  int num_pixels = wanted_width * wanted_height * wanted_channels;
  for (int i = 0; i < num_pixels; ++i) {
      if (use_scale) {
          input[i] = (float)pixel_data[i] * scale;
      } else {
          input[i] = (float)pixel_data[i];
      }
  }

  // Run inference
  TFLITE_MINIMAL_CHECK(interpreter->Invoke() == kTfLiteOk);

  // Read output buffers
  int outputId = interpreter->outputs()[0];
  TfLiteIntArray* output_dims = interpreter->tensor(outputId)->dims;
  float* output = interpreter->typed_output_tensor<float>(0);
  
  int outputsize = 1;
  for (int i = 0; i < output_dims->size; ++i)
  {
    outputsize *= output_dims->data[i];
  }
  pvec.resize(outputsize);
  std::copy(output, output + outputsize, pvec.begin());
  
  return true;
}


std::vector<float> ImageInferenceTflite::Inference(const uint8_t* pixel_data, int width, int height, bool use_scale, float scale)
{
  std::vector<float> value;
  std::unique_ptr<tflite::FlatBufferModel> ptr((tflite::FlatBufferModel*)m_Session);
  run_predict_impl(pixel_data, width, height, ptr, value, input_layer, output_layer, use_scale, scale);
  m_Session = ptr.release();
  return value;
}

int ImageInferenceTflite::Predict(const uint8_t* pixel_data, int width, int height, bool use_scale, float scale, std::vector<float>& prob)
{
  std::unique_ptr<tflite::FlatBufferModel> ptr((tflite::FlatBufferModel*)m_Session);
  bool success = run_predict_impl(pixel_data, width, height, ptr, prob, input_layer, output_layer, use_scale, scale);
  m_Session = ptr.release();
  return success ? 0 : -1;
}
void ImageInferenceTflite::LoadNet( const std::string& filename, const std::string& input_layer, const std::string& output_layer)
{
  //load_graph_to_ptr(netconfig, (Session*&)m_Session);
  // Load model
  std::unique_ptr<tflite::FlatBufferModel> model =
      tflite::FlatBufferModel::BuildFromFile(filename.c_str());
  TFLITE_MINIMAL_CHECK(model != nullptr);

  m_Session = model.release();

  this->input_layer = input_layer;
  this->output_layer = output_layer;
}

void ImageInferenceTflite::DumpModel(){
// Build the interpreter with the InterpreterBuilder.
  // Note: all Interpreters should be built with the InterpreterBuilder,
  // which allocates memory for the Interpreter and does various set up
  // tasks so that the Interpreter can read the provided model.
  tflite::ops::builtin::BuiltinOpResolver resolver;
  tflite::InterpreterBuilder builder(*(tflite::FlatBufferModel*)m_Session, resolver);
  std::unique_ptr<tflite::Interpreter> interpreter;
  builder(&interpreter);
  TFLITE_MINIMAL_CHECK(interpreter != nullptr);

  // Allocate tensor buffers.
  TFLITE_MINIMAL_CHECK(interpreter->AllocateTensors() == kTfLiteOk);
  printf("=== Pre-invoke Interpreter State ===\n");
  tflite::PrintInterpreterState(interpreter.get());
  //if (settings->verbose) 
  {
    LOG(INFO) << "tensors size: " << interpreter->tensors_size();
    LOG(INFO) << "nodes size: " << interpreter->nodes_size();
    LOG(INFO) << "inputs: " << interpreter->inputs().size();
    LOG(INFO) << "input(0) name: " << interpreter->GetInputName(0);

    int t_size = interpreter->tensors_size();
    for (int i = 0; i < t_size; i++) {
      if (interpreter->tensor(i)->name)
        LOG(INFO) << i << ": " << interpreter->tensor(i)->name << ", "
                  << interpreter->tensor(i)->bytes << ", "
                  << interpreter->tensor(i)->type << ", "
                  << interpreter->tensor(i)->params.scale << ", "
                  << interpreter->tensor(i)->params.zero_point;
    }
  }
}