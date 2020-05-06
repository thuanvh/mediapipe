export GLOG_logtostderr=1
bazel-bin/mediapipe/examples/desktop/face_mesh/face_mesh_cpu \
--calculator_graph_config_file=mediapipe/graphs/face_mesh/face_mesh_desktop_live.pbtxt \
--input_video_path=/home/thuan/20170810_130308.mp4 \
--rotate=2 \
--output_video_path=/home/thuan/mediapipe_facemesh.mp4
