#include "magnadds/MagnaDDS.h"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <thread>

namespace {

constexpr const char* kRawTypeName = "ValetParkingRawV1";

struct PublisherOptions {
  uint32_t domain_id{0U};
  std::string topic_name{"/selected_slot"};
  std::string mode{"valid"};
  uint32_t count{3U};
  uint32_t interval_ms{100U};
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

bool IsModeSupported(const std::string& mode) {
  return mode == "valid" || mode == "empty" || mode == "overflow" || mode == "nan";
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

std::string BuildPayload(const PublisherOptions& options, uint32_t index) {
  const uint64_t stamp_ms = NowMilliseconds();
  const uint64_t seq = stamp_ms + static_cast<uint64_t>(index);

  std::ostringstream out;
  out << "schema=selected_slot_v1";
  out << ";seq=" << seq;
  out << ";frame_id=selected_slot_mock_publisher";
  out << ";publish_stamp_ms=" << stamp_ms;
  out << ";data_stamp_ms=" << stamp_ms;
  out << ";mode=" << options.mode;

  if (options.mode == "valid") {
    out << ";is_valid=1";
    out << ";center_x=8.500";
    out << ";center_y=2.000";
    out << ";heading=0.200";
    out << ";length=5.200";
    out << ";width=2.400";
    out << ";count=1";
  } else if (options.mode == "empty") {
    out << ";is_valid=0";
    out << ";center_x=0.000";
    out << ";center_y=0.000";
    out << ";heading=0.000";
    out << ";length=5.200";
    out << ";width=2.400";
    out << ";count=0";
  } else if (options.mode == "overflow") {
    out << ";is_valid=1";
    out << ";center_x=8.500";
    out << ";center_y=2.000";
    out << ";heading=0.200";
    out << ";length=5.200";
    out << ";width=2.400";
    out << ";count=999999";
  } else {
    out << ";is_valid=1";
    out << ";center_x=nan";
    out << ";center_y=2.000";
    out << ";heading=0.200";
    out << ";length=5.200";
    out << ";width=2.400";
    out << ";count=1";
  }

  return out.str();
}

void PrintUsage() {
  std::cout << "Usage: selected_slot_mock_publisher [options]\n"
            << "Options:\n"
            << "  --domain-id=<uint32>          DDS domain id (default 0)\n"
            << "  --topic=<name>                topic name (default /selected_slot)\n"
            << "  --mode=<valid|empty|overflow|nan>\n"
            << "  --count=<uint32>              number of samples to publish (default 3)\n"
            << "  --interval-ms=<uint32>        interval between samples (default 100)\n"
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
    const std::string topic_prefix = "--topic=";
    const std::string mode_prefix = "--mode=";
    const std::string count_prefix = "--count=";
    const std::string interval_prefix = "--interval-ms=";

    if (arg.rfind(domain_prefix, 0) == 0) {
      const std::string value = arg.substr(domain_prefix.size());
      if (!ParseUint32(value, &options.domain_id)) {
        std::cerr << "[selected_slot_mock_publisher] invalid --domain-id: " << value
                  << std::endl;
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
        std::cerr << "[selected_slot_mock_publisher] invalid --mode: " << options.mode
                  << std::endl;
        return 2;
      }
      continue;
    }

    if (arg.rfind(count_prefix, 0) == 0) {
      const std::string value = arg.substr(count_prefix.size());
      if (!ParseUint32(value, &options.count) || options.count == 0U) {
        std::cerr << "[selected_slot_mock_publisher] invalid --count: " << value
                  << std::endl;
        return 2;
      }
      continue;
    }

    if (arg.rfind(interval_prefix, 0) == 0) {
      const std::string value = arg.substr(interval_prefix.size());
      if (!ParseUint32(value, &options.interval_ms)) {
        std::cerr << "[selected_slot_mock_publisher] invalid --interval-ms: " << value
                  << std::endl;
        return 2;
      }
      continue;
    }

    std::cerr << "[selected_slot_mock_publisher] unknown argument: " << arg << std::endl;
    PrintUsage();
    return 2;
  }

  std::cout << "[selected_slot_mock_publisher] domain=" << options.domain_id
            << ", topic=" << options.topic_name
            << ", mode=" << options.mode
            << ", count=" << options.count
            << ", interval_ms=" << options.interval_ms << std::endl;

  magna::dds::DomainParticipantFactory* factory =
      magna::dds::DomainParticipantFactory::get_instance();
  if (factory == nullptr) {
    std::cerr << "[selected_slot_mock_publisher] failed to get DomainParticipantFactory" << std::endl;
    return 3;
  }

  magna::dds::DomainParticipant* participant =
      factory->create_participant(static_cast<magna::dds::DomainId_t>(options.domain_id));
  if (participant == nullptr) {
    std::cerr << "[selected_slot_mock_publisher] failed to create DomainParticipant" << std::endl;
    return 3;
  }

  auto cleanup = [&factory, &participant]() {
    if (participant != nullptr) {
      (void)participant->delete_contained_entities();
      (void)factory->delete_participant(participant);
      participant = nullptr;
    }
  };

  magna::dds::TopicDataType_raw raw_type(kRawTypeName);
  magna::dds::ReturnCode_t rc = participant->register_type(&raw_type);
  if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
    std::cerr << "[selected_slot_mock_publisher] register_type failed: "
              << ReturnCodeToString(rc) << std::endl;
    cleanup();
    return 4;
  }

  magna::dds::Topic* topic = participant->create_topic(
      options.topic_name, raw_type.get_name(), magna::dds::TOPIC_QOS_DEFAULT);
  if (topic == nullptr) {
    std::cerr << "[selected_slot_mock_publisher] create_topic failed" << std::endl;
    cleanup();
    return 4;
  }

  magna::dds::Publisher* publisher =
      participant->create_publisher(magna::dds::PUBLISHER_QOS_DEFAULT);
  if (publisher == nullptr) {
    std::cerr << "[selected_slot_mock_publisher] create_publisher failed" << std::endl;
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
    std::cerr << "[selected_slot_mock_publisher] create_datawriter failed" << std::endl;
    cleanup();
    return 4;
  }

  for (uint32_t i = 0U; i < options.count; ++i) {
    const std::string payload = BuildPayload(options, i);
    const magna::dds::OctetSeq raw_payload(payload.begin(), payload.end());
    rc = writer->write_original(raw_payload);
    if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
      std::cerr << "[selected_slot_mock_publisher] write_original failed at sample " << i
                << ": " << ReturnCodeToString(rc) << std::endl;
      cleanup();
      return 5;
    }

    std::cout << "[selected_slot_mock_publisher] published sample " << (i + 1U)
              << "/" << options.count
              << " bytes=" << raw_payload.size() << std::endl;

    if (i + 1U < options.count && options.interval_ms > 0U) {
      std::this_thread::sleep_for(std::chrono::milliseconds(options.interval_ms));
    }
  }

  cleanup();
  std::cout << "[selected_slot_mock_publisher] done." << std::endl;
  return 0;
}
