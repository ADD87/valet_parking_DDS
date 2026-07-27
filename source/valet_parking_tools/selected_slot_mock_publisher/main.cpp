#include "magnadds/MagnaDDS.h"
#include "valet_parking_topics.h"
#include "valet_parking_topicsTopicDataType.h"

#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <thread>

namespace {

struct PublisherOptions {
  uint32_t domain_id{0U};
  std::string topic_name{"/selected_slot"};
  std::string mode{"valid"};
  uint32_t count{3U};
  uint32_t interval_ms{100U};
};

constexpr uint32_t kTargetMovesStartIndex = 3U;
constexpr uint32_t kParkingSeqChangesStartIndex = 3U;

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
  return mode == "valid" || mode == "empty" || mode == "overflow" ||
         mode == "nan" || mode == "degenerate-corners" ||
         mode == "target-moves" || mode == "parking-seq-changes";
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

Header BuildHeader(const PublisherOptions& options, uint32_t index) {
  const uint64_t stamp_ms = NowMilliseconds();
  const uint64_t seq = stamp_ms + static_cast<uint64_t>(index);

  Header header;
  header.seq(seq);
  header.frame_id("selected_slot_mock_publisher");
  header.publish_stamp_ms(stamp_ms);
  header.data_stamp_ms(stamp_ms);
  return header;
}

Point3D BuildPoint(double x, double y, double z) {
  Point3D point;
  point.x(x);
  point.y(y);
  point.z(z);
  return point;
}

PsPoint BuildPsPoint(double x, double y, PsPointPosition position) {
  PsPoint point;
  point.point(BuildPoint(x, y, 0.0));
  point.position(position);
  point.quality(PsPointQuality::PS_QUALITY_HIGH);
  return point;
}

std::vector<PsPoint> BuildSlotCorners(double center_x,
                                      double center_y,
                                      double heading,
                                      double length,
                                      double width) {
  const double half_length = length * 0.5;
  const double half_width = width * 0.5;
  const double cos_heading = std::cos(heading);
  const double sin_heading = std::sin(heading);

  const double local_x[4] = {-half_length, half_length, -half_length, half_length};
  const double local_y[4] = {half_width, half_width, -half_width, -half_width};
  const PsPointPosition positions[4] = {
      PsPointPosition::PS_POSITION_TOP_LEFT,
      PsPointPosition::PS_POSITION_TOP_RIGHT,
      PsPointPosition::PS_POSITION_BOTTOM_LEFT,
      PsPointPosition::PS_POSITION_BOTTOM_RIGHT,
  };

  std::vector<PsPoint> corners;
  corners.reserve(4U);
  for (std::size_t i = 0U; i < 4U; ++i) {
    const double x = center_x + local_x[i] * cos_heading - local_y[i] * sin_heading;
    const double y = center_y + local_x[i] * sin_heading + local_y[i] * cos_heading;
    corners.push_back(BuildPsPoint(x, y, positions[i]));
  }
  return corners;
}

uint32_t BuildParkingSeq(const PublisherOptions& options, uint32_t index) {
  if (options.mode == "parking-seq-changes" &&
      index >= kParkingSeqChangesStartIndex) {
    return 2U;
  }
  return 1U;
}

ParkingLot BuildParkingLot(const PublisherOptions& options, uint32_t index) {
  const double center_x = (options.mode == "nan")
      ? std::numeric_limits<double>::quiet_NaN()
      : ((options.mode == "target-moves" &&
          index >= kTargetMovesStartIndex)
             ? 9.3
             : 8.5);
  const double center_y = 2.0;
  const double heading = 0.2;
  const double length = 5.2;
  const double width = 2.4;
  std::vector<PsPoint> corners;
  if (options.mode == "degenerate-corners") {
    corners = {
        BuildPsPoint(center_x, center_y, PsPointPosition::PS_POSITION_TOP_LEFT),
        BuildPsPoint(center_x, center_y, PsPointPosition::PS_POSITION_TOP_RIGHT),
        BuildPsPoint(center_x, center_y, PsPointPosition::PS_POSITION_BOTTOM_LEFT),
        BuildPsPoint(center_x, center_y, PsPointPosition::PS_POSITION_BOTTOM_RIGHT),
    };
  } else {
    corners = BuildSlotCorners(center_x, center_y, heading, length, width);
  }

  ParkingLot lot;
  lot.parking_seq(BuildParkingSeq(options, index));
  lot.type(ParkingType::PARKING_TYPE_VERTICAL);
  lot.status(ParkingStatus::PARKING_STATUS_FREE);
  lot.sensor_type(ParkingSensorType::PARKING_SENSOR_CAMERA);
  lot.is_private_ps(false);
  lot.pts_vrf(corners);
  lot.time_creation(static_cast<double>(NowMilliseconds()) / 1000.0);
  lot.pts_enu(corners);
  lot.hmi_angle(heading);
  lot.hmi_depth(length);
  lot.hmi_width(width);
  lot.hmi_direction(0.0);
  lot.hmi_type(0.0);
  lot.hmi_status(0.0);
  lot.park_size(ParkingSpaceSize::PARKING_SPACE_SIZE_NORMAL);
  return lot;
}

SelectedSlot BuildSample(const PublisherOptions& options, uint32_t index) {
  SelectedSlot sample;
  sample.header(BuildHeader(options, index));
  sample.loc_seq(index);
  sample.opt_parking_seq(BuildParkingSeq(options, index));
  sample.path_point_size(0U);
  sample.traced_path(std::vector<ParkingPathPoint>{});
  sample.hpp_cruising_to_parking(false);
  sample.enable_trigger(true);

  if (options.mode == "empty") {
    sample.count(0U);
    sample.parking_lots(std::vector<ParkingLot>{});
    sample.is_valid(false);
    return sample;
  }

  std::vector<ParkingLot> lots;
  lots.push_back(BuildParkingLot(options, index));
  sample.parking_lots(lots);
  sample.count(options.mode == "overflow" ? 999999U : static_cast<uint32_t>(lots.size()));
  sample.is_valid(true);
  return sample;
}

void PrintUsage() {
  std::cout << "Usage: selected_slot_mock_publisher [options]\n"
            << "Options:\n"
            << "  --domain-id=<uint32>          DDS domain id (default 0)\n"
            << "  --topic=<name>                topic name (default /selected_slot)\n"
            << "  --mode=<valid|empty|overflow|nan|degenerate-corners|target-moves|parking-seq-changes>\n"
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

  SelectedSlotTopicDataType selected_slot_type;
  magna::dds::ReturnCode_t rc = participant->register_type(&selected_slot_type);
  if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
    std::cerr << "[selected_slot_mock_publisher] register_type failed: "
              << ReturnCodeToString(rc) << std::endl;
    cleanup();
    return 4;
  }

