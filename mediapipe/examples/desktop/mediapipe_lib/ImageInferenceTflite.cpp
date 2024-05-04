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

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/optional_debug_tools.h"
#include "opencv2/opencv.hpp"
namespace{
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
  //openbl}
  //as_set_num_threads(8);
  input_layer = "Placeholder";
  output_layer = "logits/BiasAdd";
}

ImageInferenceTflite::~ImageInferenceTflite(void)
{
  if(m_Session)
    delete (tflite::FlatBufferModel*)m_Session;
}

bool run_predict_impl(const cv::Mat& src, std::unique_ptr<tflite::FlatBufferModel>& session, std::vector<float>& pvec,
  const std::string& input_layer, const std::string& output_layer);
//bool load_graph_to_ptr(const string& network_path, tensorflow::Session*& session);
void flip_points(std::vector<cv::Point>& pvec, int region, std::vector<cv::Point>& pvec2);


bool run_predict_impl(const cv::Mat& src, std::unique_ptr<tflite::FlatBufferModel>& model, std::vector<float>& pvec,
  const std::string& input_layer, const std::string& output_layer) {
  //int roisize = 100;
  cv::Mat roi = src;
  //cv::resize(src, roi, cv::Size(roisize, roisize));
  if(src.depth() != CV_32F)
    src.convertTo(roi, CV_32F);

  const int wanted_width = roi.cols;
  const int wanted_height = roi.rows;
  const int wanted_channels = 3;
  // tensorflow::Tensor image_tensor(
  //   tensorflow::DT_FLOAT,
  //   tensorflow::TensorShape({
  //     1, wanted_height, wanted_width, wanted_channels }));
  // float* ptensor = image_tensor.flat<float>().data();
  const float* pmat = roi.ptr<float>();
  
  //string result;
  
  // Build the interpreter with the InterpreterBuilder.
  // Note: all Interpreters should be built with the InterpreterBuilder,
  // which allocates memory for the Interpreter and does various set up
  // tasks so that the Interpreter can read the provided model.
  tflite::ops::builtin::BuiltinOpResolver resolver;
  tflite::InterpreterBuilder builder(*model, resolver);
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
  // Fill input buffers
  // TODO(user): Insert code to fill input tensors.
  // Note: The buffer of the input tensor with index `i` of type T can
  // be accessed with `T* input = interpreter->typed_input_tensor<T>(i);`
  float* input = interpreter->typed_input_tensor<float>(0);
  memcpy(input, pmat, wanted_width * wanted_height * wanted_channels * sizeof(float));

  // Run inference
  TFLITE_MINIMAL_CHECK(interpreter->Invoke() == kTfLiteOk);
  printf("\n\n=== Post-invoke Interpreter State ===\n");
  tflite::PrintInterpreterState(interpreter.get());

  // Read output buffers
  // TODO(user): Insert getting data out code.
  // Note: The buffer of the output tensor with index `i` of type T can
  // be accessed with `T* output = interpreter->typed_output_tensor<T>(i);`
  int outputId = interpreter->outputs()[0];
  TfLiteIntArray* output_dims = interpreter->tensor(outputId)->dims;
  float* output = interpreter->typed_output_tensor<float>(0);
  auto output_size = output_dims->data[output_dims->size - 1];

  // std::vector<tensorflow::Tensor> outputs;
  // tensorflow::Status run_status = session->Run({ { input_layer, image_tensor } },
  //   { output_layer }, {}, &outputs);
  // if (!run_status.ok()) {
  //   LOG(ERROR) << "Running model failed: " << run_status;
  //   tensorflow::LogAllRegisteredKernels();
  //   result = "Error running model";
  //   return false;// result;
  // }
  // tensorflow::string status_string = run_status.ToString();

  // tensorflow::Tensor* output = &outputs[0];
  // float* pvalue = output->flat<float>().data();
  //LOG(INFO) << " size : " << output->flat<float>().size();
  
  //auto shape = output->shape();
  int outputsize = 1;
  for (int i = 0; i < output_dims->size; ++i)
  {
    //LOG(INFO) << " dim : " << i << " : " << shape.dim_size(i);
    outputsize *= output_dims->data[i];
  }
  pvec.resize(outputsize);
  /*for (int i = 0; i < outputsize; ++i)
    pvec[i] = pvalue[i];*/
  std::copy(output, output + outputsize, &pvec[0]);
  /*for (int i = 0; i < outputsize / 2; ++i)
  {
    pvec[2 * i] *= src.cols;
    pvec[2 * i + 1] *= src.rows;
  }*/
  return true;
}


std::vector<float> ImageInferenceTflite::Inference(const cv::Mat& src, bool use_scale, float scale)
{
  std::vector<float> value;
  std::unique_ptr<tflite::FlatBufferModel> ptr((tflite::FlatBufferModel*)m_Session);
  run_predict_impl(src, ptr, value, input_layer, output_layer);
  m_Session = ptr.release();
  return value;
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