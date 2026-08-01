#include "valet_parking_component.h"
#include "valet_parking_topics.h"
#include "valet_parking_topicsTopicDataType.h"

#include "app_build_feature_flags_config.h"

#if ENABLE_PRKVINBUS
#include "prk_vin_bus.h"
#include "prk_vin_busTopicDataType.h"
#endif

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace valet_parking {

namespace {

constexpr const char* kModuleTag = "[valet_parking]";
constexpr const char* kSelectedSlotTopicDefault = "/selected_slot";
constexpr const char* kPlanningTrajectoryTopicDefault = "/planning/trajectory";
constexpr const char* kParkingCommandTopicDefault = "/parking/command";
constexpr const char* kLocalizationTopicDefault = "/localization/estimate";
constexpr const char* kChassisTopicDefault = "/chassis/state";
constexpr const char* kObstacleTopicDefault = "/perception/obstacles";
constexpr const char* kPrkVinBusTopicDefault = "/PrkVinBus";
constexpr int kTrajectoryPointCount = 21;
constexpr std::size_t kMaxOutputWaypointCount = 100U;
constexpr int kMaxCommandDrainRoundsPerCycle = 64;
constexpr int kMaxAuxDrainRoundsPerCycle = 64;

struct SelectedSlotInput {
  uint64_t seq{0U};
  std::string frame_id{"selected_slot_mock_publisher"};
  uint64_t publish_stamp_ms{0U};
  uint64_t data_stamp_ms{0U};

  std::string mode{"valid"};
  bool is_valid{true};
  double center_x{0.0};
  double center_y{0.0};
  double heading{0.0};
  double length{5.0};
  double width{2.4};

  std::string status_reason{"valid selected_slot sample"};
};

uint64_t NowMilliseconds() {
  return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()).count());
}

std::string Trim(const std::string& text) {
  std::size_t begin = 0U;
  while (begin < text.size() && std::isspace(static_cast<unsigned char>(text[begin])) != 0) {
    ++begin;
  }

  std::size_t end = text.size();
  while (end > begin && std::isspace(static_cast<unsigned char>(text[end - 1U])) != 0) {
    --end;
  }

  return text.substr(begin, end - begin);
}