  magna::dds::Topic* topic = participant->create_topic(
      options.topic_name, selected_slot_type.get_name(), magna::dds::TOPIC_QOS_DEFAULT);
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
    SelectedSlot sample = BuildSample(options, i);
    rc = writer->write(&sample, magna::dds::HANDLE_NIL);
    if (rc != magna::dds::ReturnCode_t::RETCODE_OK) {
      std::cerr << "[selected_slot_mock_publisher] write failed at sample " << i
                << ": " << ReturnCodeToString(rc) << std::endl;
      cleanup();
      return 5;
    }

    std::cout << "[selected_slot_mock_publisher] published sample " << (i + 1U)
              << "/" << options.count
              << " count=" << sample.count()
              << " lots=" << sample.parking_lots().size()
              << " is_valid=" << (sample.is_valid() ? "true" : "false")
              << " parking_seq=" << sample.opt_parking_seq()
              << " target="
              << (options.mode == "target-moves" &&
                  i >= kTargetMovesStartIndex ? "moved" : "base")
              << std::endl;

    if (i + 1U < options.count && options.interval_ms > 0U) {
      std::this_thread::sleep_for(std::chrono::milliseconds(options.interval_ms));
    }
  }

  cleanup();
  std::cout << "[selected_slot_mock_publisher] done." << std::endl;
  return 0;
}
