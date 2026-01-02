#pragma once
#include <vector>
#include "MdppLibExport.h"
#include <string>



namespace mdpplib {
class ImageLandmarkInterface;

// Define the expected output type for clarity
class _MDPP_LIB_API_ MdppLandmarkPoint {
public:
    float x;
    float y;
};

class _MDPP_LIB_API_ MdppFaceBlendshape {
public:
    std::string label;
    float score;
};

using MdppFaceLandmarks = std::vector<std::vector<MdppLandmarkPoint> >;

class _MDPP_LIB_API_ MdppFaceLandmarker {
public:

    MdppFaceLandmarker() = default;
    
    int Initialize(const std::string& graph_config_path, bool with_blendshapes = false);

    MdppFaceLandmarks Run(const unsigned char* input_frame_data, int width, int height, int channel);

    std::vector<MdppFaceBlendshape> GetFaceBlendshapes();

    int Dispose() ;
    void SetTimeOut(int timeout_ms);
private:
    ImageLandmarkInterface* face_landmarker_;
    bool with_blendshapes_ = false;
};

} // namespace mdpplib