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
#include "mediapipe/framework/formats/classification.pb.h"

// OpenCV includes for I/O
#include <opencv2/opencv.hpp>
#include <vector>
#include <fstream>
#include "ImageFaceBlendshapes.h"
//namespace mediapipe {

// // Define the expected output type for clarity
// using FaceLandmarks = std::vector<::mediapipe::NormalizedLandmarkList>;

// class FaceLandmarkerApp {
// public:
//     // Constructor/Destructor
//     FaceLandmarkerApp() = default;
//     ~FaceLandmarkerApp() { 
//         if (graph_initialized_) {
//             Dispose();
//         }
//     }

//     /**
//      * @brief Initializes the MediaPipe CalculatorGraph.
//      * @param graph_config_path Path to the .pbtxt graph file.
//      * @return absl::Status::Ok() on success.
//      */
//     absl::Status Initialize(const std::string& graph_config_path) {
//         LOG(INFO) << "Initializing MediaPipe graph...";

//         // 1. Read the Graph config file
//         std::string graph_config_string;
//         MP_RETURN_IF_ERROR(mediapipe::file::GetContents(graph_config_path, &graph_config_string));
//         CalculatorGraphConfig config = 
//             ::mediapipe::ParseTextProtoOrDie<CalculatorGraphConfig>(graph_config_string);

//         // 2. Initialize the graph
//         MP_RETURN_IF_ERROR(graph_.Initialize(config));
        
//         // 3. Setup the output poller
//         MP_ASSIGN_OR_RETURN(
//             poller_,
//             graph_.AddOutputStreamPoller(kLandmarksStream));

//         // 4. Start the graph
//         MP_RETURN_IF_ERROR(graph_.StartRun({}));
        
//         graph_initialized_ = true;
//         LOG(INFO) << "MediaPipe graph initialized and started.";
//         return absl::OkStatus();
//     }

//     /**
//      * @brief Processes a single video frame and returns face landmarks.
//      * @param input_frame The input frame (OpenCV Mat, expected to be BGR).
//      * @param timestamp_us Current frame timestamp in microseconds.
//      * @return A vector of NormalizedLandmarkList, one for each detected face.
//      */
//     absl::StatusOr<FaceLandmarks> Run(const cv::Mat& input_frame, int64_t timestamp_us) {
//         if (!graph_initialized_) {
//             return absl::InternalError("Graph is not initialized. Call Initialize() first.");
//         }

//         // 1. Convert OpenCV Mat to MediaPipe ImageFrame
//         // MediaPipe usually expects RGB, so convert BGR to RGB
//         cv::Mat rgb_frame;
//         cv::cvtColor(input_frame, rgb_frame, cv::COLOR_BGR2RGB);

//         auto mp_frame = absl::make_unique<ImageFrame>(
//             ImageFormat::SRGB, rgb_frame.cols, rgb_frame.rows, 
//             ImageFrame::kDefaultAlignmentBoundary);
        
//         cv::Mat wrapped_mat = formats::MatView(mp_frame.get());
//         rgb_frame.copyTo(wrapped_mat);

//         // 2. Send the packet to the graph
//         MP_RETURN_IF_ERROR(graph_.AddPacketToInputStream(
//             kInputStream, 
//             Adopt(mp_frame.release())
//                 .At(Timestamp(timestamp_us))));

//         // 3. Get the landmark results
//         Packet packet;
//         if (poller_ && poller_->Next(&packet)) {
//             // Success: return the face landmarks
//             return packet.Get<FaceLandmarks>();
//         } else {
//             // This happens if the graph stalls or is closed.
//             return FaceLandmarks{};
//         }
//     }

//     /**
//      * @brief Closes all packet sources and waits for the graph to finish.
//      * @return absl::Status::Ok() on success.
//      */
//     absl::Status Dispose() {
//         if (!graph_initialized_) return absl::OkStatus();

//         LOG(INFO) << "Stopping MediaPipe graph...";
//         absl::Status close_status = graph_.CloseAllPacketSources();
//         absl::Status wait_status = graph_.WaitUntilDone();
        
//         graph_initialized_ = false;
        
//         // Return the first error encountered, or OkStatus
//         if (!close_status.ok()) return close_status;
//         return wait_status;
//     }

// private:
//     // Constant stream names from the typical Face Mesh desktop graph
//     static constexpr char kInputStream[] = "input_video";
//     static constexpr char kLandmarksStream[] = "multi_face_landmarks";

//     CalculatorGraph graph_;
//     absl::optional<OutputStreamPoller> poller_;
//     bool graph_initialized_ = false;
// };

