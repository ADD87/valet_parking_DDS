#include "magnadds/MagnaDDS.h"
#include "valet_parking_topics.h"
#include "valet_parking_topicsTopicDataType.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <limits>
#include <string>
#include <thread>
#include <vector>

namespace {

struct SubscriberOptions {
  uint32_t domain_id{0U};
  std::string topic_name{"/planning/trajectory"};
  uint32_t timeout_ms{2000U};
  bool strict{false};
};

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

bool ValidateTrajectorySample(const PlanningTrajectory& sample,
                              std::string* validation_message) {
  if (sample.header().frame_id().empty()) {
    if (validation_message != nullptr) {
      *validation_message = "header.frame_id is empty";
    }
    return false;
  }

  if (!std::isfinite(sample.total_path_length()) ||
      !std::isfinite(sample.total_path_time()) ||
      !std::isfinite(sample.longitudinal_diff()) ||
      !std::isfinite(sample.lateral_diff())) {
    if (validation_message != nullptr) {
      *validation_message = "trajectory scalar field is non-finite";
    }
    return false;
  }

  if (sample.trajectory_point().empty()) {
    if (validation_message != nullptr) {
      *validation_message = "trajectory_point is empty";
    }
    return false;
  }

  const TrajectoryPoint& first_point = sample.trajectory_point().front();
  const PathPoint& first_path_point = first_point.path_point();
  const GaussianInfo& gaussian_info = first_point.gaussian_info();
  if (first_path_point.lane_id().empty()) {
    if (validation_message != nullptr) {
      *validation_message = "path_point.lane_id is empty";
    }
    return false;
  }

  if (!std::isfinite(first_path_point.x()) ||
      !std::isfinite(first_path_point.y()) ||
      !std::isfinite(first_path_point.theta()) ||
      !std::isfinite(first_point.v()) ||
      !std::isfinite(first_point.relative_time()) ||
      !std::isfinite(gaussian_info.area_probability()) ||
      !std::isfinite(gaussian_info.theta_a())) {
    if (validation_message != nullptr) {
      *validation_message = "trajectory point field is non-finite";
    }
    return false;
  }

  if (sample.estop().is_estop() && sample.estop().reason().empty()) {
    if (validation_message != nullptr) {
      *validation_message = "estop reason is empty";
    }
    return false;
  }

  if (validation_message != nullptr) {
    *validation_message = "required trajectory fields valid";
  }
  return true;
}

void PrintUsage() {
  std::cout << "Usage: planning_trajectory_mock_subscriber [options]\n"
            << "Options:\n"
            << "  --domain-id=<uint32>          DDS domain id (default 0)\n"
            << "  --topic=<name>                topic name (default /planning/trajectory)\n"
            << "  --timeout-ms=<uint32>         wait timeout in milliseconds (default 2000)\n"
            << "  --strict                      return non-zero on timeout\n"
            << "  --help                        show this message\n";
}

}  // namespace