std::string ToLower(std::string text) {
  std::transform(text.begin(), text.end(), text.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return text;
}

std::unordered_map<std::string, std::string> ParseKeyValuePayload(const std::string& payload) {
  std::unordered_map<std::string, std::string> kv;
  std::istringstream stream(payload);
  std::string token;

  while (std::getline(stream, token, ';')) {
    if (token.empty()) {
      continue;
    }

    const auto equal_pos = token.find('=');
    if (equal_pos == std::string::npos) {
      continue;
    }

    const std::string key = Trim(token.substr(0U, equal_pos));
    const std::string value = Trim(token.substr(equal_pos + 1U));
    if (!key.empty()) {
      kv[key] = value;
    }
  }

  return kv;
}

bool ParseUint64(const std::string& text, uint64_t* out_value) {
  if (out_value == nullptr || text.empty()) {
    return false;
  }

  errno = 0;
  char* end = nullptr;
  const unsigned long long parsed = std::strtoull(text.c_str(), &end, 10);
  if (errno != 0 || end == nullptr || *end != '\0') {
    return false;
  }

  *out_value = static_cast<uint64_t>(parsed);
  return true;
}

bool ParseDouble(const std::string& text, double* out_value) {
  if (out_value == nullptr || text.empty()) {
    return false;
  }

  errno = 0;
  char* end = nullptr;
  const double parsed = std::strtod(text.c_str(), &end);
  if (errno != 0 || end == nullptr || *end != '\0') {
    return false;
  }

  *out_value = parsed;
  return true;
}

bool ParseBool(const std::string& text, bool* out_value) {
  if (out_value == nullptr) {
    return false;
  }

  const std::string lowered = ToLower(Trim(text));
  if (lowered == "1" || lowered == "true" || lowered == "yes") {
    *out_value = true;
    return true;
  }
  if (lowered == "0" || lowered == "false" || lowered == "no") {
    *out_value = false;
    return true;
  }

  return false;
}

std::string JsonEscape(const std::string& text) {
  std::string escaped;
  escaped.reserve(text.size() + 16U);

  for (const char c : text) {
    switch (c) {
      case '\\':
        escaped += "\\\\";
        break;
      case '"':
        escaped += "\\\"";
        break;
      case '\n':
        escaped += "\\n";
        break;
      case '\r':
        escaped += "\\r";
        break;
      case '\t':
        escaped += "\\t";
        break;
      default:
        escaped.push_back(c);
        break;
    }
  }

  return escaped;
}

SelectedSlotInput DecodeSelectedSlotInput(const std::string& payload) {
  SelectedSlotInput input;
  input.publish_stamp_ms = NowMilliseconds();
  input.data_stamp_ms = input.publish_stamp_ms;
  input.seq = input.publish_stamp_ms;

  const auto kv = ParseKeyValuePayload(payload);
  if (kv.empty()) {
    input.is_valid = false;
    input.mode = "invalid";
    input.status_reason = "empty or malformed selected_slot payload";
    return input;
  }

  if (const auto seq_it = kv.find("seq"); seq_it != kv.end()) {
    uint64_t parsed_seq = 0U;
    if (ParseUint64(seq_it->second, &parsed_seq)) {
      input.seq = parsed_seq;
    }
  }

  if (const auto frame_it = kv.find("frame_id"); frame_it != kv.end() && !frame_it->second.empty()) {
    input.frame_id = frame_it->second;
  }

  if (const auto stamp_it = kv.find("publish_stamp_ms"); stamp_it != kv.end()) {
    uint64_t stamp = 0U;
    if (ParseUint64(stamp_it->second, &stamp)) {
      input.publish_stamp_ms = stamp;
    }
  }

  if (const auto stamp_it = kv.find("data_stamp_ms"); stamp_it != kv.end()) {
    uint64_t stamp = 0U;
    if (ParseUint64(stamp_it->second, &stamp)) {
      input.data_stamp_ms = stamp;
    }
  }

  if (const auto mode_it = kv.find("mode"); mode_it != kv.end()) {
    input.mode = ToLower(mode_it->second);
  }

  if (const auto valid_it = kv.find("is_valid"); valid_it != kv.end()) {
    bool parsed_valid = true;
    if (ParseBool(valid_it->second, &parsed_valid)) {
      input.is_valid = parsed_valid;
    }
  }

  if (const auto value_it = kv.find("center_x"); value_it != kv.end()) {
    double parsed = input.center_x;
    if (!ParseDouble(value_it->second, &parsed)) {
      input.is_valid = false;
      input.status_reason = "center_x parse failed";
    }
    input.center_x = parsed;
  }

  if (const auto value_it = kv.find("center_y"); value_it != kv.end()) {
    double parsed = input.center_y;
    if (!ParseDouble(value_it->second, &parsed)) {
      input.is_valid = false;
      input.status_reason = "center_y parse failed";
    }
    input.center_y = parsed;
  }

  if (const auto value_it = kv.find("heading"); value_it != kv.end()) {
    double parsed = input.heading;
    if (!ParseDouble(value_it->second, &parsed)) {
      input.is_valid = false;
      input.status_reason = "heading parse failed";
    }
    input.heading = parsed;
  }

  if (const auto value_it = kv.find("length"); value_it != kv.end()) {
    double parsed = input.length;
    if (!ParseDouble(value_it->second, &parsed)) {
      input.is_valid = false;
      input.status_reason = "length parse failed";
    }
    input.length = parsed;
  }

  if (const auto value_it = kv.find("width"); value_it != kv.end()) {
    double parsed = input.width;
    if (!ParseDouble(value_it->second, &parsed)) {
      input.is_valid = false;
      input.status_reason = "width parse failed";
    }
    input.width = parsed;
  }

  if (input.mode == "empty") {
    input.is_valid = false;
    input.status_reason = "publisher requested empty-slot scene";
  } else if (input.mode == "overflow") {
    input.is_valid = false;
    input.status_reason = "publisher requested overflow scene";
  } else if (input.mode == "nan") {
    input.center_x = std::numeric_limits<double>::quiet_NaN();
    input.is_valid = false;
    input.status_reason = "publisher requested nan scene";
  }

  if (!std::isfinite(input.center_x) || !std::isfinite(input.center_y) ||
      !std::isfinite(input.heading) || !std::isfinite(input.length) ||
      !std::isfinite(input.width)) {
    input.is_valid = false;
    input.status_reason = "input contains non-finite numeric field";
  }

  if (input.length <= 0.0 || input.width <= 0.0) {
    input.is_valid = false;
    input.status_reason = "slot geometry must be positive";
  }

  if (input.is_valid) {
    input.status_reason = "valid selected_slot sample";
  }

  return input;
}

std::string BuildTrajectoryJson(const valet_parking_config_t& config,
                                const SelectedSlotInput& input) {
  const bool is_estop = !input.is_valid;
  const double start_x = config.fake_vehicle_x;
  const double start_y = config.fake_vehicle_y;
  const double target_x = is_estop ? start_x : input.center_x;
  const double target_y = is_estop ? start_y : input.center_y;
  const double dx = target_x - start_x;
  const double dy = target_y - start_y;
  const double total_path_length = is_estop ? 0.0 : std::sqrt(dx * dx + dy * dy);
  const double total_path_time = is_estop ? 0.0 : 2.0;
  const int point_count = is_estop ? 1 : kTrajectoryPointCount;
  const double heading = (total_path_length > 1e-6) ? std::atan2(dy, dx) : config.fake_vehicle_theta;
  const double nominal_speed = (total_path_time > 1e-6)
      ? (total_path_length / total_path_time)
      : 0.0;

  const uint64_t publish_stamp_ms = NowMilliseconds();
  const uint64_t data_stamp_ms = (input.data_stamp_ms == 0U) ? publish_stamp_ms : input.data_stamp_ms;
  const int gear = is_estop ? 5 : 2;
  const int trajectory_type = is_estop ? 0 : 1;

  std::ostringstream out;
  out << std::fixed << std::setprecision(6);
  out << "{";
  out << "\"header\":{";
  out << "\"seq\":" << input.seq << ",";
  out << "\"frame_id\":\"" << JsonEscape(input.frame_id) << "\",";
  out << "\"publish_stamp_ms\":" << publish_stamp_ms << ",";
  out << "\"data_stamp_ms\":" << data_stamp_ms;
  out << "},";

  out << "\"total_path_length\":" << total_path_length << ",";
  out << "\"total_path_time\":" << total_path_time << ",";
  out << "\"trajectory_point\":[";

  for (int i = 0; i < point_count; ++i) {
    const double ratio = (point_count <= 1)
        ? 0.0
        : static_cast<double>(i) / static_cast<double>(point_count - 1);
    const double x = start_x + ratio * dx;
    const double y = start_y + ratio * dy;
    const double s = total_path_length * ratio;
    const double relative_time = total_path_time * ratio;

    if (i > 0) {
      out << ",";
    }

    out << "{";
    out << "\"path_point\":{";
    out << "\"x\":" << x << ",";
    out << "\"y\":" << y << ",";
    out << "\"z\":0.000000,";
    out << "\"theta\":" << heading << ",";
    out << "\"kappa\":0.000000,";
    out << "\"s\":" << s << ",";
    out << "\"l\":0.000000,";
    out << "\"dkappa\":0.000000,";
    out << "\"ddkappa\":0.000000,";
    out << "\"lane_id\":\"\",";
    out << "\"x_derivative\":" << std::cos(heading) << ",";
    out << "\"y_derivative\":" << std::sin(heading);
    out << "},";

    out << "\"v\":" << (is_estop ? 0.0 : nominal_speed) << ",";
    out << "\"a\":0.000000,";
    out << "\"relative_time\":" << relative_time << ",";
    out << "\"da\":0.000000,";
    out << "\"steer\":0.000000,";
    out << "\"gaussian_info\":{";
    out << "\"sigma_x\":0.300000,";
    out << "\"sigma_y\":0.250000,";
    out << "\"correlation\":0.000000,";
    out << "\"area_probability\":0.950000,";
    out << "\"ellipse_a\":0.500000,";
    out << "\"ellipse_b\":0.250000,";
    out << "\"theta_a\":" << heading;
    out << "}";
    out << "}";
  }

  out << "],";
  out << "\"is_replan\":false,";
  out << "\"replan_type\":0,";
  out << "\"replan_reason\":\"" << JsonEscape(input.status_reason) << "\",";
  out << "\"longitudinal_diff\":" << dx << ",";
  out << "\"lateral_diff\":" << dy << ",";
  out << "\"gear\":" << gear << ",";
  out << "\"estop\":{";
  out << "\"is_estop\":" << (is_estop ? "true" : "false") << ",";
  out << "\"reason\":\"" << JsonEscape(input.status_reason) << "\"";
  out << "},";
  out << "\"trajectory_type\":" << trajectory_type;
  out << "}";

  return out.str();
}

valet_parking_gear_position_t MapDdsGear(::GearPosition gear) {
  switch (gear) {
    case ::GearPosition::GEAR_NEUTRAL:
      return VALET_PARKING_GEAR_NEUTRAL;
    case ::GearPosition::GEAR_DRIVE:
      return VALET_PARKING_GEAR_DRIVE;
    case ::GearPosition::GEAR_REVERSE:
      return VALET_PARKING_GEAR_REVERSE;
    case ::GearPosition::GEAR_PARKING:
      return VALET_PARKING_GEAR_PARKING;
    case ::GearPosition::GEAR_LOW:
      return VALET_PARKING_GEAR_LOW;
    case ::GearPosition::GEAR_INVALID:
      return VALET_PARKING_GEAR_INVALID;
    case ::GearPosition::GEAR_NONE:
      return VALET_PARKING_GEAR_NONE;
  }
  return VALET_PARKING_GEAR_INVALID;
}

valet_parking_obstacle_type_t MapDdsObstacleType(::ObstacleType type) {
  switch (type) {
    case ::ObstacleType::OBSTACLE_TYPE_UNKNOWN:
      return VALET_PARKING_OBSTACLE_UNKNOWN;
    case ::ObstacleType::OBSTACLE_TYPE_UNKNOWN_MOVABLE:
      return VALET_PARKING_OBSTACLE_UNKNOWN_MOVABLE;
    case ::ObstacleType::OBSTACLE_TYPE_UNKNOWN_UNMOVABLE:
      return VALET_PARKING_OBSTACLE_UNKNOWN_UNMOVABLE;
    case ::ObstacleType::OBSTACLE_TYPE_PEDESTRIAN:
      return VALET_PARKING_OBSTACLE_PEDESTRIAN;
    case ::ObstacleType::OBSTACLE_TYPE_BICYCLE:
      return VALET_PARKING_OBSTACLE_BICYCLE;
    case ::ObstacleType::OBSTACLE_TYPE_VEHICLE:
      return VALET_PARKING_OBSTACLE_VEHICLE;
  }
  return VALET_PARKING_OBSTACLE_UNKNOWN;
}

std::string ParkingCommandModeToString(ParkingCommandMode mode) {
  switch (mode) {
    case ParkingCommandMode::PARKING_COMMAND_NONE:
      return "NONE";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_IN:
      return "PARKING_IN";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_LEFT:
      return "PARKING_OUT_LEFT";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_RIGHT:
      return "PARKING_OUT_RIGHT";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_FRONT:
      return "PARKING_OUT_FRONT";
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_BACK:
      return "PARKING_OUT_BACK";
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_FORWARD:
      return "DIRECT_FORWARD";
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_BACKWARD:
      return "DIRECT_BACKWARD";
    case ParkingCommandMode::PARKING_COMMAND_PAUSE:
      return "PAUSE";
    case ParkingCommandMode::PARKING_COMMAND_BRAKE:
      return "BRAKE";
    case ParkingCommandMode::PARKING_COMMAND_FINISH:
      return "FINISH";
  }
  return "UNKNOWN";
}

bool IsKnownParkingCommandMode(ParkingCommandMode mode) {
  switch (mode) {
    case ParkingCommandMode::PARKING_COMMAND_NONE:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_IN:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_LEFT:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_RIGHT:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_FRONT:
    case ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_BACK:
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_FORWARD:
    case ParkingCommandMode::PARKING_COMMAND_DIRECT_BACKWARD:
    case ParkingCommandMode::PARKING_COMMAND_PAUSE:
    case ParkingCommandMode::PARKING_COMMAND_BRAKE:
    case ParkingCommandMode::PARKING_COMMAND_FINISH:
      return true;
  }
  return false;
}

bool IsFiniteParkingCommandSample(const ParkingCommand& sample) {
  return std::isfinite(sample.direct_distance_m()) &&
         std::isfinite(sample.direct_speed_mps());
}

uint64_t SelectSampleStampMs(const Header& header) {
  if (header.data_stamp_ms() != 0U) {
    return header.data_stamp_ms();
  }
  if (header.publish_stamp_ms() != 0U) {
    return header.publish_stamp_ms();
  }
  return NowMilliseconds();
}

bool IsFiniteLocalizationSample(const LocalizationEstimate& sample) {
  return std::isfinite(sample.x()) && std::isfinite(sample.y()) &&
         std::isfinite(sample.z()) && std::isfinite(sample.heading());
}

bool IsFiniteChassisSample(const ChassisState& sample) {
  return std::isfinite(sample.speed_mps()) &&
         std::isfinite(sample.acceleration_mps2());
}

bool IsValidObstacleSample(const Obstacle& sample) {
  return std::isfinite(sample.center_x()) &&
         std::isfinite(sample.center_y()) &&
         std::isfinite(sample.heading()) &&
         std::isfinite(sample.length()) &&
         std::isfinite(sample.width()) &&
         std::isfinite(sample.velocity_x()) &&
         std::isfinite(sample.velocity_y()) && sample.length() > 0.0 &&
         sample.width() > 0.0;
}

const ParkingLot* SelectParkingLot(const SelectedSlot& sample) {
  const auto& lots = sample.parking_lots();
  if (lots.empty()) {
    return nullptr;
  }

  const uint32_t selected_seq = sample.opt_parking_seq();
  for (const auto& lot : lots) {
    if (lot.parking_seq() == selected_seq) {
      return &lot;
    }
  }

  return &lots.front();
}

SelectedSlotInput DecodeSelectedSlotInput(const SelectedSlot& sample) {
  SelectedSlotInput input;

  const Header& input_header = sample.header();
  input.seq = input_header.seq();
  input.frame_id = input_header.frame_id().empty()
      ? std::string("selected_slot")
      : input_header.frame_id();
  input.publish_stamp_ms = input_header.publish_stamp_ms();
  input.data_stamp_ms = input_header.data_stamp_ms();

  const uint64_t now_ms = NowMilliseconds();
  if (input.seq == 0U) {
    input.seq = now_ms;
  }
  if (input.publish_stamp_ms == 0U) {
    input.publish_stamp_ms = now_ms;
  }
  if (input.data_stamp_ms == 0U) {
    input.data_stamp_ms = input.publish_stamp_ms;
  }

  input.is_valid = sample.is_valid();
  if (!sample.is_valid()) {
    input.status_reason = "selected_slot.is_valid is false";
  }

  const auto& lots = sample.parking_lots();
  if (sample.count() == 0U || lots.empty()) {
    input.is_valid = false;
    input.status_reason = "selected_slot has no parking_lots";
    return input;
  }

  if (static_cast<std::size_t>(sample.count()) > lots.size()) {
    input.is_valid = false;
    input.status_reason = "selected_slot count exceeds parking_lots size";
  }

  const ParkingLot* selected_lot = SelectParkingLot(sample);
  if (selected_lot == nullptr) {
    input.is_valid = false;
    input.status_reason = "selected_slot selected lot is unavailable";
    return input;
  }

  input.heading = selected_lot->hmi_angle();
  input.length = selected_lot->hmi_depth();
  input.width = selected_lot->hmi_width();

  const std::vector<PsPoint>* slot_points = &selected_lot->pts_enu();
  if (slot_points->empty()) {
    slot_points = &selected_lot->pts_vrf();
  }

  if (slot_points->size() < 4U) {
    input.is_valid = false;
    input.status_reason = "selected_slot selected lot has insufficient corner points";
    return input;
  }

  double sum_x = 0.0;
  double sum_y = 0.0;
  for (const auto& corner : *slot_points) {
    sum_x += corner.point().x();
    sum_y += corner.point().y();
  }

  const double point_count = static_cast<double>(slot_points->size());
  input.center_x = sum_x / point_count;
  input.center_y = sum_y / point_count;

  if (!std::isfinite(input.center_x) || !std::isfinite(input.center_y) ||
      !std::isfinite(input.heading) || !std::isfinite(input.length) ||
      !std::isfinite(input.width)) {
    input.is_valid = false;
    input.status_reason = "input contains non-finite numeric field";
  }

  if (input.length <= 0.0 || input.width <= 0.0) {
    input.is_valid = false;
    input.status_reason = "slot geometry must be positive";
  }

  if (input.is_valid) {
    input.status_reason = "valid selected_slot sample";
  }

  return input;
}

Header MakeHeader(uint64_t seq,
                  const std::string& frame_id,
                  uint64_t publish_stamp_ms,
                  uint64_t data_stamp_ms) {
  Header output_header;
  output_header.seq(seq);
  output_header.frame_id(frame_id);
  output_header.publish_stamp_ms(publish_stamp_ms);
  output_header.data_stamp_ms(data_stamp_ms);
  return output_header;
}

PlanningTrajectory BuildPlanningTrajectory(const valet_parking_config_t& config,
                                           const SelectedSlotInput& input) {
  const bool is_estop = !input.is_valid;
  const double start_x = config.fake_vehicle_x;
  const double start_y = config.fake_vehicle_y;
  const double target_x = is_estop ? start_x : input.center_x;
  const double target_y = is_estop ? start_y : input.center_y;
  const double dx = target_x - start_x;
  const double dy = target_y - start_y;
  const double total_path_length = is_estop ? 0.0 : std::sqrt(dx * dx + dy * dy);
  const double total_path_time = is_estop ? 0.0 : 2.0;
  const int point_count = is_estop ? 1 : kTrajectoryPointCount;
  const double heading = (total_path_length > 1e-6) ? std::atan2(dy, dx) : config.fake_vehicle_theta;
  const double nominal_speed = (total_path_time > 1e-6)
      ? (total_path_length / total_path_time)
      : 0.0;

  const uint64_t publish_stamp_ms = NowMilliseconds();
  const uint64_t data_stamp_ms = (input.data_stamp_ms == 0U) ? publish_stamp_ms : input.data_stamp_ms;

  std::vector<TrajectoryPoint> points;
  points.reserve(static_cast<std::size_t>(point_count));

  for (int i = 0; i < point_count; ++i) {
    const double ratio = (point_count <= 1)
        ? 0.0
        : static_cast<double>(i) / static_cast<double>(point_count - 1);
    const double x = start_x + ratio * dx;
    const double y = start_y + ratio * dy;
    const double s = total_path_length * ratio;
    const double relative_time = total_path_time * ratio;

    PathPoint path_point;
    path_point.x(x);
    path_point.y(y);
    path_point.z(0.0);
    path_point.theta(heading);
    path_point.kappa(0.0);
    path_point.s(s);
    path_point.l(0.0);
    path_point.dkappa(0.0);
    path_point.ddkappa(0.0);
    path_point.lane_id("valet_parking_lane_0");
    path_point.x_derivative(std::cos(heading));
    path_point.y_derivative(std::sin(heading));

    GaussianInfo gaussian_info;
    gaussian_info.sigma_x(0.3);
    gaussian_info.sigma_y(0.25);
    gaussian_info.correlation(0.0);
    gaussian_info.area_probability(0.95);
    gaussian_info.ellipse_a(0.5);
    gaussian_info.ellipse_b(0.25);
    gaussian_info.theta_a(heading);

    TrajectoryPoint trajectory_point;
    trajectory_point.path_point(path_point);
    trajectory_point.v(is_estop ? 0.0 : nominal_speed);
    trajectory_point.a(0.0);
    trajectory_point.relative_time(relative_time);
    trajectory_point.da(0.0);
    trajectory_point.steer(0.0);
    trajectory_point.gaussian_info(gaussian_info);
    points.push_back(trajectory_point);
  }

  // Phase 2: waypoint output protection — truncate if exceeding configured limit
  if (points.size() > kMaxOutputWaypointCount) {
    std::cerr << kModuleTag
              << " WARNING: trajectory point count " << points.size()
              << " exceeds max " << kMaxOutputWaypointCount
              << ", truncating" << std::endl;
    points.resize(kMaxOutputWaypointCount);
  }

  EStop estop_sample;
  estop_sample.is_estop(is_estop);
  estop_sample.reason(input.status_reason);

  PlanningTrajectory output;
  output.header(MakeHeader(input.seq, input.frame_id, publish_stamp_ms, data_stamp_ms));
  output.total_path_length(total_path_length);
  output.total_path_time(total_path_time);
  output.trajectory_point(points);
  output.is_replan(false);
  output.replan_type(0U);
  output.replan_reason(input.status_reason);
  output.longitudinal_diff(dx);
  output.lateral_diff(dy);
  output.gear(is_estop ? GearPosition::GEAR_INVALID : GearPosition::GEAR_REVERSE);
  output.estop(estop_sample);
  output.trajectory_type(is_estop ? PlanningTrajectoryType::TRAJECTORY_TYPE_UNKNOWN
                                  : PlanningTrajectoryType::TRAJECTORY_TYPE_NORMAL);
  return output;
}

// ============================================================
// Phase 2: PrkVinBus coordinate transform utilities
// Enabled via --features=ENABLE_PRKVINBUS
// ============================================================
#if ENABLE_PRKVINBUS

namespace {

double NormalizeAngle(double angle) {
  constexpr double kPi = 3.14159265358979323846;
  constexpr double kTwoPi = 2.0 * kPi;
  while (angle >= kPi) {
    angle -= kTwoPi;
  }
  while (angle < -kPi) {
    angle += kTwoPi;
  }
  return angle;
}

void ConvertKmPoseToPlannerFrame(double km_x,
                                 double km_y,
                                 double km_heading,
                                 double* planner_x,
                                 double* planner_y,
                                 double* planner_heading) {
  if (planner_x == nullptr || planner_y == nullptr || planner_heading == nullptr) {
    return;
  }
  // KM frame: +x backward, +y right. Planner internal: +x forward, +y left.
  constexpr double kPi = 3.14159265358979323846;
  *planner_x = -km_x;
  *planner_y = -km_y;
  *planner_heading = NormalizeAngle(km_heading + kPi);
}

// VALin Mapping-pf normalized gear: 0=P, 1=R, 2=N, 3=D, 4=GEAR_NOSIGNAL.
valet_parking_gear_position_t MapPrkVinGearStatus(uint32_t gear_status) {
  switch (gear_status) {
    case 0U:
      return VALET_PARKING_GEAR_PARKING;
    case 1U:
      return VALET_PARKING_GEAR_REVERSE;
    case 2U:
      return VALET_PARKING_GEAR_NEUTRAL;
    case 3U:
      return VALET_PARKING_GEAR_DRIVE;
    case 4U:
      return VALET_PARKING_GEAR_NONE;
    default:
      return VALET_PARKING_GEAR_INVALID;
  }
}

// VALin Mapping-pf normalized m_vehicleDrvDir: 1=forward, 2=backward, 15=unknown.
double SignedPrkVinVelocity(double absolute_speed,
                            uint32_t vehicle_drive_direction,
                            valet_parking_gear_position_t gear) {
  const double magnitude = std::fabs(absolute_speed);
  if (vehicle_drive_direction == 1U) {
    return magnitude;
  }
  if (vehicle_drive_direction == 2U) {
    return -magnitude;
  }
  // Conservative fallback: use normalized gear position.
  switch (gear) {
    case VALET_PARKING_GEAR_DRIVE:
      return magnitude;
    case VALET_PARKING_GEAR_REVERSE:
      return -magnitude;
    case VALET_PARKING_GEAR_NEUTRAL:
    case VALET_PARKING_GEAR_PARKING:
    case VALET_PARKING_GEAR_LOW:
    case VALET_PARKING_GEAR_INVALID:
    case VALET_PARKING_GEAR_NONE:
    default:
      return 0.0;
  }
}

}  // namespace

#endif  // ENABLE_PRKVINBUS

}  // namespace

