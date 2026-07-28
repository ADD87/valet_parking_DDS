#include "magnadds/MagnaDDS.h"
#include "valet_parking_topics.h"
#include "valet_parking_topicsTopicDataType.h"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <limits>
#include <string>
#include <thread>

namespace {

struct PublisherOptions {
  uint32_t domain_id{0U};
  std::string topic_name{"/parking/command"};
  std::string mode{"none"};
  uint32_t count{1U};
  uint32_t interval_ms{100U};
  uint32_t parking_seq{0U};
  double direct_distance_m{3.0};
  double direct_speed_mps{0.8};
  bool reset_history{false};
  std::string reason;
};

uint64_t NowMilliseconds() {
  return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()).count());
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
  if (errno != 0 || end == nullptr || *end != '\0') {
    return false;
  }

  *out_value = parsed;
  return true;
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

bool IsModeSupported(const std::string& mode) {
  return mode == "none" || mode == "parking-in" ||
         mode == "parking-out-left" || mode == "parking-out-right" ||
         mode == "parking-out-front" || mode == "parking-out-back" ||
         mode == "direct-forward" || mode == "direct-backward" ||
         mode == "pause" || mode == "brake" || mode == "finish" ||
         mode == "invalid";
}

ParkingCommandMode MapMode(const std::string& mode) {
  if (mode == "parking-in") {
    return ParkingCommandMode::PARKING_COMMAND_PARKING_IN;
  }
  if (mode == "parking-out-left") {
    return ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_LEFT;
  }
  if (mode == "parking-out-right") {
    return ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_RIGHT;
  }
  if (mode == "parking-out-front") {
    return ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_FRONT;
  }
  if (mode == "parking-out-back") {
    return ParkingCommandMode::PARKING_COMMAND_PARKING_OUT_BACK;
  }
  if (mode == "direct-forward") {
    return ParkingCommandMode::PARKING_COMMAND_DIRECT_FORWARD;
  }
  if (mode == "direct-backward") {
    return ParkingCommandMode::PARKING_COMMAND_DIRECT_BACKWARD;
  }
  if (mode == "pause") {
    return ParkingCommandMode::PARKING_COMMAND_PAUSE;
  }
  if (mode == "brake") {
    return ParkingCommandMode::PARKING_COMMAND_BRAKE;
  }
  if (mode == "finish") {
    return ParkingCommandMode::PARKING_COMMAND_FINISH;
  }
  return ParkingCommandMode::PARKING_COMMAND_NONE;
}

