// Copyright 2019 The MediaPipe Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// An example of sending OpenCV webcam frames into a MediaPipe graph.
#include <cstdlib>

#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/commandlineflags.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/formats/detection.pb.h"
#include "mediapipe/framework/formats/location_data.pb.h"
#include "mediapipe/util/resource_util.h"

#include "mp_face_detection.h"

namespace{
  constexpr char kInputStream[] = "input_video";
  //constexpr char kOutputStream[] = "output_video";
  constexpr char kOutputStream[] = "output_detections";
}

class MPFaceDetectionImpl
{
  private:
  
  mediapipe::CalculatorGraph graph;
  public:
  ~MPFaceDetectionImpl()=default;
  MPFaceDetectionImpl(const MPFaceDetectionImpl&) = default;
  MPFaceDetectionImpl& operator=(const MPFaceDetectionImpl&) = default;
  MPFaceDetectionImpl(){}
  MPFaceDetectionImpl(MPFaceDetectionImpl &&){}

  ::mediapipe::Status init(const std::string& calculator_graph_config_file);
  ::mediapipe::Status detect(cv::Mat camera_frame_raw, std::vector<cv::Rect>& detection_result);
  ::mediapipe::Status close();
  
  private:
  std::vector<cv::Rect> GetDetection(mediapipe::Packet& packet, int width, int height);
  cv::Rect GetLocation(const mediapipe::Detection& detection, int width, int height);
};

::mediapipe::Status MPFaceDetectionImpl::init(const std::string& calculator_graph_config_file)
{
  std::string calculator_graph_config_contents;
  MP_RETURN_IF_ERROR(mediapipe::file::GetContents(
      calculator_graph_config_file, &calculator_graph_config_contents));
  LOG(INFO) << "Get calculator graph config contents: "
            << calculator_graph_config_contents;
  mediapipe::CalculatorGraphConfig config =
      mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(
          calculator_graph_config_contents);

  LOG(INFO) << "Initialize the calculator graph.";
  
  MP_RETURN_IF_ERROR(graph.Initialize(config));
  return ::mediapipe::OkStatus();
}

::mediapipe::Status MPFaceDetectionImpl::detect(cv::Mat camera_frame_raw, std::vector<cv::Rect>& detection_result)
{
  LOG(INFO) << "Start running the calculator graph.";  
  ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller poller,
                   graph.AddOutputStreamPoller(kOutputStream));
  //graph.AddOutputStreamPoller(kOutputStream).Assign(poller);
  MP_RETURN_IF_ERROR(graph.StartRun({}));

  cv::Mat camera_frame;
  cv::cvtColor(camera_frame_raw, camera_frame, cv::COLOR_BGR2RGB);
  
  // Wrap Mat into an ImageFrame.
  LOG(INFO) << "Wrap Mat into an ImageFrame."; 
  auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
      mediapipe::ImageFormat::SRGB, camera_frame.cols, camera_frame.rows,
      mediapipe::ImageFrame::kDefaultAlignmentBoundary);
  cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
  camera_frame.copyTo(input_frame_mat);

  // Send image packet into the graph.
  LOG(INFO) << "Send image packet into the graph."; 
  size_t frame_timestamp_us =
      (double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;
  MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
      kInputStream, mediapipe::Adopt(input_frame.release())
                        .At(mediapipe::Timestamp(frame_timestamp_us))));

  // Get the graph result packet, or stop if that fails.
  LOG(INFO) << "Get the graph result packet, or stop if that fails."; 
  mediapipe::Packet packet;
  poller.Next(&packet);
  
  detection_result.clear();
  for(auto r : GetDetection(packet, camera_frame.cols, camera_frame.rows))
    detection_result.push_back(r);
  return ::mediapipe::OkStatus();
}

::mediapipe::Status MPFaceDetectionImpl::close(){
  LOG(INFO) << "Shutting down.";
  //if (writer.isOpened()) writer.release();
  MP_RETURN_IF_ERROR(graph.CloseInputStream(kInputStream));
  return graph.WaitUntilDone();
}

std::vector<cv::Rect> MPFaceDetectionImpl::GetDetection(mediapipe::Packet& packet, int width, int height)
{
  std::vector<cv::Rect> rect_list;
  for (const auto& detection : packet.Get<std::vector<mediapipe::Detection>>()){
    rect_list.push_back(GetLocation(detection, width, height));
  }
  LOG(INFO) << "Get detection size " << rect_list.size();
  if(rect_list.size() > 0)
    LOG(INFO) << rect_list[0];
  return rect_list;
}

cv::Rect MPFaceDetectionImpl::GetLocation(const mediapipe::Detection& detection, int width, int height){
  LOG(INFO) << "Get detection detection.location_data().format() " << detection.location_data().format();
  if (detection.location_data().format() == mediapipe::LocationData::BOUNDING_BOX) {
    return cv::Rect(detection.location_data().bounding_box().xmin() * width,
                    detection.location_data().bounding_box().ymin() * height,
                    detection.location_data().bounding_box().width() * width,
                    detection.location_data().bounding_box().height() * height);
  } else {
    return cv::Rect (
        detection.location_data().relative_bounding_box().xmin() * width,
        detection.location_data().relative_bounding_box().ymin() * height,
        detection.location_data().relative_bounding_box().width() * width,
        detection.location_data().relative_bounding_box().height() * height
        );
  }
  return cv::Rect(0,0,0,0);
}
bool MPFaceDetection::Init(const std::string& calculator_graph_config_file){
  impl = new MPFaceDetectionImpl();
  ((MPFaceDetectionImpl*)impl)->init(calculator_graph_config_file);
  return true;
}

std::vector<cv::Rect> MPFaceDetection::Detect(cv::Mat img){
  std::vector<cv::Rect> rectvec;
  ((MPFaceDetectionImpl*)impl)->detect(img, rectvec);  
  return rectvec;
}

void MPFaceDetection::Exit(){
  ((MPFaceDetectionImpl*)impl)->close();
  delete ((MPFaceDetectionImpl*)impl);
}


void MPEnv::SetResourceEnv(const std::string& env){
  mediapipe::SetResourceBasePath(env);
}
