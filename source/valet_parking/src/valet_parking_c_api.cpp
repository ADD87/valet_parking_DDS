#include "valet_parking_c_api.h"

#include "valet_parking_component.h"

#include <cstring>
#include <memory>
#include <new>
#include <string>

struct valet_parking_handle_t {
  std::unique_ptr<valet_parking::ValetParkingComponent> component;
  std::string local_error;
};

namespace {

valet_parking_config_t DefaultConfig() {
  valet_parking_config_t cfg{};
  cfg.domain_id = 0;
  cfg.input_topic_name = "/selected_slot";
  cfg.output_topic_name = "/planning/trajectory";
  cfg.command_topic_name = "/parking/command";
  cfg.localization_topic_name = "/localization/estimate";
  cfg.chassis_topic_name = "/chassis/state";
  cfg.obstacle_topic_name = "/perception/obstacles";
  cfg.enable_command_topic = 1U;
  cfg.enable_aux_input_topics = 1U;
  cfg.qos_depth = 10;
  cfg.max_parking_lots = 20;
  cfg.max_parking_lot_points = 8;
  cfg.max_path_points = 512;
  cfg.max_trajectory_points = 1024;
  cfg.fake_vehicle_x = 0.0;
  cfg.fake_vehicle_y = 0.0;
  cfg.fake_vehicle_theta = 0.0;
  return cfg;
}

bool IsValidConfig(const valet_parking_config_t& cfg) {
  if (cfg.qos_depth == 0) {
    return false;
  }
  if (cfg.max_parking_lots == 0 || cfg.max_trajectory_points == 0) {
    return false;
  }
  return true;
}

}  // namespace

extern "C" {

const char* valet_parking_get_api_version(void) {
  return "0.3.0-mvp";
}

int valet_parking_create(const valet_parking_config_t* config,
                         valet_parking_handle_t** out_handle) {
  if (out_handle == nullptr) {
    return VALET_PARKING_ERR_INVALID_ARG;
  }

  auto* handle = new (std::nothrow) valet_parking_handle_t();
  if (handle == nullptr) {
    return VALET_PARKING_ERR_RUNTIME;
  }

  valet_parking_config_t cfg = (config != nullptr) ? *config : DefaultConfig();
  if (!IsValidConfig(cfg)) {
    handle->local_error = "invalid config";
    delete handle;
    return VALET_PARKING_ERR_INVALID_ARG;
  }

  try {
    handle->component = std::make_unique<valet_parking::ValetParkingComponent>(cfg);
  } catch (...) {
    delete handle;
    return VALET_PARKING_ERR_RUNTIME;
  }

  *out_handle = handle;
  return VALET_PARKING_OK;
}

int valet_parking_start(valet_parking_handle_t* handle) {
  if (handle == nullptr || !handle->component) {
    return VALET_PARKING_ERR_INVALID_ARG;
  }
  return handle->component->Start();
}

int valet_parking_stop(valet_parking_handle_t* handle) {
  if (handle == nullptr || !handle->component) {
    return VALET_PARKING_ERR_INVALID_ARG;
  }
  return handle->component->Stop();
}

int valet_parking_update_vehicle_state(
    valet_parking_handle_t* handle,
    const valet_parking_vehicle_state_t* vehicle_state) {
  if (handle == nullptr || !handle->component || vehicle_state == nullptr) {
    return VALET_PARKING_ERR_INVALID_ARG;
  }
  return handle->component->UpdateVehicleState(*vehicle_state);
}

int valet_parking_clear_vehicle_state(valet_parking_handle_t* handle) {
  if (handle == nullptr || !handle->component) {
    return VALET_PARKING_ERR_INVALID_ARG;
  }
  return handle->component->ClearVehicleState();
}

int valet_parking_update_obstacles(
    valet_parking_handle_t* handle,
    const valet_parking_obstacle_t* obstacles,
    uint32_t obstacle_count) {
  if (handle == nullptr || !handle->component) {
    return VALET_PARKING_ERR_INVALID_ARG;
  }
  return handle->component->UpdateObstacles(obstacles, obstacle_count);
}

int valet_parking_clear_obstacles(valet_parking_handle_t* handle) {
  if (handle == nullptr || !handle->component) {
    return VALET_PARKING_ERR_INVALID_ARG;
  }
  return handle->component->ClearObstacles();
}

const char* valet_parking_get_last_error(valet_parking_handle_t* handle) {
  if (handle == nullptr || !handle->component) {
    return "invalid handle";
  }
  return handle->component->LastError().c_str();
}

void valet_parking_destroy(valet_parking_handle_t* handle) {
  if (handle == nullptr) {
    return;
  }

  if (handle->component) {
    (void)handle->component->Stop();
    handle->component.reset();
  }
  delete handle;
}

}  // extern "C"