ValetParkingComponent::ValetParkingComponent(const valet_parking_config_t& config)
    : config_(config),
      stage_parking_adapter_(config_),
      input_topic_name_(config.input_topic_name != nullptr ? config.input_topic_name
                                                            : kSelectedSlotTopicDefault),
      output_topic_name_(config.output_topic_name != nullptr ? config.output_topic_name
                                                              : kPlanningTrajectoryTopicDefault),
      command_topic_name_(config.command_topic_name != nullptr
                              ? config.command_topic_name
                              : kParkingCommandTopicDefault),
      localization_topic_name_(config.localization_topic_name != nullptr
                                   ? config.localization_topic_name
                                   : kLocalizationTopicDefault),
      chassis_topic_name_(config.chassis_topic_name != nullptr
                              ? config.chassis_topic_name
                              : kChassisTopicDefault),
      obstacle_topic_name_(config.obstacle_topic_name != nullptr
                               ? config.obstacle_topic_name
                               : kObstacleTopicDefault),
      prk_vin_bus_topic_name_(config.prk_vin_bus_topic_name != nullptr
                                  ? config.prk_vin_bus_topic_name
                                  : kPrkVinBusTopicDefault),
      command_topic_enabled_(config.enable_command_topic != 0U),
      aux_input_topics_enabled_(config.enable_aux_input_topics != 0U) {
  if (input_topic_name_.empty()) {
    input_topic_name_ = kSelectedSlotTopicDefault;
  }
  if (output_topic_name_.empty()) {
    output_topic_name_ = kPlanningTrajectoryTopicDefault;
  }
  if (command_topic_name_.empty()) {
    command_topic_name_ = kParkingCommandTopicDefault;
  }
  if (localization_topic_name_.empty()) {
    localization_topic_name_ = kLocalizationTopicDefault;
  }
  if (chassis_topic_name_.empty()) {
    chassis_topic_name_ = kChassisTopicDefault;
  }
  if (obstacle_topic_name_.empty()) {
    obstacle_topic_name_ = kObstacleTopicDefault;
  }
  if (prk_vin_bus_topic_name_.empty()) {
    prk_vin_bus_topic_name_ = kPrkVinBusTopicDefault;
  }
  aux_vehicle_input_.state.is_valid = 1;
  aux_vehicle_input_.state.gear = VALET_PARKING_GEAR_PARKING;
}

