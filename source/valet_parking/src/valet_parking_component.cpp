#include "valet_parking_component.h"
#include "valet_parking_topics.h"
#include "valet_parking_topicsTopicDataType.h"

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
constexpr int kTrajectoryPointCount = 21;

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

}  // namespace

ValetParkingComponent::ValetParkingComponent(const valet_parking_config_t& config)
    : config_(config),
      stage_parking_adapter_(config_),
      input_topic_name_(config.input_topic_name != nullptr ? config.input_topic_name
                                                            : kSelectedSlotTopicDefault),
      output_topic_name_(config.output_topic_name != nullptr ? config.output_topic_name
                                                              : kPlanningTrajectoryTopicDefault) {
  if (input_topic_name_.empty()) {
    input_topic_name_ = kSelectedSlotTopicDefault;
  }
  if (output_topic_name_.empty()) {
    output_topic_name_ = kPlanningTrajectoryTopicDefault;
  }
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
  input_reader_ = nullptr;
  publisher_ = nullptr;
  subscriber_ = nullptr;
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
  planning_trajectory_topic_type_.reset();
  selected_slot_topic_type_.reset();
}

bool ValetParkingComponent::BuildTrajectoryFromInput(const SelectedSlot& input_sample,
                                                     PlanningTrajectory* output_sample,
                                                     std::string* status_reason) {
  if (output_sample == nullptr || status_reason == nullptr) {
    return false;
  }

  return stage_parking_adapter_.Process(input_sample, output_sample, status_reason);
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
            << ", out_topic=" << output_topic_name_
            << ")" << std::endl;
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

const std::string& ValetParkingComponent::LastError() const {
  return last_error_;
}

void ValetParkingComponent::WorkerLoop() {
  while (running_.load()) {
    bool handled = false;
    try {
      handled = HandleOneSample();
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
