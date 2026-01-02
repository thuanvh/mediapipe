#pragma once
#include "MdppLibExport.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x;
    float y;
} MdppPointC;

typedef struct {
    MdppPointC* points;
    int point_count;
} MdppFaceC;

typedef struct {
    MdppFaceC* faces;
    int face_count;
} MdppResultC;

typedef struct {
    char* label;
    float score;
} MdppBlendshapeC;

typedef struct {
    MdppBlendshapeC* blendshapes;
    int count;
} MdppBlendshapeResultC;

_MDPP_LIB_API_ void* Mdpp_CreateLandmarker();
_MDPP_LIB_API_ void Mdpp_DestroyLandmarker(void* handle);
_MDPP_LIB_API_ int Mdpp_Initialize(void* handle, const char* graph_config_path, int with_blendshapes);
_MDPP_LIB_API_ MdppResultC Mdpp_Run(void* handle, const unsigned char* data, int width, int height, int channels);
_MDPP_LIB_API_ void Mdpp_FreeResult(MdppResultC result);
_MDPP_LIB_API_ MdppBlendshapeResultC Mdpp_GetBlendshapes(void* handle);
_MDPP_LIB_API_ void Mdpp_FreeBlendshapes(MdppBlendshapeResultC result);
_MDPP_LIB_API_ void Mdpp_SetTimeOut(void* handle, int timeout_ms);

// ImageInferenceTflite
_MDPP_LIB_API_ void* Mdpp_CreateImageInference();
_MDPP_LIB_API_ void Mdpp_DestroyImageInference(void* handle);
_MDPP_LIB_API_ void Mdpp_LoadNet(void* handle, const char* filename, const char* input_layer, const char* output_layer);
_MDPP_LIB_API_ int Mdpp_Inference(void* handle, const unsigned char* pixel_data, int width, int height, int use_scale, float scale, float* output_data, int* output_size);
_MDPP_LIB_API_ int Mdpp_Predict(void* handle, const unsigned char* pixel_data, int width, int height, int use_scale, float scale, float* prob_data, int* prob_size);

#ifdef __cplusplus
}
#endif
