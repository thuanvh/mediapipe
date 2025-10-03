#include "mediapipe/framework/calculator_graph.h"
#include "mediapipe/framework/port/logging.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
// #include "mediapipe/framework/port/opencv_imgcodecs.h"
// #include "mediapipe/framework/port/opencv_video_inc.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "absl/types/optional.h"

// OpenCV includes for I/O
#include <opencv2/opencv.hpp>
#include <vector>

namespace mdpplib {

// Define the expected output type for clarity
using FaceLandmarks = std::vector<::mediapipe::NormalizedLandmarkList>;

class VideoFaceLandmarker {
public:
    // Constructor/Destructor
    VideoFaceLandmarker() = default;
    ~VideoFaceLandmarker() { 
        if (graph_initialized_) {
            Dispose();
        }
    }

    /**
     * @brief Initializes the MediaPipe CalculatorGraph.
     * @param graph_config_path Path to the .pbtxt graph file.
     * @return absl::Status::Ok() on success.
     */
    absl::Status Initialize(const std::string& graph_config_path) ;

    /**
     * @brief Processes a single video frame and returns face landmarks.
     * @param input_frame The input frame (OpenCV Mat, expected to be BGR).
     * @param timestamp_us Current frame timestamp in microseconds.
     * @return A vector of NormalizedLandmarkList, one for each detected face.
     */
    absl::StatusOr<FaceLandmarks> Run(const cv::Mat& input_frame, int64_t timestamp_us);

    /**
     * @brief Closes all packet sources and waits for the graph to finish.
     * @return absl::Status::Ok() on success.
     */
    absl::Status Dispose() ;

private:
    // Constant stream names from the typical Face Mesh desktop graph
    static constexpr char kInputStream[] = "input_video";
    static constexpr char kLandmarksStream[] = "multi_face_landmarks";

    mediapipe::CalculatorGraph graph_;
    absl::optional<mediapipe::OutputStreamPoller> poller_;
    bool graph_initialized_ = false;
};

} // namespace mediapipe