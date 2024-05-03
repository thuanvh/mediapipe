set BAZEL_SH=C:\msys64\usr\bin\bash.exe
set BAZEL_VS=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community
set BAZEL_VC=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC
set BAZEL_VC_FULL_VERSION=14.29.30133
set BAZEL_WINSDK_FULL_VERSION=10.0.14393.0

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