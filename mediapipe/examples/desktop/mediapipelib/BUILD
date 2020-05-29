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

licenses(["notice"])  # Apache 2.0

package(default_visibility = ["//mediapipe/examples:__subpackages__"])

cc_library(
    name = "mediapipelib_static",
    srcs = ["mp_face_detection.cc"],
    hdrs = ["mp_face_detection.h", "mp_export.h"],
    defines = ["_MEDIAPIPE_LIB_EXPORT_"],
    deps = [
        "//mediapipe/examples/desktop:demo_run_graph_main",
        "//mediapipe/graphs/face_detection:desktop_tflite_calculators",
    ],
)

# cc_binary(
#     name = "mediapipe.dll",
#     defines = ["_MEDIAPIPE_LIB_EXPORT_"],
#     deps = [
#         #"//mediapipe/examples/desktop:demo_run_graph_main",
#         "//mediapipe/graphs/face_detection:desktop_tflite_calculators",
#         ":mediapipelib_static",
#     ],
#     linkshared = 1,
#     #linkstatic = 1,
# )
cc_library(
    name = "mediapipe_header",
    hdrs = ["mp_face_detection.h", "mp_export.h"],
    visibility = ["//visibility:public"],
)
cc_binary(
    name = "mediapipe.dll",
    srcs = ["mp_face_detection.cc"],
    defines = ["_MEDIAPIPE_LIB_EXPORT_"],
    deps = [
        "//mediapipe/examples/desktop:demo_run_graph_main",
        "//mediapipe/graphs/face_detection:desktop_tflite_calculators",
        ":mediapipe_header",
        #":mediapipelib_static",
    ],
    linkshared = 1,
    #linkstatic = 1,
)

cc_import(
  name = "mediapipe_shared",
  hdrs = ["mp_face_detection.h", "mp_export.h"],
  shared_library = "mediapipe.dll",
  interface_library = "mediapipe.lib", 
)

cc_binary(
    name = "mediapipe_face_detection",
    srcs = ["mp_face_detection_main.cc"],
    deps = [
        ":mediapipelib_static",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_glog_glog//:glog",
        #"//third_party:opencv",
        #"//mediapipe/examples/desktop:demo_run_graph_main",
        "//mediapipe/graphs/face_detection:desktop_tflite_calculators",
    ],
    #linkstatic = 0,
)
# # Linux only
# cc_binary(
#     name = "face_detection_gpu",
#     deps = [
#         "//mediapipe/examples/desktop:demo_run_graph_main_gpu",
#         "//mediapipe/graphs/face_detection:mobile_calculators",
#     ],
# )
