#include "magnadds/MagnaDDS.h"
#include "valet_parking_topics.h"
#include "valet_parking_topicsTopicDataType.h"

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <limits>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace {

enum class AuxPublishMode {
  kAllValid,
  kInvalidLocalization,
  kNanLocalization,
  kChassisOnly,
  kInvalidObstacles,
  kBadObstacleGeometry,
  kMovingLocalization,
  kMovingLocalizationLarge,
  kFarLocalization,
  kFarObstacles,
  kManyObstacles,
  kObstacleAppears,
  kObstacleDisappears,
};

constexpr uint32_t kManyObstaclesCount = 128U;
constexpr uint32_t kObstacleAppearsStartIndex = 3U;
constexpr uint32_t kObstacleDisappearsClearIndex = 3U;

struct PublisherOptions {
  uint32_t domain_id{0U};
  std::string localization_topic{"/localization/estimate"};
  std::string chassis_topic{"/chassis/state"};
  std::string obstacle_topic{"/perception/obstacles"};
  AuxPublishMode mode{AuxPublishMode::kAllValid};
  std::string mode_name{"all-valid"};
  GearPosition chassis_gear{GearPosition::GEAR_PARKING};
  std::string chassis_gear_name{"parking"};
  double chassis_speed_mps{0.0};
  uint32_t count{3U};
  uint32_t interval_ms{100U};
};

uint64_t NowMilliseconds() {
  return static_cast<uint64_t>(
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count());
}

bool ParseUint32(const std::string& text, uint32_t* out_value) {
  if (out_value == nullptr || text.empty()) {
    return false;
  }

  errno = 0;
  char* end = nullptr;
  const unsigned long parsed = std::strtoul(text.c_str(), &end, 10);

  if (errno != 0 || end == nullptr || *end != '\0') {
    return false;
  }

  if (parsed > static_cast<unsigned long>(std::numeric_limits<uint32_t>::max())) {
    return false;
  }

  *out_value = static_cast<uint32_t>(parsed);
  return true;
}

bool ParseDouble(const std::string& text, double* out_value) {
  if (out_value == nullptr || text.empty()) {
    return false;
  }

  errno = 0;
  char* end = nullptr;
  const double parsed = std::strtod(text.c_str(), &end);
  if (errno != 0 || end == nullptr || *end != '\0' ||
      !std::isfinite(parsed)) {
    return false;
  }

  *out_value = parsed;
  return true;
}

bool ParseMode(const std::string& text, AuxPublishMode* out_mode) {
  if (out_mode == nullptr) {
    return false;
  }
  if (text == "all-valid") {
    *out_mode = AuxPublishMode::kAllValid;
    return true;
  }
  if (text == "invalid-localization") {
    *out_mode = AuxPublishMode::kInvalidLocalization;
    return true;
  }
  if (text == "nan-localization") {
    *out_mode = AuxPublishMode::kNanLocalization;
    return true;
  }
  if (text == "chassis-only") {
    *out_mode = AuxPublishMode::kChassisOnly;
    return true;
  }
  if (text == "invalid-obstacles") {
    *out_mode = AuxPublishMode::kInvalidObstacles;
    return true;
  }
  if (text == "bad-obstacle-geometry") {
    *out_mode = AuxPublishMode::kBadObstacleGeometry;
    return true;
  }
  if (text == "moving-localization") {
    *out_mode = AuxPublishMode::kMovingLocalization;
    return true;
  }
  if (text == "moving-localization-large") {
    *out_mode = AuxPublishMode::kMovingLocalizationLarge;
    return true;
  }
  if (text == "far-localization") {
    *out_mode = AuxPublishMode::kFarLocalization;
    return true;
  }
  if (text == "far-obstacles") {
    *out_mode = AuxPublishMode::kFarObstacles;
    return true;
  }
  if (text == "many-obstacles") {
    *out_mode = AuxPublishMode::kManyObstacles;
    return true;
  }
  if (text == "obstacle-appears") {
    *out_mode = AuxPublishMode::kObstacleAppears;
    return true;
  }
  if (text == "obstacle-disappears") {
    *out_mode = AuxPublishMode::kObstacleDisappears;
    return true;
  }
  return false;
}

bool ParseGear(const std::string& text, GearPosition* out_gear) {
  if (out_gear == nullptr) {
    return false;
  }
  if (text == "parking") {
    *out_gear = GearPosition::GEAR_PARKING;
    return true;
  }
  if (text == "drive") {
    *out_gear = GearPosition::GEAR_DRIVE;
    return true;
  }
  if (text == "reverse") {
    *out_gear = GearPosition::GEAR_REVERSE;
    return true;
  }
  if (text == "neutral") {
    *out_gear = GearPosition::GEAR_NEUTRAL;
    return true;
  }
  return false;
}

std::string ReturnCodeToString(magna::dds::ReturnCode_t rc) {
  using RC = magna::dds::ReturnCode_t;
  switch (rc) {
    case RC::RETCODE_OK:
      return "RETCODE_OK";
    case RC::RETCODE_ERROR:
      return "RETCODE_ERROR";
    case RC::RETCODE_UNSUPPORTED:
      return "RETCODE_UNSUPPORTED";
    case RC::RETCODE_NO_DATA:
      return "RETCODE_NO_DATA";
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
    case RC::RETCODE_ILLEGAL_OPERATION:
      return "RETCODE_ILLEGAL_OPERATION";
    default:
      return "RETCODE_ERROR";
  }
}

Header BuildHeader(const std::string& frame_id, uint32_t index) {
  const uint64_t stamp_ms = NowMilliseconds();
  Header header;
  header.seq(stamp_ms + static_cast<uint64_t>(index));
  header.frame_id(frame_id);
  header.publish_stamp_ms(stamp_ms);
  header.data_stamp_ms(stamp_ms);
  return header;
}

bool IsLastSample(const PublisherOptions& options, uint32_t index) {
  return index + 1U >= options.count;
}

bool ShouldPublishLocalization(const PublisherOptions& options) {
  return options.mode != AuxPublishMode::kChassisOnly;
}

bool ShouldPublishChassis(const PublisherOptions& /*options*/) {
  return true;
}

bool ShouldPublishObstacles(const PublisherOptions& options, uint32_t index) {
  if (options.mode == AuxPublishMode::kChassisOnly) {
    return false;
  }
  if (options.mode == AuxPublishMode::kObstacleAppears &&
      index < kObstacleAppearsStartIndex) {
    return false;
  }
  return true;
}

std::string LocalizationStatusForLog(const PublisherOptions& options,
                                     const LocalizationEstimate& sample,
                                     uint32_t index) {
  if (!ShouldPublishLocalization(options)) {
    return "skipped";
  }
  if (IsLastSample(options, index) &&
      options.mode == AuxPublishMode::kNanLocalization) {
    return "nan";
  }
  if (options.mode == AuxPublishMode::kMovingLocalization) {
    return "moving-valid";
  }
  if (options.mode == AuxPublishMode::kMovingLocalizationLarge) {
    return "moving-large-valid";
  }
  if (options.mode == AuxPublishMode::kFarLocalization) {
    return "far-valid";
  }
  return sample.is_valid() ? "valid" : "invalid";
}

std::string ObstacleStatusForLog(const PublisherOptions& options,
                                 const ObstacleArray& sample,
                                 uint32_t index) {
  if (!ShouldPublishObstacles(options, index)) {
    return "skipped";
  }
  if (IsLastSample(options, index) &&
      options.mode == AuxPublishMode::kBadObstacleGeometry) {
    return "bad-geometry";
  }
  if (options.mode == AuxPublishMode::kManyObstacles) {
    return "many-valid";
  }
  if (options.mode == AuxPublishMode::kFarObstacles) {
    return "far-valid";
  }
  if (options.mode == AuxPublishMode::kObstacleAppears) {
    return "appeared-valid";
  }
  if (options.mode == AuxPublishMode::kObstacleDisappears) {
    return sample.obstacles().empty() ? "cleared-valid" : "present-valid";
  }
  return sample.is_valid() ? "valid" : "invalid";
}

LocalizationEstimate BuildLocalization(const PublisherOptions& options,
                                        uint32_t index) {
  LocalizationEstimate sample;
  sample.header(BuildHeader("aux_input_mock_publisher/localization", index));
  sample.is_valid(true);
  sample.x(0.0);
  sample.y(0.0);
  sample.z(0.0);
  sample.heading(0.0);
  if (options.mode == AuxPublishMode::kMovingLocalization && index > 2U) {
    sample.x(0.80);
    sample.y(0.70);
  }
  if (options.mode == AuxPublishMode::kMovingLocalizationLarge && index > 2U) {
    sample.x(1.40);
    sample.y(1.30);
  }
  if (options.mode == AuxPublishMode::kFarLocalization) {
    sample.x(1000.0);
    sample.y(1000.0);
  }
  if (IsLastSample(options, index) &&
      options.mode == AuxPublishMode::kInvalidLocalization) {
    sample.is_valid(false);
  }
  if (IsLastSample(options, index) &&
      options.mode == AuxPublishMode::kNanLocalization) {
    sample.x(std::numeric_limits<double>::quiet_NaN());
  }
  return sample;
}

ChassisState BuildChassis(const PublisherOptions& options, uint32_t index) {
  ChassisState sample;
  sample.header(BuildHeader("aux_input_mock_publisher/chassis", index));
  sample.is_valid(true);
  sample.speed_mps(options.chassis_speed_mps);
  sample.acceleration_mps2(0.0);
  sample.gear(options.chassis_gear);
  return sample;
}

Obstacle BuildObstacle(const PublisherOptions& options,
                       uint32_t index,
                       uint32_t obstacle_index) {
  Obstacle obstacle;
  if (options.mode == AuxPublishMode::kMovingLocalization ||
      options.mode == AuxPublishMode::kMovingLocalizationLarge) {
    obstacle.id(1000U);
  } else if (options.mode == AuxPublishMode::kManyObstacles) {
    obstacle.id(100000U + index * 1000U + obstacle_index);
  } else if (options.mode == AuxPublishMode::kFarObstacles) {
    obstacle.id(200000U + index);
  } else if (options.mode == AuxPublishMode::kObstacleAppears) {
    obstacle.id(300000U);
  } else if (options.mode == AuxPublishMode::kObstacleDisappears) {
    obstacle.id(400000U);
  } else {
    obstacle.id(1000U + index);
  }
  obstacle.type(ObstacleType::OBSTACLE_TYPE_UNKNOWN_UNMOVABLE);
  obstacle.is_dynamic(false);
  if (options.mode == AuxPublishMode::kManyObstacles) {
    const uint32_t row = obstacle_index / 16U;
    const uint32_t column = obstacle_index % 16U;
    obstacle.center_x(18.0 + static_cast<double>(column) * 0.8);
    obstacle.center_y(18.0 + static_cast<double>(row) * 0.8);
  } else if (options.mode == AuxPublishMode::kFarObstacles) {
    obstacle.center_x(1000.0);
    obstacle.center_y(1000.0);
  } else {
    obstacle.center_x(30.0);
    obstacle.center_y(30.0);
  }
  obstacle.heading(0.0);
  obstacle.length(options.mode == AuxPublishMode::kManyObstacles ? 0.25 : 0.5);
  obstacle.width(options.mode == AuxPublishMode::kManyObstacles ? 0.25 : 0.5);
  obstacle.velocity_x(0.0);
  obstacle.velocity_y(0.0);
  return obstacle;
}

ObstacleArray BuildObstacleArray(const PublisherOptions& options,
                                 uint32_t index) {
  ObstacleArray sample;
  sample.header(BuildHeader("aux_input_mock_publisher/obstacles", index));
  sample.is_valid(true);
  if (IsLastSample(options, index) &&
      options.mode == AuxPublishMode::kInvalidObstacles) {
    sample.is_valid(false);
    sample.obstacles(std::vector<Obstacle>{});
    return sample;
  }
  if (options.mode == AuxPublishMode::kManyObstacles) {
    std::vector<Obstacle> obstacles;
    obstacles.reserve(kManyObstaclesCount);
    for (uint32_t i = 0U; i < kManyObstaclesCount; ++i) {
      obstacles.push_back(BuildObstacle(options, index, i));
    }
    sample.obstacles(std::move(obstacles));
    return sample;
  }
  if (options.mode == AuxPublishMode::kObstacleDisappears &&
      index >= kObstacleDisappearsClearIndex) {
    sample.obstacles(std::vector<Obstacle>{});
    return sample;
  }

  Obstacle obstacle = BuildObstacle(options, index, 0U);
  if (IsLastSample(options, index) &&
      options.mode == AuxPublishMode::kBadObstacleGeometry) {
    obstacle.length(-1.0);
  }
  sample.obstacles(std::vector<Obstacle>{obstacle});
  return sample;
}

void PrintUsage() {
  std::cout << "Usage: aux_input_mock_publisher [options]\n"
            << "Options:\n"
            << "  --domain-id=<uint32>          DDS domain id (default 0)\n"
            << "  --localization-topic=<name>   localization topic (default /localization/estimate)\n"
            << "  --chassis-topic=<name>        chassis topic (default /chassis/state)\n"
            << "  --obstacle-topic=<name>       obstacle topic (default /perception/obstacles)\n"
            << "  --mode=<name>                 all-valid|invalid-localization|nan-localization|\n"
            << "                                chassis-only|invalid-obstacles|bad-obstacle-geometry|\n"
            << "                                moving-localization|moving-localization-large|\n"
            << "                                far-localization|far-obstacles|many-obstacles|\n"
            << "                                obstacle-appears|obstacle-disappears\n"
            << "  --chassis-gear=<name>         parking|drive|reverse|neutral (default parking)\n"
            << "  --chassis-speed-mps=<value>   signed chassis speed in m/s (default 0.0)\n"
            << "  --count=<uint32>              number of sample groups to publish (default 3)\n"
            << "  --interval-ms=<uint32>        interval between sample groups (default 100)\n"
            << "  --help                        show this message\n";
}

}  // namespace

