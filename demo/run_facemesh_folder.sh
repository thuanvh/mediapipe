export GLOG_logtostderr=1
bazel-bin/mediapipe/examples/desktop/face_mesh/face_mesh_cpu \
--calculator_graph_config_file=mediapipe/graphs/face_mesh/face_mesh_desktop_live.pbtxt \
--input_image_folder=/home/thuan/sandbox/images/beard_people \
--rotate=-1 \
--output_image_folder=/home/thuan/sandbox/images/beard_people_output
