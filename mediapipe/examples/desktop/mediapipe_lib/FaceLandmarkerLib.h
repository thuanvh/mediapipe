#pragma once
#include <vector>
#include "MdppLibExport.h"




namespace mdpplib {
class ImageFaceLandmarker;
// Define the expected output type for clarity
class _MDPP_LIB_API_ MdppLandmarkPoint {
public:
    float x;
    float y;
};
using MdppFaceLandmarks = std::vector<std::vector<MdppLandmarkPoint> >;

class _MDPP_LIB_API_ MdppFaceLandmarker {
public:

    MdppFaceLandmarker() = default;
    


    int Initialize(const std::string& graph_config_path) ;


    MdppFaceLandmarks Run(const unsigned char* input_frame_data, int width, int height, int channel);


    int Dispose() ;
    void SetTimeOut(int timeout_ms);
private:
    ImageFaceLandmarker* face_landmarker_;
};

} // namespace mdpplib