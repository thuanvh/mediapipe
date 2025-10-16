set BAZEL_SH=C:\msys64\usr\bin\bash.exe
set BAZEL_VS=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community
set BAZEL_VC=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC
set BAZEL_VC_FULL_VERSION=14.29.30133
set BAZEL_WINSDK_FULL_VERSION=10.0.14393.0
set PATH=%PATH%;d:\Projects\Booth\bazelisk\;




# NOT RUN

# bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 --action_env PYTHON_BIN_PATH="C://python_36//python.exe" mediapipe/examples/desktop/hello_world
#bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 --action_env PYTHON_BIN_PATH="C://Users//Admin//AppData//Local//Programs//Python//Python312//python.exe" mediapipe/examples/desktop/hello_world
#bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 --action_env PYTHON_BIN_PATH="//c//Users//Admin//AppData//Local//Programs//Python//Python312//python.exe" mediapipe/examples/desktop/hello_world

# RUN OK
#bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/examples/desktop/hello_world
#bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/examples/desktop/face_mesh:face_mesh_cpu
#bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/examples/desktop/face_detection:face_detection_cpu
#bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/examples/desktop/selfie_segmentation:selfie_segmentation_cpu

bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/examples/desktop/mediapipe_lib:mediapipe_lib --experimental_cc_shared_library
bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/examples/desktop/mediapipe_lib:mdpplib --experimental_cc_shared_library
bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/examples/desktop/mediapipe_lib:image_face_blendshapes_app --experimental_cc_shared_library
bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/examples/desktop/face_mesh:face_mesh_cpu --experimental_cc_shared_library

bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/modules/face_landmark:face_landmark_with_attention.tflite  --experimental_cc_shared_library

wget https://storage.googleapis.com/mediapipe-assets/face_detection_short_range.tflite
cp .\face_detection_short_range.tflite mediapipe/modules/face_detection/face_detection_short_range.tflite



set BAZEL_SH=C:\msys64\usr\bin\bash.exe
set BAZEL_WINSDK_FULL_VERSION=10.0.14393.0
set PATH=%PATH%;d:\Projects\Booth\bazelisk\;C:\Users\Admin\AppData\Local\Programs\Python\Python312;
set BAZEL_VS=C:\Program Files\Microsoft Visual Studio\2022\Community
SET BAZEL_VC=%BAZEL_VS%\VC
SET BAZEL_VC_FULL_VERSION=14.44.35207

new_local_repository(
    name = "windows_opencv",
    build_file = "@//third_party:opencv_windows.BUILD",
    path = "D:\\Projects\\Booth\\opencv-4.9.0\\opencv\\build",
    #path = "C:\\Projects\\VM\\opencv-3.4.10\\opencv\\build",
)


bazel build --cxxopt=/std:c++20 -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/examples/desktop/hello_world
bazel build --cxxopt=/std:c++20 -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/examples/desktop/face_mesh:face_mesh_cpu