ValetParkingComponent::~ValetParkingComponent() {
  (void)Stop();
}

void ValetParkingComponent::SetLastError(const std::string& message) {
  std::lock_guard<std::mutex> lock(mutex_);
  last_error_ = message;
}

std::string ValetParkingComponent::ReturnCodeToString(magna::dds::ReturnCode_t rc) {
  using RC = magna::dds::ReturnCode_t;
  switch (rc) {
    case RC::RETCODE_OK:
      return "RETCODE_OK";
    case RC::RETCODE_ERROR:
      return "RETCODE_ERROR";
    case RC::RETCODE_UNSUPPORTED:
      return "RETCODE_UNSUPPORTED";
    case RC::RETCODE_BAD_PARAMETER:
      return "RETCODE_BAD_PARAMETER";
    case RC::RETCODE_PRECONDITION_NOT_MET:
      return "RETCODE_PRECONDITION_NOT_MET";
    case RC::RETCODE_OUT_OF_RESOURCES:
      return "RETCODE_OUT_OF_RESOURCES";
    case RC::RETCODE_NOT_ENABLED:
      return "RETCODE_NOT_ENABLED";
    case RC::RETCODE_IMMUTABLE_POLICY:
      return "RETCODE_IMMUTABLE_POLICY";
    case RC::RETCODE_INCONSISTENT_POLICY:
      return "RETCODE_INCONSISTENT_POLICY";
    case RC::RETCODE_ALREADY_DELETED:
      return "RETCODE_ALREADY_DELETED";
    case RC::RETCODE_TIMEOUT:
      return "RETCODE_TIMEOUT";
    case RC::RETCODE_NO_DATA:
      return "RETCODE_NO_DATA";
    case RC::RETCODE_ILLEGAL_OPERATION:
      return "RETCODE_ILLEGAL_OPERATION";
    default:
      return "RETCODE_UNKNOWN";
  }
}