std::string ModeName(ParkingCommandMode mode) {
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

Header BuildHeader(uint32_t index) {
  const uint64_t stamp_ms = NowMilliseconds();
  Header header;
  header.seq(stamp_ms + static_cast<uint64_t>(index));
  header.frame_id("parking_command_mock_publisher");
  header.publish_stamp_ms(stamp_ms);
  header.data_stamp_ms(stamp_ms);
  return header;
}

ParkingCommand BuildSample(const PublisherOptions& options, uint32_t index) {
  ParkingCommand sample;
  sample.header(BuildHeader(index));
  sample.is_valid(options.mode != "invalid");
  sample.mode(MapMode(options.mode));
  sample.parking_seq(options.parking_seq);
  sample.direct_distance_m(options.direct_distance_m);
  sample.direct_speed_mps(options.direct_speed_mps);
  sample.reset_history(options.reset_history);
  sample.reason(options.reason);
  return sample;
}

void PrintUsage() {
  std::cout << "Usage: parking_command_mock_publisher [options]\n"
            << "Options:\n"
            << "  --domain-id=<uint32>          DDS domain id (default 0)\n"
            << "  --topic=<name>                topic name (default /parking/command)\n"
            << "  --mode=<none|parking-in|parking-out-left|parking-out-right|parking-out-front|parking-out-back|direct-forward|direct-backward|pause|brake|finish|invalid>\n"
            << "  --count=<uint32>              number of samples to publish (default 1)\n"
            << "  --interval-ms=<uint32>        interval between samples (default 100)\n"
            << "  --parking-seq=<uint32>        command parking sequence id (default 0)\n"
            << "  --direct-distance=<double>    direct branch distance in meters (default 3.0)\n"
            << "  --direct-speed=<double>       direct branch speed in m/s (default 0.8)\n"
            << "  --reset-history               request one-shot planning history reset\n"
            << "  --reason=<text>               command reason string\n"
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
    if (arg == "--reset-history") {
      options.reset_history = true;
      continue;
    }

    const std::string domain_prefix = "--domain-id=";
    const std::string topic_prefix = "--topic=";
    const std::string mode_prefix = "--mode=";
    const std::string count_prefix = "--count=";
    const std::string interval_prefix = "--interval-ms=";
    const std::string parking_seq_prefix = "--parking-seq=";
    const std::string direct_distance_prefix = "--direct-distance=";
    const std::string direct_speed_prefix = "--direct-speed=";
    const std::string reason_prefix = "--reason=";

    if (arg.rfind(domain_prefix, 0) == 0) {
      const std::string value = arg.substr(domain_prefix.size());
      if (!ParseUint32(value, &options.domain_id)) {
        std::cerr << "[parking_command_mock_publisher] invalid --domain-id: "
                  << value << std::endl;
        return 2;
      }
      continue;
    }
    if (arg.rfind(topic_prefix, 0) == 0) {
      options.topic_name = arg.substr(topic_prefix.size());
      continue;
    }
    if (arg.rfind(mode_prefix, 0) == 0) {
      options.mode = arg.substr(mode_prefix.size());
      if (!IsModeSupported(options.mode)) {
        std::cerr << "[parking_command_mock_publisher] invalid --mode: "
                  << options.mode << std::endl;
        return 2;
      }
      continue;
    }
    if (arg.rfind(count_prefix, 0) == 0) {
      const std::string value = arg.substr(count_prefix.size());
      if (!ParseUint32(value, &options.count) || options.count == 0U) {
        std::cerr << "[parking_command_mock_publisher] invalid --count: "
                  << value << std::endl;
        return 2;
      }
      continue;
    }
    if (arg.rfind(interval_prefix, 0) == 0) {
      const std::string value = arg.substr(interval_prefix.size());
      if (!ParseUint32(value, &options.interval_ms)) {
        std::cerr << "[parking_command_mock_publisher] invalid --interval-ms: "
                  << value << std::endl;
        return 2;
      }
      continue;
    }
    if (arg.rfind(parking_seq_prefix, 0) == 0) {
      const std::string value = arg.substr(parking_seq_prefix.size());
      if (!ParseUint32(value, &options.parking_seq)) {
        std::cerr << "[parking_command_mock_publisher] invalid --parking-seq: "
                  << value << std::endl;
        return 2;
      }
      continue;
    }
    if (arg.rfind(direct_distance_prefix, 0) == 0) {
      const std::string value = arg.substr(direct_distance_prefix.size());
      if (!ParseDouble(value, &options.direct_distance_m)) {
        std::cerr << "[parking_command_mock_publisher] invalid --direct-distance: "
                  << value << std::endl;
        return 2;
      }
      continue;
    }
    if (arg.rfind(direct_speed_prefix, 0) == 0) {
      const std::string value = arg.substr(direct_speed_prefix.size());
      if (!ParseDouble(value, &options.direct_speed_mps)) {
        std::cerr << "[parking_command_mock_publisher] invalid --direct-speed: "
                  << value << std::endl;
        return 2;
      }
      continue;
    }
    if (arg.rfind(reason_prefix, 0) == 0) {
      options.reason = arg.substr(reason_prefix.size());
      continue;
    }

    std::cerr << "[parking_command_mock_publisher] unknown argument: " << arg
              << std::endl;
    PrintUsage();
    return 2;
  }

  std::cout << "[parking_command_mock_publisher] domain=" << options.domain_id
            << ", topic=" << options.topic_name
            << ", mode=" << options.mode
            << ", count=" << options.count
            << ", interval_ms=" << options.interval_ms
            << std::endl;

  magna::dds::DomainParticipantFactory* factory =
      magna::dds::DomainParticipantFactory::get_instance();
  if (factory == nullptr) {
    std::cerr << "[parking_command_mock_publisher] failed to get DomainParticipantFactory"
              << std::endl;
    return 3;
  }

  magna::dds::DomainParticipant* participant =
      factory->create_participant(
          static_cast<magna::dds::DomainId_t>(options.domain_id));
  if (participant == nullptr) {
    std::cerr << "[parking_command_mock_publisher] failed to create DomainParticipant"
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

  ParkingCommandTopicDataType command_type;
  magna::dds::ReturnCode_t rc = participant->register_type(&command_type);
  if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
    std::cerr << "[parking_command_mock_publisher] register_type failed: "
              << ReturnCodeToString(rc) << std::endl;
    cleanup();
    return 4;
  }

  magna::dds::Topic* topic = participant->create_topic(
      options.topic_name, command_type.get_name(), magna::dds::TOPIC_QOS_DEFAULT);
  if (topic == nullptr) {
    std::cerr << "[parking_command_mock_publisher] create_topic failed"
              << std::endl;
    cleanup();
    return 4;
  }

  magna::dds::Publisher* publisher =
      participant->create_publisher(magna::dds::PUBLISHER_QOS_DEFAULT);
  if (publisher == nullptr) {
    std::cerr << "[parking_command_mock_publisher] create_publisher failed"
              << std::endl;
    cleanup();
    return 4;
  }

  magna::dds::DataWriterQos writer_qos = magna::dds::DATAWRITER_QOS_DEFAULT;
  writer_qos.history.kind = magna::dds::HistoryQosPolicyKind::KEEP_LAST_HISTORY_QOS;
  writer_qos.history.depth = 10;
  writer_qos.resource_limits.max_samples = 10;
  writer_qos.resource_limits.max_samples_per_instance = 10;

  magna::dds::DataWriter* writer = publisher->create_datawriter(
      topic, writer_qos, nullptr, magna::dds::STATUS_MASK_NONE);
  if (writer == nullptr) {
    std::cerr << "[parking_command_mock_publisher] create_datawriter failed"
              << std::endl;
    cleanup();
    return 4;
  }

  for (uint32_t i = 0U; i < options.count; ++i) {
    ParkingCommand sample = BuildSample(options, i);
    rc = writer->write(&sample, magna::dds::HANDLE_NIL);
    if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
      std::cerr << "[parking_command_mock_publisher] write failed at sample "
                << i << ": " << ReturnCodeToString(rc) << std::endl;
      cleanup();
      return 5;
    }

    std::cout << "[parking_command_mock_publisher] published sample "
              << (i + 1U) << "/" << options.count
              << " mode=" << options.mode
              << " enum=" << ModeName(sample.mode())
              << " is_valid=" << (sample.is_valid() ? "true" : "false")
              << " parking_seq=" << sample.parking_seq()
              << " direct_distance_m=" << sample.direct_distance_m()
              << " direct_speed_mps=" << sample.direct_speed_mps()
              << " reset_history="
              << (sample.reset_history() ? "true" : "false")
              << std::endl;

    if (i + 1U < options.count && options.interval_ms > 0U) {
      std::this_thread::sleep_for(
          std::chrono::milliseconds(options.interval_ms));
    }
  }

  cleanup();
  std::cout << "[parking_command_mock_publisher] done." << std::endl;
  return 0;
}