int main(int argc, char* argv[]) {
  SubscriberOptions options;

  for (int i = 1; i < argc; ++i) {
    const std::string arg(argv[i]);

    if (arg == "--help") {
      PrintUsage();
      return 0;
    }
    if (arg == "--strict") {
      options.strict = true;
      continue;
    }

    const std::string domain_prefix = "--domain-id=";
    const std::string topic_prefix = "--topic=";
    const std::string timeout_prefix = "--timeout-ms=";

    if (arg.rfind(domain_prefix, 0) == 0) {
      const std::string value = arg.substr(domain_prefix.size());
      if (!ParseUint32(value, &options.domain_id)) {
        std::cerr << "[planning_trajectory_mock_subscriber] invalid --domain-id: " << value
                  << std::endl;
        return 2;
      }
      continue;
    }

    if (arg.rfind(topic_prefix, 0) == 0) {
      options.topic_name = arg.substr(topic_prefix.size());
      continue;
    }

    if (arg.rfind(timeout_prefix, 0) == 0) {
      const std::string value = arg.substr(timeout_prefix.size());
      if (!ParseUint32(value, &options.timeout_ms)) {
        std::cerr << "[planning_trajectory_mock_subscriber] invalid --timeout-ms: " << value
                  << std::endl;
        return 2;
      }
      continue;
    }

    std::cerr << "[planning_trajectory_mock_subscriber] unknown argument: " << arg << std::endl;
    PrintUsage();
    return 2;
  }

  std::cout << "[planning_trajectory_mock_subscriber] domain=" << options.domain_id
            << ", topic=" << options.topic_name
            << ", timeout_ms=" << options.timeout_ms
            << ", strict=" << (options.strict ? "true" : "false") << std::endl;

  magna::dds::DomainParticipantFactory* factory =
      magna::dds::DomainParticipantFactory::get_instance();
  if (factory == nullptr) {
    std::cerr << "[planning_trajectory_mock_subscriber] failed to get DomainParticipantFactory" << std::endl;
    return 3;
  }

  magna::dds::DomainParticipant* participant =
      factory->create_participant(static_cast<magna::dds::DomainId_t>(options.domain_id));
  if (participant == nullptr) {
    std::cerr << "[planning_trajectory_mock_subscriber] failed to create DomainParticipant" << std::endl;
    return 3;
  }

  auto cleanup = [&factory, &participant]() {
    if (participant != nullptr) {
      (void)participant->delete_contained_entities();
      (void)factory->delete_participant(participant);
      participant = nullptr;
    }
  };

  PlanningTrajectoryTopicDataType planning_trajectory_type;
  magna::dds::ReturnCode_t rc = participant->register_type(&planning_trajectory_type);
  if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
    std::cerr << "[planning_trajectory_mock_subscriber] register_type failed: "
              << ReturnCodeToString(rc) << std::endl;
    cleanup();
    return 4;
  }

  magna::dds::Topic* topic = participant->create_topic(
      options.topic_name, planning_trajectory_type.get_name(), magna::dds::TOPIC_QOS_DEFAULT);
  if (topic == nullptr) {
    std::cerr << "[planning_trajectory_mock_subscriber] create_topic failed" << std::endl;
    cleanup();
    return 4;
  }

  magna::dds::Subscriber* subscriber =
      participant->create_subscriber(magna::dds::SUBSCRIBER_QOS_DEFAULT);
  if (subscriber == nullptr) {
    std::cerr << "[planning_trajectory_mock_subscriber] create_subscriber failed" << std::endl;
    cleanup();
    return 4;
  }

  magna::dds::DataReaderQos reader_qos = magna::dds::DATAREADER_QOS_DEFAULT;
  reader_qos.history.kind = magna::dds::HistoryQosPolicyKind::KEEP_LAST_HISTORY_QOS;
  reader_qos.history.depth = 10;
  reader_qos.resource_limits.max_samples = 10;
  reader_qos.resource_limits.max_samples_per_instance = 10;

  magna::dds::DataReader* reader = subscriber->create_datareader(
      topic, reader_qos, nullptr, magna::dds::STATUS_MASK_NONE);
  if (reader == nullptr) {
    std::cerr << "[planning_trajectory_mock_subscriber] create_datareader failed" << std::endl;
    cleanup();
    return 4;
  }

  bool received = false;
  const auto deadline = std::chrono::steady_clock::now() +
      std::chrono::milliseconds(options.timeout_ms);

  while (std::chrono::steady_clock::now() < deadline) {
    PlanningTrajectory sample;
    magna::dds::SampleInfo sample_info;
    rc = reader->take_next_sample(&sample, sample_info);

    if (rc == magna::dds::ReturnCode_t::RETCODE_NO_DATA) {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
      continue;
    }

    if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
      std::cerr << "[planning_trajectory_mock_subscriber] take_next_sample failed: "
                << ReturnCodeToString(rc) << std::endl;
      cleanup();
      return 6;
    }

    if (!sample_info.valid_data) {
      continue;
    }

    std::string validation_message;
    const bool valid_sample = ValidateTrajectorySample(sample, &validation_message);
    if (!valid_sample) {
      std::cerr << "[planning_trajectory_mock_subscriber] invalid sample: "
                << validation_message << std::endl;
      cleanup();
      return 7;
    }

    const bool is_estop = sample.estop().is_estop();
    std::cout << "[planning_trajectory_mock_subscriber] received sample points="
              << sample.trajectory_point().size()
              << ", length=" << sample.total_path_length()
              << ", is_estop=" << (is_estop ? "true" : "false")
              << std::endl;
    std::cout << "[planning_trajectory_mock_subscriber] validation: "
              << validation_message << std::endl;
    received = true;
    break;
  }

  cleanup();

  if (!received) {
    if (options.strict) {
      std::cerr << "[planning_trajectory_mock_subscriber] timeout without sample (strict mode)."
                << std::endl;
      return 5;
    }

    std::cout << "[planning_trajectory_mock_subscriber] timeout reached, non-strict mode exits 0."
              << std::endl;
    return 0;
  }

  std::cout << "[planning_trajectory_mock_subscriber] done." << std::endl;
  return 0;
}
