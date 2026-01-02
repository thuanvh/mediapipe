/// Interface for ImageLandmarker and ImageFaceBlendshapes
#pragma once
#include <string>
#include <vector>
#include "mediapipe/framework/calculator_graph.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/port/statusor.h"
#include "MdppLibExport.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/classification.pb.h"
#include <cstdint>
namespace mdpplib {
// Define the expected output type for clarity
using FaceLandmarks = std::vector<::mediapipe::NormalizedLandmarkList>;
using FaceBlendshapes = std::vector<::mediapipe::ClassificationList>;
class ImageFaceLandmarker;
class ImageFaceBlendshapes;
class ImageLandmarkInterface {
public:
    virtual ~ImageLandmarkInterface() = default;
    virtual absl::Status Initialize(const std::string& graph_config_path) = 0;
    virtual absl::StatusOr<FaceLandmarks> Run(const uint8_t* pixel_data, int width, int height, int64_t timestamp_us) = 0;
    virtual absl::StatusOr<mediapipe::ClassificationList> GetFaceBlendshapes() = 0;
    virtual absl::Status Dispose() = 0;
    virtual void SetTimeOut(int timeout_ms) = 0;
};
} // namespace mdpplib