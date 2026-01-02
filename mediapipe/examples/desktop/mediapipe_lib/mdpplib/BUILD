# Copyright 2019 The MediaPipe Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

licenses(["notice"])

package(default_visibility = ["//mediapipe/examples:__subpackages__"])

cc_library(
    name = "demo_run_graph_lib",
    srcs = ["demo_run_graph_lib.cc","mediapipelib.h"],
    deps = [
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/formats:image_frame",
        "//mediapipe/framework/formats:image_frame_opencv",
        "//mediapipe/framework/port:file_helpers",
        "//mediapipe/framework/port:opencv_highgui",
        "//mediapipe/framework/port:opencv_imgproc",
        "//mediapipe/framework/port:opencv_video",
        "//mediapipe/framework/port:parse_text_proto",
        "//mediapipe/framework/port:status",
        "//mediapipe/util:resource_util",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/log:absl_log",
    ],
)


cc_library(
    name = "image_inference_tflite",
    srcs = ["ImageInferenceTflite.cpp","ImageInferenceTflite.h","MdppLibExport.h"],
    deps = [
        "//mediapipe/framework/port:file_helpers",
        "//mediapipe/framework/port:status",
        "//mediapipe/util:resource_util",
        "@com_google_absl//absl/log:absl_log",
        "@org_tensorflow//tensorflow/lite:framework",
        "@org_tensorflow//tensorflow/lite/delegates/xnnpack:xnnpack_delegate",
        "@org_tensorflow//tensorflow/lite/kernels:builtin_ops",
    ],
    defines = ["_MDPP_LIB_EXPORT_"],
)
cc_library(
    name = "face_landmarker_lib",
    srcs = ["ImageFaceLandmarker.cpp", 
            "ImageFaceLandmarker.h", 
            "ImageLandmarkInterface.h", 
            "FaceLandmarkerLib.cpp", 
            "FaceLandmarkerLib.h", "MdppLibExport.h", "ImageFaceBlendshapes.h"],
    deps = [
        "//mediapipe/examples/desktop/mediapipe_lib:image_inference_tflite",
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/formats:classification_cc_proto",
        "//mediapipe/framework/formats:landmark_cc_proto",
        "//mediapipe/framework/formats:image",
        "//mediapipe/framework/formats:image_frame",
    ],
    defines = ["_MDPP_LIB_EXPORT_"],
)
cc_library(
    name = "face_blendshapes_lib",
    srcs = ["ImageFaceBlendshapes.cpp", 
            "ImageFaceBlendshapes.h", 
            "ImageFaceLandmarker.cpp", 
            "ImageFaceLandmarker.h", 
            "FaceLandmarkerLib.cpp",
            "FaceLandmarkerLib.h",
            "ImageLandmarkInterface.h",
            "MdppLibExport.h",
            "MdppShim.cpp",
            "MdppShim.h",
            "VectorToNormalizedLandmarkListCalculator.cc"],
    deps = [
        ":image_inference_tflite",
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/formats:classification_cc_proto",
        "//mediapipe/framework/formats:landmark_cc_proto",
        "//mediapipe/calculators/core:constant_side_packet_calculator",
        #"//mediapipe/calculators/core:constant_side_packet_calculator_proto",
        #"//mediapipe/graphs/face_mesh:desktop_calculators",
        #"//mediapipe/examples/desktop:demo_run_graph_main",
        #"//mediapipe/graphs/face_mesh:desktop_live_calculators",
        "//mediapipe/modules/face_landmark:face_landmark_front_cpu_image",
        "//mediapipe/tasks/cc/vision/face_landmarker:face_blendshapes_graph", 
        "//mediapipe/framework/formats:image",
        "//mediapipe/framework/formats:image_frame",
        "//mediapipe/framework/port:parse_text_proto",
        #":vector_to_landmark_calc",
    ],
    defines = ["_MDPP_LIB_EXPORT_"],
)
cc_binary(
    name = "image_inference_tflite_test",
    srcs = ["ImageInferenceTfliteTest.cpp"],
    deps = [
        "//mediapipe/examples/desktop/mediapipe_lib:image_inference_tflite",
    ],
)
cc_binary(
    name = "image_face_landmarker_app",
    srcs = ["ImageFaceLandmarkerApp.cpp"],
    deps = [
        "//mediapipe/examples/desktop/mediapipe_lib:image_inference_tflite",
        "//mediapipe/examples/desktop/mediapipe_lib:face_landmarker_lib",
        "//mediapipe/framework/formats:landmark_cc_proto",
        #"//mediapipe/calculators/core:constant_side_packet_calculator",
        #"//mediapipe/calculators/core:constant_side_packet_calculator_proto",
        #"//mediapipe/graphs/face_mesh:desktop_calculators",
        "//mediapipe/examples/desktop:demo_run_graph_main",
        "//mediapipe/graphs/face_mesh:desktop_live_calculators",
    ],
)
cc_library(
  name = "vector_to_landmark_calc",
  srcs = ["VectorToNormalizedLandmarkListCalculator.cc"],
  deps = [
    "//mediapipe/framework:calculator_framework",
    "//mediapipe/framework/formats:landmark_cc_proto",
  ],
)
cc_binary(
    name = "image_face_blendshapes_app",
    srcs = ["ImageFaceBlendshapesApp.cpp"],
    deps = [
        "//mediapipe/examples/desktop/mediapipe_lib:image_inference_tflite",
        "//mediapipe/examples/desktop/mediapipe_lib:face_blendshapes_lib",
        #":vector_to_landmark_calc",
        "//mediapipe/framework/formats:landmark_cc_proto",
        #"//mediapipe/calculators/core:constant_side_packet_calculator",
        #"//mediapipe/calculators/core:constant_side_packet_calculator_proto",
        #"//mediapipe/graphs/face_mesh:desktop_calculators",
        "//mediapipe/examples/desktop:demo_run_graph_main",
        "//mediapipe/graphs/face_mesh:desktop_live_calculators",
        "//mediapipe/tasks/cc/vision/face_landmarker:face_blendshapes_graph", 
    ],
)
cc_binary(
    name = "video_face_landmarker_app",
    srcs = ["VideoFaceLandmarkerApp.cpp", "VideoFaceLandmarker.cpp", "VideoFaceLandmarker.h"],
    deps = [
        "//mediapipe/examples/desktop/mediapipe_lib:image_inference_tflite",
        "//mediapipe/framework/formats:landmark_cc_proto",
        #"//mediapipe/calculators/core:constant_side_packet_calculator",
        #"//mediapipe/calculators/core:constant_side_packet_calculator_proto",
        #"//mediapipe/graphs/face_mesh:desktop_calculators",
        "//mediapipe/examples/desktop:demo_run_graph_main",
        "//mediapipe/graphs/face_mesh:desktop_live_calculators",
    ],
)
cc_shared_library(
    name = "mdpplib_tflite",
    #data = ["//mediapipe/modules/face_landmark:face_landmark_with_attention.tflite"],
    deps = [
        "//mediapipe/examples/desktop/mediapipe_lib:simple_run_graph_lib",
        "//mediapipe/examples/desktop/mediapipe_lib:image_inference_tflite",
        "//mediapipe/graphs/face_mesh:desktop_calculators",
        "//mediapipe/examples/desktop/mediapipe_lib:face_landmarker_lib",
    ],
)

# CPU ONLY
cc_shared_library(
    name = "mdpplib",
    #data = ["//mediapipe/modules/face_landmark:face_landmark_with_attention.tflite"],
    deps = [        
        "//mediapipe/examples/desktop/mediapipe_lib/mdpplib:face_blendshapes_lib",
    ],    
)

# Linux only
cc_shared_library(
    name = "mdpplib_gpu",
    #data = ["//mediapipe/modules/face_landmark:face_landmark_with_attention.tflite"],
    deps = [
        "//mediapipe/examples/desktop/mediapipe_lib:demo_run_graph_main_lib",
        "//mediapipe/graphs/face_mesh:desktop_live_gpu_calculators",
    ],
)
