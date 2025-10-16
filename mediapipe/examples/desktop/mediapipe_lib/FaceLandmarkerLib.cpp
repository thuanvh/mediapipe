#pragma once
#include <vector>
#include "MdppLibExport.h"
#include "ImageFaceLandmarker.h"
#include "ImageFaceBlendshapes.h"
#include "FaceLandmarkerLib.h"
#include <iostream>

namespace mdpplib {

using FaceLandmarks = std::vector<::mediapipe::NormalizedLandmarkList>;

using MdppFaceLandmarks = std::vector<std::vector<MdppLandmarkPoint> >;


    /**
     * @brief Initializes the MediaPipe CalculatorGraph.
     * @param graph_config_path Path to the .pbtxt graph file.
     * @return absl::Status::Ok() on success.
     */
    int MdppFaceLandmarker::Initialize(const std::string& graph_config_path, bool with_blendshapes) {
        std::cout << "MdppFaceLandmarker::Initialize: " << graph_config_path << std::endl;
        


        with_blendshapes_ = with_blendshapes;
            if (face_landmarker_ == nullptr) {
                if (with_blendshapes) {
                    face_landmarker_ = new ImageFaceBlendshapes();
                } else {
                    face_landmarker_ = new ImageFaceLandmarker();
                }
            }
        
        absl::Status status = face_landmarker_->Initialize(graph_config_path);
        std::cout << "MdppFaceLandmarker::Initialize status: " << status.message() << std::endl;
        return (int)status.code();
    }

    /**
     * @brief Processes a single video frame and returns face landmarks.
     * @param input_frame The input frame (OpenCV Mat, expected to be BGR).
     * @param timestamp_us Current frame timestamp in microseconds.
     * @return A vector of NormalizedLandmarkList, one for each detected face.
     */
    MdppFaceLandmarks MdppFaceLandmarker::Run(const unsigned char* input_frame_data, int width, int height, int channel){
        if (face_landmarker_ == nullptr) {
            std::cerr << "MdppFaceLandmarker::Run: FaceLandmarker not initialized." << std::endl;
            return MdppFaceLandmarks();
        }

        // 1. Convert input data to OpenCV Mat
        cv::Mat input_frame(height, width, (channel == 3) ? CV_8UC3 : CV_8UC4, (void*)input_frame_data);
        
        // Get current time in microseconds
        int64_t timestamp_us = std::chrono::duration_cast<std::chrono::microseconds>(
             std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        //int64_t timestamp_us =  //0;//frame_count * 1000 * 1000 / 30; // Mock 30 FPS timestamp
        // 2. Run the face landmarker
        absl::StatusOr<FaceLandmarks> result = face_landmarker_->Run(input_frame, timestamp_us);
        if (!result.ok()) {
            return MdppFaceLandmarks();
        }
        

        // 3. Convert the result to MdppFaceLandmarks
        MdppFaceLandmarks mdpp_landmarks;
        for (const auto& landmark_list : *result) {
            std::vector<MdppLandmarkPoint> mdpp_landmarks_list;
            for (const auto& landmark : landmark_list.landmark()) {
                MdppLandmarkPoint point;
                point.x = landmark.x() * width;
                point.y = landmark.y() * height;
                mdpp_landmarks_list.push_back(point);
            }
            mdpp_landmarks.push_back(mdpp_landmarks_list);
        }
        // if (result.ok()) {
        //     const auto& face_landmarks = result.value();
            
        //     // Draw the landmarks on the frame
        //     for (const auto& face : face_landmarks) {
        //         for (int i = 0; i < face.landmark_size(); ++i) {
        //             const auto& landmark = face.landmark(i);
        //             // Convert normalized coordinates [0.0, 1.0] to pixel coordinates
        //             int x = static_cast<int>(landmark.x() * frame.cols);
        //             int y = static_cast<int>(landmark.y() * frame.rows);
        //             cv::circle(frame, cv::Point(x, y), 1, cv::Scalar(0, 255, 0), -1);
        //         }
        //     }
        // } else {
        //     LOG(ERROR) << "Error during Run: " << result.status().message();
        // }
        return mdpp_landmarks;
    }

    std::vector<MdppFaceBlendshape> MdppFaceLandmarker::GetFaceBlendshapes() {
        if(with_blendshapes_ && face_landmarker_ != nullptr) {
            absl::StatusOr<mediapipe::ClassificationList> result = face_landmarker_->GetFaceBlendshapes();
            if (!result.ok()) {
                return std::vector<MdppFaceBlendshape>();
            }
            std::vector<MdppFaceBlendshape> mdpp_blendshapes;
            for (const auto& classification : result->classification()) {
                MdppFaceBlendshape blendshape;
                blendshape.label = classification.label();
                blendshape.score = classification.score();
                mdpp_blendshapes.push_back(blendshape);
            }
            return mdpp_blendshapes;
        }
        return std::vector<MdppFaceBlendshape>();
    }

    /**
     * @brief Closes all packet sources and waits for the graph to finish.
     * @return absl::Status::Ok() on success.
     */
    int MdppFaceLandmarker::Dispose(){
        if (face_landmarker_ != nullptr) {
            int status = (int)(face_landmarker_->Dispose().code());
            delete face_landmarker_;
            face_landmarker_ = nullptr;
            return status;
        }
        return 0;
    }
    void MdppFaceLandmarker::SetTimeOut(int timeout_ms) {
        if (face_landmarker_ != nullptr) {
            face_landmarker_->SetTimeOut(timeout_ms);
        }
    }
} // namespace mdpplib