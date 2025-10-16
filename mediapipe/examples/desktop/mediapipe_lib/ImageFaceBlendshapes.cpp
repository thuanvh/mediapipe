#include "ImageFaceBlendshapes.h"
#include "mediapipe/framework/formats/image.h"
#include "mediapipe/framework/formats/image_opencv.h"
using namespace mediapipe;
namespace mdpplib {

// Define the expected output type for clarity
using FaceLandmarks = std::vector<::mediapipe::NormalizedLandmarkList>;


    /**
     * @brief Initializes the MediaPipe CalculatorGraph.
     * @param graph_config_path Path to the .pbtxt graph file.
     * @return absl::Status::Ok() on success.
     */
    absl::Status ImageFaceBlendshapes::Initialize(const std::string& graph_config_path) 
    {
        LOG(INFO) << "Initializing MediaPipe graph...";

        // 1. Read the Graph config file
        std::string graph_config_string;
        MP_RETURN_IF_ERROR(mediapipe::file::GetContents(graph_config_path, &graph_config_string));
        CalculatorGraphConfig config = 
            ::mediapipe::ParseTextProtoOrDie<CalculatorGraphConfig>(graph_config_string);

        // 2. Initialize the graph
        MP_RETURN_IF_ERROR(graph_.Initialize(config));
        
        // 3. Setup the output poller
        MP_ASSIGN_OR_RETURN(
            poller_,
            graph_.AddOutputStreamPoller(kLandmarksStream));

        MP_ASSIGN_OR_RETURN(
            poller_face_blendshapes_,
            graph_.AddOutputStreamPoller(kFaceBlendshapesStream));

        // 4. Start the graph
        MP_RETURN_IF_ERROR(graph_.StartRun({}));
        
        graph_initialized_ = true;
        LOG(INFO) << "MediaPipe graph initialized and started.";
        return absl::OkStatus();
    }

    /**
     * @brief Processes a single video frame and returns face landmarks.
     * @param input_frame The input frame (OpenCV Mat, expected to be BGR).
     * @param timestamp_us Current frame timestamp in microseconds.
     * @return A vector of NormalizedLandmarkList, one for each detected face.
     */
    absl::StatusOr<FaceLandmarks> ImageFaceBlendshapes::Run(const cv::Mat& input_frame, int64_t timestamp_us) {
        if (!graph_initialized_) {
            return absl::InternalError("Graph is not initialized. Call Initialize() first.");
        }
        //if (poller_)
        //    poller_->Reset();
        // 1. Convert OpenCV Mat to MediaPipe ImageFrame
        // MediaPipe usually expects RGB, so convert BGR to RGB
        std::cout << "Input frame size: " << input_frame.cols << "x" << input_frame.rows << " Channels: " << input_frame.channels() << std::endl;
        cv::Mat rgb_frame;
        cv::cvtColor(input_frame, rgb_frame, cv::COLOR_BGR2RGB);

        auto mp_frame = absl::make_unique<ImageFrame>(
            ImageFormat::SRGB, rgb_frame.cols, rgb_frame.rows, 
            ImageFrame::kDefaultAlignmentBoundary);
        cv::Mat wrapped_mat = formats::MatView(mp_frame.get());
        rgb_frame.copyTo(wrapped_mat);

        auto mp_frame_shared = std::shared_ptr<mediapipe::ImageFrame>(mp_frame.release());
        ::mediapipe::Image mp_image(mp_frame_shared);

        std::cout << "Converted to ImageFrame." << std::endl;
        std::cout << "Timestamp (us): " << timestamp_us << std::endl;
        // 2. Send the packet to the graph
        // MP_RETURN_IF_ERROR(graph_.AddPacketToInputStream(
        //     kInputStream, 
        //     Adopt(mp_frame.release())
        //         .At(Timestamp(timestamp_us))));
        MP_RETURN_IF_ERROR(graph_.AddPacketToInputStream(
            kInputStream,
            MakePacket<::mediapipe::Image>(mp_image).At(Timestamp(timestamp_us))));

        // 3. Get the landmark results
        std::cout << "Waiting for output packet..." << std::endl;
        // wait timeout 100ms
        // sleep for a while to let the graph process
        std::cout << "Timeout (ms): " << timeout_ms_ << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms_));
        Packet packet;
        if (poller_ && poller_->QueueSize() > 0 && poller_->Next(&packet)) {
            // Success: return the face landmarks
            std::cout << "Received output packet." << std::endl;
            return packet.Get<FaceLandmarks>();
        } else {
            // This happens if the graph stalls or is closed.
            std::cout << "No output packet received." << std::endl;
            return FaceLandmarks{};
        }
    }

    absl::StatusOr<mediapipe::ClassificationList> ImageFaceBlendshapes::GetFaceBlendshapes() {
        if (!graph_initialized_) {
            return absl::InternalError("Graph is not initialized. Call Initialize() first.");
        }

        Packet packet;
        if (poller_face_blendshapes_ && poller_face_blendshapes_->QueueSize() > 0 && poller_face_blendshapes_->Next(&packet)) {
            // Success: return the face blendshapes
            std::cout << "Received face blendshapes packet." << std::endl;
            return packet.Get<mediapipe::ClassificationList>();
        } else {
            // This happens if the graph stalls or is closed.
            std::cout << "No face blendshapes packet received." << std::endl;
            return mediapipe::ClassificationList{};
        }
    }

    /**
     * @brief Closes all packet sources and waits for the graph to finish.
     * @return absl::Status::Ok() on success.
     */
    absl::Status ImageFaceBlendshapes::Dispose() {
        if (!graph_initialized_) return absl::OkStatus();

        LOG(INFO) << "Stopping MediaPipe graph...";
        absl::Status close_status = graph_.CloseAllPacketSources();
        absl::Status wait_status = graph_.WaitUntilDone();
        
        graph_initialized_ = false;
        
        // Return the first error encountered, or OkStatus
        if (!close_status.ok()) return close_status;
        return wait_status;
    }


} // namespace mediapipe