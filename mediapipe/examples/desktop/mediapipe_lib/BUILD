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
    srcs = ["ImageInferenceTflite.cpp","ImageInferenceTflite.h","MediapipeLibExport.h"],
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
        "@org_tensorflow//tensorflow/lite:framework",
        "@org_tensorflow//tensorflow/lite/delegates/xnnpack:xnnpack_delegate",
        "@org_tensorflow//tensorflow/lite/kernels:builtin_ops",
    ],
    defines = ["_MEDIAPIPE_LIB_EXPORT_"],
)

cc_binary(
    name = "image_inference_tflite_test",
    srcs = ["ImageInferenceTfliteTest.cpp"],
    deps = [
        "//mediapipe/examples/desktop/mediapipe_lib:image_inference_tflite",
    ],
)
cc_shared_library(
    name = "mediapipe_lib_tflite",
    #data = ["//mediapipe/modules/face_landmark:face_landmark_with_attention.tflite"],
    deps = [
        "//mediapipe/examples/desktop/mediapipe_lib:simple_run_graph_lib",
        "//mediapipe/examples/desktop/mediapipe_lib:image_inference_tflite",
        "//mediapipe/graphs/face_mesh:desktop_calculators",
    ],
)

# CPU ONLY
cc_shared_library(
    name = "mediapipe_lib",
    #data = ["//mediapipe/modules/face_landmark:face_landmark_with_attention.tflite"],
    deps = [
        "//mediapipe/examples/desktop/mediapipe_lib:demo_run_graph_lib",
        "//mediapipe/examples/desktop/mediapipe_lib:image_inference_tflite",
        "//mediapipe/graphs/face_mesh:desktop_live_calculators",
    ],
    exports_filter = ["//mediapipe/examples/desktop/mediapipe_lib:demo_run_graph_lib",],
    #win_def_file = "win.def"
)

# Linux only
cc_shared_library(
    name = "mediapipe_lib_gpu",
    #data = ["//mediapipe/modules/face_landmark:face_landmark_with_attention.tflite"],
    deps = [
        "//mediapipe/examples/desktop/mediapipe_lib:demo_run_graph_main_lib",
        "//mediapipe/graphs/face_mesh:desktop_live_gpu_calculators",
    ],
)