bool ValetParkingComponent::InitDds() {
  dds_factory_ = magna::dds::DomainParticipantFactory::get_instance();
  if (dds_factory_ == nullptr) {
    SetLastError("failed to get DomainParticipantFactory instance");
    return false;
  }

  participant_ = dds_factory_->create_participant(static_cast<magna::dds::DomainId_t>(config_.domain_id));
  if (participant_ == nullptr) {
    SetLastError("failed to create DomainParticipant");
    CleanupDds();
    return false;
  }

  selected_slot_topic_type_ = std::make_unique<SelectedSlotTopicDataType>();
  magna::dds::ReturnCode_t rc = participant_->register_type(selected_slot_topic_type_.get());
  if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
    SetLastError(std::string("register selected_slot type failed: ") + ReturnCodeToString(rc));
    CleanupDds();
    return false;
  }

  planning_trajectory_topic_type_ = std::make_unique<PlanningTrajectoryTopicDataType>();
  rc = participant_->register_type(planning_trajectory_topic_type_.get());
  if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
    SetLastError(std::string("register planning_trajectory type failed: ") + ReturnCodeToString(rc));
    CleanupDds();
    return false;
  }

  if (command_topic_enabled_) {
    command_topic_type_ = std::make_unique<ParkingCommandTopicDataType>();
    rc = participant_->register_type(command_topic_type_.get());
    if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
      SetLastError(std::string("register parking_command type failed: ") +
                   ReturnCodeToString(rc));
      CleanupDds();
      return false;
    }
  }

  if (aux_input_topics_enabled_) {
    localization_topic_type_ = std::make_unique<LocalizationEstimateTopicDataType>();
    rc = participant_->register_type(localization_topic_type_.get());
    if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
      SetLastError(std::string("register localization type failed: ") + ReturnCodeToString(rc));
      CleanupDds();
      return false;
    }

    chassis_topic_type_ = std::make_unique<ChassisStateTopicDataType>();
    rc = participant_->register_type(chassis_topic_type_.get());
    if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
      SetLastError(std::string("register chassis type failed: ") + ReturnCodeToString(rc));
      CleanupDds();
      return false;
    }

    obstacle_topic_type_ = std::make_unique<ObstacleArrayTopicDataType>();
    rc = participant_->register_type(obstacle_topic_type_.get());
    if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
      SetLastError(std::string("register obstacle array type failed: ") + ReturnCodeToString(rc));
      CleanupDds();
      return false;
    }
  }

#if ENABLE_PRKVINBUS
  prk_vin_bus_topic_enabled_ = true;
  prk_vin_bus_topic_type_ = std::make_unique<PrkVinBusTopicDataType>();
  rc = participant_->register_type(prk_vin_bus_topic_type_.get());
  if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
    SetLastError(std::string("register PrkVinBus type failed: ") + ReturnCodeToString(rc));
    CleanupDds();
    return false;
  }
#endif

  input_topic_ = participant_->create_topic(input_topic_name_, selected_slot_topic_type_->get_name(),
                                            magna::dds::TOPIC_QOS_DEFAULT);
  if (input_topic_ == nullptr) {
    SetLastError("failed to create input topic: " + input_topic_name_);
    CleanupDds();
    return false;
  }

  output_topic_ = participant_->create_topic(output_topic_name_, planning_trajectory_topic_type_->get_name(),
                                             magna::dds::TOPIC_QOS_DEFAULT);
  if (output_topic_ == nullptr) {
    SetLastError("failed to create output topic: " + output_topic_name_);
    CleanupDds();
    return false;
  }

  if (command_topic_enabled_) {
    command_topic_ = participant_->create_topic(
        command_topic_name_, command_topic_type_->get_name(),
        magna::dds::TOPIC_QOS_DEFAULT);
    if (command_topic_ == nullptr) {
      SetLastError("failed to create parking command topic: " +
                   command_topic_name_);
      CleanupDds();
      return false;
    }
  }

  if (aux_input_topics_enabled_) {
    localization_topic_ = participant_->create_topic(
        localization_topic_name_, localization_topic_type_->get_name(),
        magna::dds::TOPIC_QOS_DEFAULT);
    if (localization_topic_ == nullptr) {
      SetLastError("failed to create localization topic: " +
                   localization_topic_name_);
      CleanupDds();
      return false;
    }

    chassis_topic_ = participant_->create_topic(
        chassis_topic_name_, chassis_topic_type_->get_name(),
        magna::dds::TOPIC_QOS_DEFAULT);
    if (chassis_topic_ == nullptr) {
      SetLastError("failed to create chassis topic: " + chassis_topic_name_);
      CleanupDds();
      return false;
    }

    obstacle_topic_ = participant_->create_topic(
        obstacle_topic_name_, obstacle_topic_type_->get_name(),
        magna::dds::TOPIC_QOS_DEFAULT);
    if (obstacle_topic_ == nullptr) {
      SetLastError("failed to create obstacle topic: " + obstacle_topic_name_);
      CleanupDds();
      return false;
    }
  }

