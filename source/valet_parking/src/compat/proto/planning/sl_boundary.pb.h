#pragma once

namespace TL {
namespace planning {

class SLBoundary {
 public:
  void set_start_s(double value) { start_s_ = value; }
  void set_end_s(double value) { end_s_ = value; }
  void set_start_l(double value) { start_l_ = value; }
  void set_end_l(double value) { end_l_ = value; }

  double start_s() const { return start_s_; }
  double end_s() const { return end_s_; }
  double start_l() const { return start_l_; }
  double end_l() const { return end_l_; }

 private:
  double start_s_{0.0};
  double end_s_{0.0};
  double start_l_{0.0};
  double end_l_{0.0};
};

}  // namespace planning
}  // namespace TL
