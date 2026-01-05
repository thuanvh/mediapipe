#include "MdppShim.h"
#include "FaceLandmarkerLib.h"
#include "ImageInferenceTflite.h"
#include <string.h>
#include <iostream>

extern "C" {

void* Mdpp_CreateLandmarker() {
    return new mdpplib::MdppFaceLandmarker();
}

void Mdpp_DestroyLandmarker(void* handle) {
    if (handle) {
        delete static_cast<mdpplib::MdppFaceLandmarker*>(handle);
    }
}

int Mdpp_Initialize(void* handle, const char* graph_config_path, int with_blendshapes) {
    if (!handle) return -1;
    try {
        return static_cast<mdpplib::MdppFaceLandmarker*>(handle)->Initialize(graph_config_path, with_blendshapes != 0);
    } catch (...) {
        std::cerr << "Exception in Mdpp_Initialize" << std::endl;
        return -1;
    }
}

MdppResultC Mdpp_Run(void* handle, const unsigned char* data, int width, int height, int channels) {
    MdppResultC result = {nullptr, 0};
    if (!handle || !data) return result;

    try {
        auto landmarks = static_cast<mdpplib::MdppFaceLandmarker*>(handle)->Run(data, width, height, channels);
        
        result.face_count = (int)landmarks.size();
        if (result.face_count > 0) {
            result.faces = new MdppFaceC[result.face_count];
            for (int i = 0; i < result.face_count; ++i) {
                result.faces[i].point_count = (int)landmarks[i].size();
                result.faces[i].points = new MdppPointC[result.faces[i].point_count];
                for (int j = 0; j < result.faces[i].point_count; ++j) {
                    result.faces[i].points[j].x = landmarks[i][j].x;
                    result.faces[i].points[j].y = landmarks[i][j].y;
                }
            }
        }
    } catch (...) {
        std::cerr << "Exception in Mdpp_Run" << std::endl;
    }
    return result;
}

void Mdpp_FreeResult(MdppResultC result) {
    if (result.faces) {
        for (int i = 0; i < result.face_count; ++i) {
            delete[] result.faces[i].points;
        }
        delete[] result.faces;
    }
}

MdppBlendshapeResultC Mdpp_GetBlendshapes(void* handle) {
    MdppBlendshapeResultC result = {nullptr, 0};
    if (!handle) return result;

    try {
        auto blendshapes = static_cast<mdpplib::MdppFaceLandmarker*>(handle)->GetFaceBlendshapes();
        result.count = (int)blendshapes.size();
        if (result.count > 0) {
            result.blendshapes = new MdppBlendshapeC[result.count];
            for (int i = 0; i < result.count; ++i) {
                result.blendshapes[i].label = _strdup(blendshapes[i].label.c_str());
                result.blendshapes[i].score = blendshapes[i].score;
            }
        }
    } catch (...) {
        std::cerr << "Exception in Mdpp_GetBlendshapes" << std::endl;
    }
    return result;
}

void Mdpp_FreeBlendshapes(MdppBlendshapeResultC result) {
    if (result.blendshapes) {
        for (int i = 0; i < result.count; ++i) {
            free(result.blendshapes[i].label);
        }
        delete[] result.blendshapes;
    }
}

void Mdpp_SetTimeOut(void* handle, int timeout_ms) {
    if (handle) {
        static_cast<mdpplib::MdppFaceLandmarker*>(handle)->SetTimeOut(timeout_ms);
    }
}

// ImageInferenceTflite
void* Mdpp_CreateImageInference() {
    return new ImageInferenceTflite();
}

void Mdpp_DestroyImageInference(void* handle) {
    if (handle) {
        delete static_cast<ImageInferenceTflite*>(handle);
    }
}

void Mdpp_LoadNet(void* handle, const char* filename, const char* input_layer, const char* output_layer) {
    if (handle && filename) {
        static_cast<ImageInferenceTflite*>(handle)->LoadNet(filename, input_layer ? input_layer : "", output_layer ? output_layer : "");
    }
}

int Mdpp_Inference(void* handle, const unsigned char* pixel_data, int width, int height, int use_scale, float scale, float subtract, float* output_data, int* output_size) {
    if (!handle || !pixel_data || !output_data || !output_size) return -1;
    try {
        auto result = static_cast<ImageInferenceTflite*>(handle)->Inference(pixel_data, width, height, use_scale != 0, scale, subtract);
        *output_size = (int)result.size();
        std::copy(result.begin(), result.end(), output_data);
        return 0;
    } catch (...) {
        return -1;
    }
}

int Mdpp_Predict(void* handle, const unsigned char* pixel_data, int width, int height, int use_scale, float scale, float subtract, float* prob_data, int* prob_size) {
    if (!handle || !pixel_data || !prob_data || !prob_size) return -1;
    try {
        std::vector<float> prob;
        int status = static_cast<ImageInferenceTflite*>(handle)->Predict(pixel_data, width, height, use_scale != 0, scale, subtract, prob);
        if (status == 0) {
            *prob_size = (int)prob.size();
            std::copy(prob.begin(), prob.end(), prob_data);
        }
        return status;
    } catch (...) {
        return -1;
    }
}

}