// } // namespace mediapipe
bool exists (const std::string& name)
    {
        std::ifstream f(name.c_str());
        return f.good();
    }
    bool isDirectory (const std::string& path) {
        struct stat info;
        if (stat(path.c_str(), &info) != 0) {
            return false; // Cannot access
        }
        return (info.st_mode & S_IFDIR) != 0;
    }
// --- Example Usage (main function) ---
int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);

    // **IMPORTANT**: In a real build, you must have this file available 
    // and the path must be correct relative to your Bazel workspace.
    const std::string kGraphPath = "mediapipe/graphs/face_mesh/face_mesh_desktop_image_blendshapes.pbtxt";
    
    mdpplib::ImageFaceBlendshapes landmarker;
    absl::Status init_status = landmarker.Initialize(kGraphPath);

    if (!init_status.ok()) {
        LOG(ERROR) << "Failed to initialize: " << init_status.message();
        return 1;
    }

    const std::string folder_path = argc > 1 ? argv[1] : ".";
    const std::string output_folder = argc > 2 ? argv[2] : "";
    std::vector<std::string> image_files;
    // if folder_path is a file, just process that file using std library
    
    if (exists(folder_path) && !isDirectory(folder_path)) {
        image_files.push_back(folder_path);
    }else{
        cv::glob(folder_path + "/*.jpg", image_files);
    }
    if (image_files.empty()) {
        LOG(ERROR) << "No .jpg images found in " << folder_path;
        return 1;
    }
    int frame_count = 0;
    for (const auto& image_path : image_files) {
        std::cout << frame_count << " Processing image: " << image_path << std::endl;
        cv::Mat frame = cv::imread(image_path);
        if (frame.empty()) {
            LOG(ERROR) << "Failed to read image: " << image_path;
            continue;
        }
        cv::Mat frame_resized;
        cv::resize(frame, frame_resized, cv::Size(600, 600 * frame.rows / frame.cols));

        //int64_t timestamp_us = 1;//frame_count * 1000 * 1000 / 30; // Mock 30 FPS timestamp
        int64_t timestamp_us = std::chrono::duration_cast<std::chrono::microseconds>(
             std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        cv::Mat rgb_frame;
        cv::cvtColor(frame_resized, rgb_frame, cv::COLOR_BGR2RGB);

        // Run detection
        absl::StatusOr<mdpplib::FaceLandmarks> result = landmarker.Run(rgb_frame.data, rgb_frame.cols, rgb_frame.rows, timestamp_us);

        if (result.ok()) {
            const auto& face_landmarks = result.value();
            
            // Draw the landmarks on the frame
            for (const auto& face : face_landmarks) {
                for (int i = 0; i < face.landmark_size(); ++i) {
                    const auto& landmark = face.landmark(i);
                    // Convert normalized coordinates [0.0, 1.0] to pixel coordinates
                    int x = static_cast<int>(landmark.x() * frame_resized.cols);
                    int y = static_cast<int>(landmark.y() * frame_resized.rows);
                    cv::circle(frame_resized, cv::Point(x, y), 1, cv::Scalar(0, 255, 0), -1);
                }
            }

            // Get blendshapes
            absl::StatusOr<mediapipe::ClassificationList> blendshapes_result = landmarker.GetFaceBlendshapes();
            if (blendshapes_result.ok()) {
                const auto& blendshapes = blendshapes_result.value();
                std::cout << "Blendshapes:" << std::endl;
                for (const auto& classification : blendshapes.classification()) {
                    std::cout << "  " << classification.label() << ": " << classification.score() << std::endl;
                }
            } else {
                LOG(ERROR) << "Error getting blendshapes: " << blendshapes_result.status().message();
            }
        } else {
            LOG(ERROR) << "Error during Run: " << result.status().message();
        }

        if (!output_folder.empty()) {
            std::string output_path = output_folder + "/output_" + std::to_string(frame_count) + ".jpg";
            cv::imwrite(output_path, frame_resized);
            std::cout << "Saved output image to: " << output_path << std::endl;
        }else{
            cv::imshow("Face Landmarker", frame_resized);
            cv::waitKey(0); // Wait for a key press to show the next image
            //if (cv::waitKey(1) == 'q') break;
        }
        frame_count++;
    }

    // Clean up
    absl::Status dispose_status = landmarker.Dispose();
    if (!dispose_status.ok()) {
        LOG(ERROR) << "Failed to dispose: " << dispose_status.message();
        return 1;
    }

    LOG(INFO) << "Application finished gracefully.";
    return 0;
}