#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/packet.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/formats/landmark.pb.h"

namespace mediapipe {

class VectorToNormalizedLandmarkListCalculator : public CalculatorBase {
 public:
  static absl::Status GetContract(CalculatorContract* cc) {
    cc->Inputs().Index(0).Set<std::vector<mediapipe::NormalizedLandmarkList>>();
    cc->Outputs().Index(0).Set<mediapipe::NormalizedLandmarkList>();
    return absl::OkStatus();
  }

  absl::Status Process(CalculatorContext* cc) override {
    if (cc->Inputs().Index(0).IsEmpty()) return absl::OkStatus();
    const auto& vec = cc->Inputs().Index(0).Get<std::vector<mediapipe::NormalizedLandmarkList>>();
    if (!vec.empty()) {
      // emit the first face's landmarks (change to loop if you want to emit all faces)
      auto packet = MakePacket<mediapipe::NormalizedLandmarkList>(vec[0]).At(cc->InputTimestamp());
      cc->Outputs().Index(0).AddPacket(packet);
    }
    return absl::OkStatus();
  }
};

REGISTER_CALCULATOR(VectorToNormalizedLandmarkListCalculator);

}  // namespace mediapipe