#if ENABLE_PRKVINBUS
  prk_vin_bus_topic_ = participant_->create_topic(
      prk_vin_bus_topic_name_, prk_vin_bus_topic_type_->get_name(),
      magna::dds::TOPIC_QOS_DEFAULT);
  if (prk_vin_bus_topic_ == nullptr) {
    SetLastError("failed to create PrkVinBus topic: " + prk_vin_bus_topic_name_);
    CleanupDds();
    return false;
  }
#endif

  subscriber_ = participant_->create_subscriber(magna::dds::SUBSCRIBER_QOS_DEFAULT);
  if (subscriber_ == nullptr) {
    SetLastError("failed to create subscriber");
    CleanupDds();
    return false;
  }

  publisher_ = participant_->create_publisher(magna::dds::PUBLISHER_QOS_DEFAULT);
  if (publisher_ == nullptr) {
    SetLastError("failed to create publisher");
    CleanupDds();
    return false;
  }

  const int32_t depth = static_cast<int32_t>(std::max<uint32_t>(1U, config_.qos_depth));

  magna::dds::DataReaderQos reader_qos = magna::dds::DATAREADER_QOS_DEFAULT;
  reader_qos.history.kind = magna::dds::HistoryQosPolicyKind::KEEP_LAST_HISTORY_QOS;
  reader_qos.history.depth = depth;
  reader_qos.resource_limits.max_samples = depth;
  reader_qos.resource_limits.max_samples_per_instance = depth;

  input_reader_ = subscriber_->create_datareader(input_topic_, reader_qos, nullptr,
                                                 magna::dds::STATUS_MASK_NONE);
  if (input_reader_ == nullptr) {
    SetLastError("failed to create datareader for input topic");
    CleanupDds();
    return false;
  }

  if (command_topic_enabled_) {
    command_reader_ = subscriber_->create_datareader(
        command_topic_, reader_qos, nullptr, magna::dds::STATUS_MASK_NONE);
    if (command_reader_ == nullptr) {
      SetLastError("failed to create datareader for parking command topic");
      CleanupDds();
      return false;
    }
  }

  if (aux_input_topics_enabled_) {
    localization_reader_ = subscriber_->create_datareader(
        localization_topic_, reader_qos, nullptr, magna::dds::STATUS_MASK_NONE);
    if (localization_reader_ == nullptr) {
      SetLastError("failed to create datareader for localization topic");
      CleanupDds();
      return false;
    }

    chassis_reader_ = subscriber_->create_datareader(
        chassis_topic_, reader_qos, nullptr, magna::dds::STATUS_MASK_NONE);
    if (chassis_reader_ == nullptr) {
      SetLastError("failed to create datareader for chassis topic");
      CleanupDds();
      return false;
    }

    obstacle_reader_ = subscriber_->create_datareader(
        obstacle_topic_, reader_qos, nullptr, magna::dds::STATUS_MASK_NONE);
    if (obstacle_reader_ == nullptr) {
      SetLastError("failed to create datareader for obstacle topic");
      CleanupDds();
      return false;
    }
  }

#if ENABLE_PRKVINBUS
  prk_vin_bus_reader_ = subscriber_->create_datareader(
      prk_vin_bus_topic_, reader_qos, nullptr, magna::dds::STATUS_MASK_NONE);
  if (prk_vin_bus_reader_ == nullptr) {
    SetLastError("failed to create datareader for PrkVinBus topic");
    CleanupDds();
    return false;
  }
#endif

  magna::dds::DataWriterQos writer_qos = magna::dds::DATAWRITER_QOS_DEFAULT;
  writer_qos.history.kind = magna::dds::HistoryQosPolicyKind::KEEP_LAST_HISTORY_QOS;
  writer_qos.history.depth = depth;
  writer_qos.resource_limits.max_samples = depth;
  writer_qos.resource_limits.max_samples_per_instance = depth;

  output_writer_ = publisher_->create_datawriter(output_topic_, writer_qos, nullptr,
                                                 magna::dds::STATUS_MASK_NONE);
  if (output_writer_ == nullptr) {
    SetLastError("failed to create datawriter for output topic");
    CleanupDds();
    return false;
  }

  return true;
}

void ValetParkingComponent::CleanupDds() noexcept {
  output_writer_ = nullptr;
  prk_vin_bus_reader_ = nullptr;
  obstacle_reader_ = nullptr;
  chassis_reader_ = nullptr;
  localization_reader_ = nullptr;
  command_reader_ = nullptr;
  input_reader_ = nullptr;
  publisher_ = nullptr;
  subscriber_ = nullptr;
  obstacle_topic_ = nullptr;
  prk_vin_bus_topic_ = nullptr;
  chassis_topic_ = nullptr;
  localization_topic_ = nullptr;
  command_topic_ = nullptr;
  output_topic_ = nullptr;
  input_topic_ = nullptr;

  if (participant_ != nullptr) {
    (void)participant_->delete_contained_entities();
    if (dds_factory_ != nullptr) {
      (void)dds_factory_->delete_participant(participant_);
    }
  }

  participant_ = nullptr;
  dds_factory_ = nullptr;
  obstacle_topic_type_.reset();
  prk_vin_bus_topic_type_.reset();
  chassis_topic_type_.reset();
  localization_topic_type_.reset();
  command_topic_type_.reset();
  planning_trajectory_topic_type_.reset();
  selected_slot_topic_type_.reset();
}

bool ValetParkingComponent::HandleCommandSample() {
  if (!command_topic_enabled_ || command_reader_ == nullptr) {
    return false;
  }

  ParkingCommand sample;
  magna::dds::SampleInfo sample_info;
  const magna::dds::ReturnCode_t read_rc =
      command_reader_->take_next_sample(&sample, sample_info);
  if (read_rc == magna::dds::ReturnCode_t::RETCODE_NO_DATA) {
    return false;
  }
  if (read_rc != magna::dds::ReturnCode_t::RETCODE_OK) {
    SetLastError(std::string("take parking command sample failed: ") +
                 ReturnCodeToString(read_rc));
    return false;
  }
  if (!sample_info.valid_data) {
    return true;
  }

  ++handled_command_samples_;
  const std::string mode_text = ParkingCommandModeToString(sample.mode());
  if (!sample.is_valid() ||
      sample.mode() == ParkingCommandMode::PARKING_COMMAND_NONE) {
    latest_parking_command_.reset();
    std::cout << kModuleTag
              << " command #" << handled_command_samples_
              << " mode=" << mode_text
              << " (cleared_command)" << std::endl;
    return true;
  }

  if (!IsKnownParkingCommandMode(sample.mode()) ||
      !IsFiniteParkingCommandSample(sample)) {
    latest_parking_command_.reset();
    SetLastError("rejected invalid parking command sample");
    std::cout << kModuleTag
              << " command rejected #" << handled_command_samples_
              << " mode=" << mode_text
              << " (cleared_command)" << std::endl;
    return true;
  }

  latest_parking_command_ = std::make_unique<ParkingCommand>(sample);
  std::cout << kModuleTag
            << " command #" << handled_command_samples_
            << " mode=" << mode_text
            << " parking_seq=" << sample.parking_seq()
            << " direct_distance_m=" << sample.direct_distance_m()
            << " direct_speed_mps=" << sample.direct_speed_mps()
            << " reset_history="
            << (sample.reset_history() ? "true" : "false")
            << " reason=\"" << sample.reason() << "\""
            << std::endl;
  return true;
}