int main(int argc, char* argv[]) {
  PublisherOptions options;

  for (int i = 1; i < argc; ++i) {
    const std::string arg(argv[i]);

    if (arg == "--help") {
      PrintUsage();
      return 0;
    }

    const std::string domain_prefix = "--domain-id=";
    const std::string localization_prefix = "--localization-topic=";
    const std::string chassis_prefix = "--chassis-topic=";
    const std::string obstacle_prefix = "--obstacle-topic=";
    const std::string mode_prefix = "--mode=";
    const std::string chassis_gear_prefix = "--chassis-gear=";
    const std::string chassis_speed_prefix = "--chassis-speed-mps=";
    const std::string count_prefix = "--count=";
    const std::string interval_prefix = "--interval-ms=";

    if (arg.rfind(domain_prefix, 0) == 0) {
      const std::string value = arg.substr(domain_prefix.size());
      if (!ParseUint32(value, &options.domain_id)) {
        std::cerr << "[aux_input_mock_publisher] invalid --domain-id: "
                  << value << std::endl;
        return 2;
      }
      continue;
    }

    if (arg.rfind(localization_prefix, 0) == 0) {
      options.localization_topic = arg.substr(localization_prefix.size());
      continue;
    }

    if (arg.rfind(chassis_prefix, 0) == 0) {
      options.chassis_topic = arg.substr(chassis_prefix.size());
      continue;
    }

    if (arg.rfind(obstacle_prefix, 0) == 0) {
      options.obstacle_topic = arg.substr(obstacle_prefix.size());
      continue;
    }

    if (arg.rfind(mode_prefix, 0) == 0) {
      options.mode_name = arg.substr(mode_prefix.size());
      if (!ParseMode(options.mode_name, &options.mode)) {
        std::cerr << "[aux_input_mock_publisher] invalid --mode: "
                  << options.mode_name << std::endl;
        return 2;
      }
      continue;
    }

    if (arg.rfind(chassis_gear_prefix, 0) == 0) {
      options.chassis_gear_name = arg.substr(chassis_gear_prefix.size());
      if (!ParseGear(options.chassis_gear_name, &options.chassis_gear)) {
        std::cerr << "[aux_input_mock_publisher] invalid --chassis-gear: "
                  << options.chassis_gear_name << std::endl;
        return 2;
      }
      continue;
    }

    if (arg.rfind(chassis_speed_prefix, 0) == 0) {
      const std::string value = arg.substr(chassis_speed_prefix.size());
      if (!ParseDouble(value, &options.chassis_speed_mps)) {
        std::cerr << "[aux_input_mock_publisher] invalid --chassis-speed-mps: "
                  << value << std::endl;
        return 2;
      }
      continue;
    }

    if (arg.rfind(count_prefix, 0) == 0) {
      const std::string value = arg.substr(count_prefix.size());
      if (!ParseUint32(value, &options.count) || options.count == 0U) {
        std::cerr << "[aux_input_mock_publisher] invalid --count: "
                  << value << std::endl;
        return 2;
      }
      continue;
    }

    if (arg.rfind(interval_prefix, 0) == 0) {
      const std::string value = arg.substr(interval_prefix.size());
      if (!ParseUint32(value, &options.interval_ms)) {
        std::cerr << "[aux_input_mock_publisher] invalid --interval-ms: "
                  << value << std::endl;
        return 2;
      }
      continue;
    }

    std::cerr << "[aux_input_mock_publisher] unknown argument: " << arg
              << std::endl;
    PrintUsage();
    return 2;
  }

  std::cout << "[aux_input_mock_publisher] domain=" << options.domain_id
            << ", localization_topic=" << options.localization_topic
            << ", chassis_topic=" << options.chassis_topic
            << ", obstacle_topic=" << options.obstacle_topic
            << ", mode=" << options.mode_name
            << ", chassis_gear=" << options.chassis_gear_name
            << ", chassis_speed_mps=" << options.chassis_speed_mps
            << ", count=" << options.count
            << ", interval_ms=" << options.interval_ms << std::endl;

  magna::dds::DomainParticipantFactory* factory =
      magna::dds::DomainParticipantFactory::get_instance();
  if (factory == nullptr) {
    std::cerr << "[aux_input_mock_publisher] failed to get DomainParticipantFactory"
              << std::endl;
    return 3;
  }

  magna::dds::DomainParticipant* participant =
      factory->create_participant(
          static_cast<magna::dds::DomainId_t>(options.domain_id));
  if (participant == nullptr) {
    std::cerr << "[aux_input_mock_publisher] failed to create DomainParticipant"
              << std::endl;
    return 3;
  }

  auto cleanup = [&factory, &participant]() {
    if (participant != nullptr) {
      (void)participant->delete_contained_entities();
      (void)factory->delete_participant(participant);
      participant = nullptr;
    }
  };

  LocalizationEstimateTopicDataType localization_type;
  ChassisStateTopicDataType chassis_type;
  ObstacleArrayTopicDataType obstacle_array_type;

  magna::dds::ReturnCode_t rc = participant->register_type(&localization_type);
  if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
    std::cerr << "[aux_input_mock_publisher] register localization type failed: "
              << ReturnCodeToString(rc) << std::endl;
    cleanup();
    return 4;
  }

  rc = participant->register_type(&chassis_type);
  if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
    std::cerr << "[aux_input_mock_publisher] register chassis type failed: "
              << ReturnCodeToString(rc) << std::endl;
    cleanup();
    return 4;
  }

  rc = participant->register_type(&obstacle_array_type);
  if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
    std::cerr << "[aux_input_mock_publisher] register obstacle array type failed: "
              << ReturnCodeToString(rc) << std::endl;
    cleanup();
    return 4;
  }

  magna::dds::Topic* localization_topic = participant->create_topic(
      options.localization_topic, localization_type.get_name(),
      magna::dds::TOPIC_QOS_DEFAULT);
  magna::dds::Topic* chassis_topic = participant->create_topic(
      options.chassis_topic, chassis_type.get_name(),
      magna::dds::TOPIC_QOS_DEFAULT);
  magna::dds::Topic* obstacle_topic = participant->create_topic(
      options.obstacle_topic, obstacle_array_type.get_name(),
      magna::dds::TOPIC_QOS_DEFAULT);

  if (localization_topic == nullptr || chassis_topic == nullptr ||
      obstacle_topic == nullptr) {
    std::cerr << "[aux_input_mock_publisher] create_topic failed" << std::endl;
    cleanup();
    return 4;
  }

  magna::dds::Publisher* publisher =
      participant->create_publisher(magna::dds::PUBLISHER_QOS_DEFAULT);
  if (publisher == nullptr) {
    std::cerr << "[aux_input_mock_publisher] create_publisher failed"
              << std::endl;
    cleanup();
    return 4;
  }

  magna::dds::DataWriterQos writer_qos = magna::dds::DATAWRITER_QOS_DEFAULT;
  writer_qos.history.kind =
      magna::dds::HistoryQosPolicyKind::KEEP_LAST_HISTORY_QOS;
  writer_qos.history.depth = 10;
  writer_qos.resource_limits.max_samples = 10;
  writer_qos.resource_limits.max_samples_per_instance = 10;

  magna::dds::DataWriter* localization_writer = publisher->create_datawriter(
      localization_topic, writer_qos, nullptr, magna::dds::STATUS_MASK_NONE);
  magna::dds::DataWriter* chassis_writer = publisher->create_datawriter(
      chassis_topic, writer_qos, nullptr, magna::dds::STATUS_MASK_NONE);
  magna::dds::DataWriter* obstacle_writer = publisher->create_datawriter(
      obstacle_topic, writer_qos, nullptr, magna::dds::STATUS_MASK_NONE);

  if (localization_writer == nullptr || chassis_writer == nullptr ||
      obstacle_writer == nullptr) {
    std::cerr << "[aux_input_mock_publisher] create_datawriter failed"
              << std::endl;
    cleanup();
    return 4;
  }

  for (uint32_t i = 0U; i < options.count; ++i) {
    LocalizationEstimate localization = BuildLocalization(options, i);
    ChassisState chassis = BuildChassis(options, i);
    ObstacleArray obstacles = BuildObstacleArray(options, i);

    if (ShouldPublishLocalization(options)) {
      rc = localization_writer->write(&localization, magna::dds::HANDLE_NIL);
      if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
        std::cerr << "[aux_input_mock_publisher] write localization failed: "
                  << ReturnCodeToString(rc) << std::endl;
        cleanup();
        return 5;
      }
    }

    if (ShouldPublishChassis(options)) {
      rc = chassis_writer->write(&chassis, magna::dds::HANDLE_NIL);
      if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
        std::cerr << "[aux_input_mock_publisher] write chassis failed: "
                  << ReturnCodeToString(rc) << std::endl;
        cleanup();
        return 5;
      }
    }

    if (ShouldPublishObstacles(options, i)) {
      rc = obstacle_writer->write(&obstacles, magna::dds::HANDLE_NIL);
      if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
        std::cerr << "[aux_input_mock_publisher] write obstacles failed: "
                  << ReturnCodeToString(rc) << std::endl;
        cleanup();
        return 5;
      }
    }

    std::cout << "[aux_input_mock_publisher] published sample group "
              << (i + 1U) << "/" << options.count
              << " localization="
              << LocalizationStatusForLog(options, localization, i)
              << " chassis="
              << (ShouldPublishChassis(options) ? "valid" : "skipped")
              << " gear=" << options.chassis_gear_name
              << " speed_mps=" << options.chassis_speed_mps
              << " obstacles="
              << ObstacleStatusForLog(options, obstacles, i)
              << " obstacle_count="
              << (ShouldPublishObstacles(options, i)
                      ? obstacles.obstacles().size()
                      : 0U)
              << std::endl;

    if (i + 1U < options.count && options.interval_ms > 0U) {
      std::this_thread::sleep_for(std::chrono::milliseconds(options.interval_ms));
    }
  }

  cleanup();
  std::cout << "[aux_input_mock_publisher] done." << std::endl;
  return 0;
}
