set BAZEL_SH=C:\msys64\usr\bin\bash.exe
set BAZEL_VS=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community
set BAZEL_VC=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC
set BAZEL_VC_FULL_VERSION=14.29.30133
set BAZEL_WINSDK_FULL_VERSION=10.0.14393.0
set PATH=%PATH%;d:\Projects\Booth\bazelisk\;

cd d:\Projects\Booth\mediapipe\

bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1  mediapipe/examples/desktop/mediapipe_lib/mdpplib:mdpplib --experimental_cc_shared_library

DIR D:\Projects\Booth\mediapipe\bazel-bin\mediapipe\examples\desktop\mediapipe_lib\mdpplib\mdpplib.dll
cp D:\Projects\Booth\mediapipe\bazel-bin\mediapipe\examples\desktop\mediapipe_lib\mdpplib\mdpplib.dll D:\Projects\Booth\VMRS\Sources\target\debug\