bool ValetParkingComponent::DrainCommandSamples() {
  if (!command_topic_enabled_) {
    return false;
  }

  bool handled_any = false;
  for (int i = 0; i < kMaxCommandDrainRoundsPerCycle && running_.load(); ++i) {
    const bool handled_one = HandleCommandSample();
    handled_any = handled_one || handled_any;
    if (!handled_one) {
      break;
    }
  }

  return handled_any;
}

void ValetParkingComponent::ApplyAuxVehicleInput() {
  // Chassis data only supplements speed/accel/gear; localization gates pose.
  if (!aux_vehicle_input_.has_localization) {
    (void)stage_parking_adapter_.ClearVehicleState();
    return;
  }

  aux_vehicle_input_.state.is_valid = 1;
  if (aux_vehicle_input_.state.stamp_ms == 0U) {
    aux_vehicle_input_.state.stamp_ms = NowMilliseconds();
  }
  const int ret =
      stage_parking_adapter_.UpdateVehicleState(aux_vehicle_input_.state);
  if (ret != VALET_PARKING_OK) {
    SetLastError("failed to apply auxiliary vehicle state");
  }
}

bool ValetParkingComponent::HandleLocalizationSample() {
  if (localization_reader_ == nullptr) {
    return false;
  }

  LocalizationEstimate sample;
  magna::dds::SampleInfo sample_info;
  const magna::dds::ReturnCode_t read_rc =
      localization_reader_->take_next_sample(&sample, sample_info);
  if (read_rc == magna::dds::ReturnCode_t::RETCODE_NO_DATA) {
    return false;
  }
  if (read_rc != magna::dds::ReturnCode_t::RETCODE_OK) {
    SetLastError(std::string("take localization sample failed: ") +
                 ReturnCodeToString(read_rc));
    return false;
  }
  if (!sample_info.valid_data) {
    return true;
  }

  ++handled_localization_samples_;
  if (!sample.is_valid() || !IsFiniteLocalizationSample(sample)) {
    aux_vehicle_input_.has_localization = false;
    ApplyAuxVehicleInput();
    std::cout << kModuleTag
              << " aux localization invalid #"
              << handled_localization_samples_
              << " (cleared_vehicle_state)" << std::endl;
    return true;
  }

  aux_vehicle_input_.has_localization = true;
  aux_vehicle_input_.state.stamp_ms = SelectSampleStampMs(sample.header());
  aux_vehicle_input_.state.x = sample.x();
  aux_vehicle_input_.state.y = sample.y();
  aux_vehicle_input_.state.z = sample.z();
  aux_vehicle_input_.state.heading = sample.heading();
  ApplyAuxVehicleInput();
  std::cout << kModuleTag
            << " aux localization #" << handled_localization_samples_
            << " (x=" << sample.x()
            << ", y=" << sample.y()
            << ", heading=" << sample.heading() << ")"
            << std::endl;
  return true;
}

bool ValetParkingComponent::HandleChassisSample() {
  if (chassis_reader_ == nullptr) {
    return false;
  }

  ChassisState sample;
  magna::dds::SampleInfo sample_info;
  const magna::dds::ReturnCode_t read_rc =
      chassis_reader_->take_next_sample(&sample, sample_info);
  if (read_rc == magna::dds::ReturnCode_t::RETCODE_NO_DATA) {
    return false;
  }
  if (read_rc != magna::dds::ReturnCode_t::RETCODE_OK) {
    SetLastError(std::string("take chassis sample failed: ") +
                 ReturnCodeToString(read_rc));
    return false;
  }
  if (!sample_info.valid_data) {
    return true;
  }

  ++handled_chassis_samples_;
  if (!sample.is_valid() || !IsFiniteChassisSample(sample)) {
    aux_vehicle_input_.has_chassis = false;
    aux_vehicle_input_.state.linear_velocity = 0.0;
    aux_vehicle_input_.state.linear_acceleration = 0.0;
    aux_vehicle_input_.state.gear = VALET_PARKING_GEAR_PARKING;
    ApplyAuxVehicleInput();
    std::cout << kModuleTag
              << " aux chassis invalid #" << handled_chassis_samples_
              << " (cleared_chassis_state)" << std::endl;
    return true;
  }

  aux_vehicle_input_.has_chassis = true;
  if (!aux_vehicle_input_.has_localization) {
    aux_vehicle_input_.state.stamp_ms = SelectSampleStampMs(sample.header());
  }
  aux_vehicle_input_.state.linear_velocity = sample.speed_mps();
  aux_vehicle_input_.state.linear_acceleration =
      sample.acceleration_mps2();
  aux_vehicle_input_.state.gear = MapDdsGear(sample.gear());
  ApplyAuxVehicleInput();
  std::cout << kModuleTag
            << " aux chassis #" << handled_chassis_samples_
            << " (speed_mps=" << sample.speed_mps()
            << ", acceleration_mps2=" << sample.acceleration_mps2()
            << ", gear=" << static_cast<int>(sample.gear()) << ")"
            << std::endl;
  return true;
}

bool ValetParkingComponent::HandleObstacleSample() {
  if (obstacle_reader_ == nullptr) {
    return false;
  }

  ObstacleArray sample;
  magna::dds::SampleInfo sample_info;
  const magna::dds::ReturnCode_t read_rc =
      obstacle_reader_->take_next_sample(&sample, sample_info);
  if (read_rc == magna::dds::ReturnCode_t::RETCODE_NO_DATA) {
    return false;
  }
  if (read_rc != magna::dds::ReturnCode_t::RETCODE_OK) {
    SetLastError(std::string("take obstacle sample failed: ") +
                 ReturnCodeToString(read_rc));
    return false;
  }
  if (!sample_info.valid_data) {
    return true;
  }

  ++handled_obstacle_samples_;
  if (!sample.is_valid()) {
    (void)stage_parking_adapter_.ClearObstacles();
    std::cout << kModuleTag
              << " aux obstacles invalid #" << handled_obstacle_samples_
              << " (cleared_obstacles)" << std::endl;
    return true;
  }

  std::vector<valet_parking_obstacle_t> obstacles;
  obstacles.reserve(sample.obstacles().size());
  for (const Obstacle& source : sample.obstacles()) {
    if (!IsValidObstacleSample(source)) {
      (void)stage_parking_adapter_.ClearObstacles();
      SetLastError("rejected invalid auxiliary obstacle sample");
      std::cout << kModuleTag
                << " aux obstacles rejected #" << handled_obstacle_samples_
                << " (cleared_obstacles)" << std::endl;
      return true;
    }
    valet_parking_obstacle_t obstacle{};
    obstacle.id = source.id();
    obstacle.type = MapDdsObstacleType(source.type());
    obstacle.is_dynamic = source.is_dynamic() ? 1 : 0;
    obstacle.center_x = source.center_x();
    obstacle.center_y = source.center_y();
    obstacle.heading = source.heading();
    obstacle.length = source.length();
    obstacle.width = source.width();
    obstacle.velocity_x = source.velocity_x();
    obstacle.velocity_y = source.velocity_y();
    obstacles.push_back(obstacle);
  }

  const int ret = stage_parking_adapter_.UpdateObstacles(
      obstacles.empty() ? nullptr : obstacles.data(),
      static_cast<uint32_t>(obstacles.size()));
  if (ret != VALET_PARKING_OK) {
    (void)stage_parking_adapter_.ClearObstacles();
    SetLastError("failed to apply auxiliary obstacle array");
    std::cout << kModuleTag
              << " aux obstacles rejected #" << handled_obstacle_samples_
              << " (cleared_obstacles)" << std::endl;
    return true;
  }
  std::cout << kModuleTag
            << " aux obstacles #" << handled_obstacle_samples_
            << " (count=" << obstacles.size() << ")"
            << std::endl;
  return true;
}

