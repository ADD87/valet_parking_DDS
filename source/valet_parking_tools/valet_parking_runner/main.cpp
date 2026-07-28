#include "valet_parking_c_api.h"

#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <limits>
#include <string>
#include <thread>

namespace {

volatile std::sig_atomic_t g_stop_requested = 0;

void OnSignal(int) {
  g_stop_requested = 1;
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

void PrintUsage() {
  std::cout << "Usage: valet_parking_runner [options]\n"
            << "Options:\n"
            << "  --domain-id=<uint32>          DDS domain id (default 0)\n"
            << "  --in-topic=<name>             input topic name (default /selected_slot)\n"
            << "  --out-topic=<name>            output topic name (default /planning/trajectory)\n"
            << "  --command-topic=<name>        parking command input topic (default /parking/command)\n"
            << "  --disable-command-topic       do not subscribe parking command topic\n"
            << "  --localization-topic=<name>   localization input topic (default /localization/estimate)\n"
            << "  --chassis-topic=<name>        chassis input topic (default /chassis/state)\n"
            << "  --obstacle-topic=<name>       obstacle input topic (default /perception/obstacles)\n"
            << "  --disable-aux-input-topics    do not subscribe localization/chassis/obstacle topics\n"
            << "  --help                        show this message\n";
}

}  // namespace

int main(int argc, char* argv[]) {
  std::string in_topic = "/selected_slot";
  std::string out_topic = "/planning/trajectory";
  std::string command_topic = "/parking/command";
  std::string localization_topic = "/localization/estimate";
  std::string chassis_topic = "/chassis/state";
  std::string obstacle_topic = "/perception/obstacles";
  bool enable_command_topic = true;
  bool enable_aux_input_topics = true;
  uint32_t domain_id = 0U;

  for (int i = 1; i < argc; ++i) {
    const std::string arg(argv[i]);
    if (arg == "--help") {
      PrintUsage();
      return 0;
    }

    const std::string domain_prefix = "--domain-id=";
    const std::string in_prefix = "--in-topic=";
    const std::string out_prefix = "--out-topic=";
    const std::string command_prefix = "--command-topic=";
    const std::string localization_prefix = "--localization-topic=";
    const std::string chassis_prefix = "--chassis-topic=";
    const std::string obstacle_prefix = "--obstacle-topic=";

    if (arg.rfind(domain_prefix, 0) == 0) {
      const std::string value = arg.substr(domain_prefix.size());
      if (!ParseUint32(value, &domain_id)) {
        std::cerr << "[valet_parking_runner] invalid --domain-id: " << value << std::endl;
        return 2;
      }
      continue;
    }

    if (arg.rfind(in_prefix, 0) == 0) {
      in_topic = arg.substr(in_prefix.size());
      continue;
    }

    if (arg.rfind(out_prefix, 0) == 0) {
      out_topic = arg.substr(out_prefix.size());
      continue;
    }

    if (arg.rfind(command_prefix, 0) == 0) {
      command_topic = arg.substr(command_prefix.size());
      continue;
    }

    if (arg.rfind(localization_prefix, 0) == 0) {
      localization_topic = arg.substr(localization_prefix.size());
      continue;
    }

    if (arg.rfind(chassis_prefix, 0) == 0) {
      chassis_topic = arg.substr(chassis_prefix.size());
      continue;
    }

    if (arg.rfind(obstacle_prefix, 0) == 0) {
      obstacle_topic = arg.substr(obstacle_prefix.size());
      continue;
    }

    if (arg == "--disable-command-topic") {
      enable_command_topic = false;
      continue;
    }

    if (arg == "--disable-aux-input-topics") {
      enable_aux_input_topics = false;
      continue;
    }

    std::cerr << "[valet_parking_runner] unknown argument: " << arg << std::endl;
    PrintUsage();
    return 2;
  }

  valet_parking_config_t config{};
  config.domain_id = domain_id;
  config.input_topic_name = in_topic.c_str();
  config.output_topic_name = out_topic.c_str();
  config.command_topic_name = command_topic.c_str();
  config.localization_topic_name = localization_topic.c_str();
  config.chassis_topic_name = chassis_topic.c_str();
  config.obstacle_topic_name = obstacle_topic.c_str();
  config.enable_command_topic = enable_command_topic ? 1U : 0U;
  config.enable_aux_input_topics = enable_aux_input_topics ? 1U : 0U;
  config.qos_depth = 10U;
  config.max_parking_lots = 20U;
  config.max_parking_lot_points = 8U;
  config.max_path_points = 512U;
  config.max_trajectory_points = 1024U;
  config.fake_vehicle_x = 0.0;
  config.fake_vehicle_y = 0.0;
  config.fake_vehicle_theta = 0.0;

  valet_parking_handle_t* handle = nullptr;
  const int create_ret = valet_parking_create(&config, &handle);
  if (create_ret != VALET_PARKING_OK || handle == nullptr) {
    std::cerr << "[valet_parking_runner] create failed, ret=" << create_ret << std::endl;
    return 3;
  }

  const int start_ret = valet_parking_start(handle);
  if (start_ret != VALET_PARKING_OK) {
    std::cerr << "[valet_parking_runner] start failed, ret=" << start_ret
              << ", err=" << valet_parking_get_last_error(handle) << std::endl;
    valet_parking_destroy(handle);
    return 4;
  }

  std::signal(SIGINT, OnSignal);
  std::signal(SIGTERM, OnSignal);

  std::cout << "[valet_parking_runner] started, press Ctrl+C to stop." << std::endl;

  while (g_stop_requested == 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  (void)valet_parking_stop(handle);
  valet_parking_destroy(handle);
  std::cout << "[valet_parking_runner] exited." << std::endl;
  return 0;
}
