#include <opencv2/opencv.hpp>
#include <gflags/gflags.h>
#include "mp_face_detection.h"
#include <glog/logging.h>

std::string basename(const std::string& pathname)
{
    return {std::find_if(pathname.rbegin(), pathname.rend(),
                         [](char c) { return c == '/' || c=='\\'; }).base(),
            pathname.end()};
}

constexpr char kWindowName[] = "MediaPipe";

DEFINE_string(
    calculator_graph_config_file, "",
    "Name of file containing text format CalculatorGraphConfig proto.");
DEFINE_string(input_video_path, "",
              "Full path of video to load. "
              "If not provided, attempt to use a webcam.");
DEFINE_string(output_video_path, "",
              "Full path of where to save result (.mp4 only). "
              "If not provided, show result in a window.");
DEFINE_int32(rotate, -1,
              "Rotate of image");
DEFINE_string(input_image_folder, "",
              "Full path of image folder to load. "
              "If not provided, attempt to use a webcam.");
DEFINE_string(output_image_folder, "",
              "Full path of where to save image folder result. "
              "If not provided, show result in a window.");

int main(int argc, char** argv){
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  LOG(INFO) << "Initialize the camera or load the video.";
  cv::VideoCapture capture;
  const bool load_video = !FLAGS_input_video_path.empty() && FLAGS_input_image_folder.empty();
  const bool load_image_folder = !FLAGS_input_image_folder.empty();
  std::vector<cv::String> files;
  if (load_video) {
    capture.open(FLAGS_input_video_path);
  } else if(load_image_folder){
    cv::glob(FLAGS_input_image_folder, files);
  } else{
    capture.open(0);
  }
  if(!load_image_folder)
    if(!capture.isOpened())
      return -1;

  cv::VideoWriter writer;
  const bool save_video = !FLAGS_output_video_path.empty() && FLAGS_output_image_folder.empty();
  const bool save_image = !FLAGS_output_image_folder.empty() && FLAGS_output_video_path.empty();
  if (!save_video && !save_image) {
    cv::namedWindow(kWindowName, /*flags=WINDOW_AUTOSIZE*/ 1);
#if (CV_MAJOR_VERSION >= 3) && (CV_MINOR_VERSION >= 2)
    capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    capture.set(cv::CAP_PROP_FPS, 30);
#endif
  }

  MPFaceDetection fd;
  fd.Init(FLAGS_calculator_graph_config_file);

  LOG(INFO) << "Start grabbing and processing frames.";
  bool grab_frames = true;
  auto file_ptr = files.begin();
  while (grab_frames) {
    // Capture opencv camera or video frame.
    std::string fname = "";
    if(load_image_folder){
      if(file_ptr == files.end())
        break;
      fname = *file_ptr;
      file_ptr++;
      LOG(INFO) << "Process " << fname;
    }

    cv::Mat camera_frame_raw;
    if(load_image_folder){
      camera_frame_raw = cv::imread(fname);
    }else{
      capture >> camera_frame_raw;
    }
    if (camera_frame_raw.empty()) break;  // End of video.

    // PROCESS HERE
    auto& rect_vec = fd.Detect(camera_frame_raw);
    for(auto& r : rect_vec){
      cv::rectangle(camera_frame_raw, r, cv::Scalar(0,0,255), 3);
    }

    // Convert back to opencv for display or saving.
    cv::Mat output_frame_mat = camera_frame_raw;//mediapipe::formats::MatView(&output_frame);
    //cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2BGR);
    if (save_video) {
      if (!writer.isOpened()) {
        LOG(INFO) << "Prepare video writer.";
        writer.open(FLAGS_output_video_path,
                    cv::VideoWriter::fourcc('a', 'v', 'c', '1'),  // .mp4
                    capture.get(cv::CAP_PROP_FPS), output_frame_mat.size());
        if(!writer.isOpened())
          return 1;
      }
      writer.write(output_frame_mat);
    } else if(save_image){
      LOG(INFO) << "Save image to ." << FLAGS_output_image_folder;
      auto bname = basename(fname);
      cv::imwrite(FLAGS_output_image_folder + "//" + bname + ".jpg", output_frame_mat);
    } else {
      LOG(INFO) << "Show image .";
      cv::imshow(kWindowName, output_frame_mat);
      // Press any key to exit.
      const int pressed_key = cv::waitKey(5);
      if (pressed_key >= 0 && pressed_key != 255) grab_frames = false;
    }
  }

  LOG(INFO) << "Shutting down.";
  fd.Exit();
  return 0;
}