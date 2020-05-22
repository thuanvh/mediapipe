set GLOG_logtostderr=1
bazel-bin\mediapipe\examples\desktop\face_detection\face_detection_cpu ^
--calculator_graph_config_file=mediapipe\graphs\face_detection\face_detection_desktop_live.pbtxt ^
--input_image_folder=G:\Drive_O\images\Beard_People ^
--rotate=-1 ^
--output_image_folder=O:\sandbox\vmakeup\deploy\icaochecker\Output_BaseID3_Bg_12