#if ENABLE_PRKVINBUS
bool ValetParkingComponent::HandlePrkVinBusSample() {
  if (prk_vin_bus_reader_ == nullptr) {
    return false;
  }

  PrkVinBus sample;
  magna::dds::SampleInfo sample_info;
  const magna::dds::ReturnCode_t read_rc =
      prk_vin_bus_reader_->take_next_sample(&sample, sample_info);
  if (read_rc == magna::dds::ReturnCode_t::RETCODE_NO_DATA) {
    return false;
  }
  if (read_rc != magna::dds::ReturnCode_t::RETCODE_OK) {
    SetLastError(std::string("take PrkVinBus sample failed: ") +
                 ReturnCodeToString(read_rc));
    return false;
  }
  if (!sample_info.valid_data) {
    return true;
  }

  ++handled_prk_vin_samples_;
  std::cout << kModuleTag
            << " PrkVinBus sample #" << handled_prk_vin_samples_
            << " received (stub — full processing TBD)" << std::endl;
  return true;
}
#endif

bool ValetParkingComponent::DrainAuxInputSamples() {
  if (!aux_input_topics_enabled_) {
    return false;
  }

  bool handled_any = false;
  for (int i = 0; i < kMaxAuxDrainRoundsPerCycle && running_.load(); ++i) {
    bool handled_one = false;
    handled_one = HandleLocalizationSample() || handled_one;
    handled_one = HandleChassisSample() || handled_one;
    handled_one = HandleObstacleSample() || handled_one;
    handled_any = handled_one || handled_any;
    if (!handled_one) {
      break;
    }
  }

  return handled_any;
}

bool ValetParkingComponent::BuildTrajectoryFromInput(const SelectedSlot& input_sample,
                                                     PlanningTrajectory* output_sample,
                                                     std::string* status_reason) {
  if (output_sample == nullptr || status_reason == nullptr) {
    return false;
  }

  const bool ok = stage_parking_adapter_.Process(
      input_sample, latest_parking_command_.get(), output_sample,
      status_reason);
  if (latest_parking_command_ != nullptr &&
      latest_parking_command_->reset_history()) {
    std::cout << kModuleTag
              << " command reset_history consumed mode="
              << ParkingCommandModeToString(latest_parking_command_->mode())
              << " (one_shot)" << std::endl;
    latest_parking_command_->reset_history(false);
  }
  return ok;
}

bool ValetParkingComponent::HandleOneSample() {
  if (input_reader_ == nullptr || output_writer_ == nullptr) {
    return false;
  }

  SelectedSlot input_sample;
  magna::dds::SampleInfo sample_info;
  const magna::dds::ReturnCode_t read_rc =
      input_reader_->take_next_sample(&input_sample, sample_info);

  if (read_rc == magna::dds::ReturnCode_t::RETCODE_NO_DATA) {
    return false;
  }

  if (read_rc != magna::dds::ReturnCode_t::RETCODE_OK) {
    SetLastError(std::string("take_next_sample failed: ") + ReturnCodeToString(read_rc));
    return false;
  }

  if (!sample_info.valid_data) {
    return true;
  }

  PlanningTrajectory output_sample;
  std::string status_reason;
  if (!BuildTrajectoryFromInput(input_sample, &output_sample, &status_reason)) {
    SetLastError("failed to build output sample");
    return true;
  }

  const magna::dds::ReturnCode_t write_rc =
      output_writer_->write(&output_sample, magna::dds::HANDLE_NIL);
  if (write_rc != magna::dds::ReturnCode_t::RETCODE_OK) {
    SetLastError(std::string("write failed: ") + ReturnCodeToString(write_rc));
    return true;
  }

  ++handled_samples_;
  std::cout << kModuleTag
            << " bridged sample #" << handled_samples_
            << " (input_count=" << input_sample.count()
            << ", trajectory_points=" << output_sample.trajectory_point().size()
            << ", estop=" << (output_sample.estop().is_estop() ? "true" : "false")
            << ", status=\"" << status_reason << "\")"
            << std::endl;
  return true;
}

int ValetParkingComponent::Start() {
  bool expected = false;
  if (!running_.compare_exchange_strong(expected, true)) {
    return VALET_PARKING_ERR_STATE;
  }

  {
    std::lock_guard<std::mutex> lock(mutex_);
    last_error_.clear();
  }

  if (!InitDds()) {
    running_.store(false);
    return VALET_PARKING_ERR_RUNTIME;
  }

  try {
    worker_ = std::thread(&ValetParkingComponent::WorkerLoop, this);
  } catch (const std::exception& e) {
    running_.store(false);
    CleanupDds();
    SetLastError(std::string("failed to start worker thread: ") + e.what());
    return VALET_PARKING_ERR_RUNTIME;
  }

  std::cout << kModuleTag << " started (domain=" << config_.domain_id
            << ", in_topic=" << input_topic_name_
            << ", out_topic=" << output_topic_name_;
  if (command_topic_enabled_) {
    std::cout << ", command_topic=" << command_topic_name_;
  } else {
    std::cout << ", command_topic=disabled";
  }
  if (aux_input_topics_enabled_) {
    std::cout << ", localization_topic=" << localization_topic_name_
              << ", chassis_topic=" << chassis_topic_name_
              << ", obstacle_topic=" << obstacle_topic_name_;
  } else {
    std::cout << ", aux_input_topics=disabled";
  }
  std::cout << ")" << std::endl;
  return VALET_PARKING_OK;
}

int ValetParkingComponent::Stop() {
  bool expected = true;
  if (!running_.compare_exchange_strong(expected, false)) {
    return VALET_PARKING_OK;
  }

  cv_.notify_all();
  if (worker_.joinable()) {
    worker_.join();
  }

  CleanupDds();
  std::cout << kModuleTag << " stopped" << std::endl;
  return VALET_PARKING_OK;
}

int ValetParkingComponent::UpdateVehicleState(
    const valet_parking_vehicle_state_t& vehicle_state) {
  const int ret = stage_parking_adapter_.UpdateVehicleState(vehicle_state);
  if (ret != VALET_PARKING_OK) {
    SetLastError("invalid vehicle state input");
  }
  return ret;
}

int ValetParkingComponent::ClearVehicleState() {
  return stage_parking_adapter_.ClearVehicleState();
}

int ValetParkingComponent::UpdateObstacles(
    const valet_parking_obstacle_t* obstacles,
    uint32_t obstacle_count) {
  const int ret =
      stage_parking_adapter_.UpdateObstacles(obstacles, obstacle_count);
  if (ret != VALET_PARKING_OK) {
    SetLastError("invalid obstacle input");
  }
  return ret;
}

int ValetParkingComponent::ClearObstacles() {
  return stage_parking_adapter_.ClearObstacles();
}

const std::string& ValetParkingComponent::LastError() const {
  return last_error_;
}

void ValetParkingComponent::WorkerLoop() {
  while (running_.load()) {
    bool handled = false;
    try {
      handled = DrainCommandSamples();
      handled = DrainAuxInputSamples() || handled;
#if ENABLE_PRKVINBUS
      handled = HandlePrkVinBusSample() || handled;
#endif
      handled = HandleOneSample() || handled;
    } catch (const std::exception& e) {
      SetLastError(std::string("worker exception: ") + e.what());
    } catch (...) {
      SetLastError("worker exception: unknown");
    }

    if (!handled) {
      std::unique_lock<std::mutex> lock(mutex_);
      cv_.wait_for(lock, std::chrono::milliseconds(20));
    }
  }
}

}  // namespace valet_parking
