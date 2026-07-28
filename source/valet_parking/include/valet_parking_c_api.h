#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
  #if defined(VALET_PARKING_BUILDING_DLL)
    #define VALET_PARKING_API __declspec(dllexport)
  #else
    #define VALET_PARKING_API __declspec(dllimport)
  #endif
#else
  #define VALET_PARKING_API __attribute__((visibility("default")))
#endif

typedef struct valet_parking_handle_t valet_parking_handle_t;

typedef struct valet_parking_config_t {
    uint32_t domain_id;
    const char* input_topic_name;
    const char* output_topic_name;
    const char* command_topic_name;
    const char* localization_topic_name;
    const char* chassis_topic_name;
    const char* obstacle_topic_name;
    uint32_t enable_command_topic;
    uint32_t enable_aux_input_topics;
    uint32_t qos_depth;

    uint32_t max_parking_lots;
    uint32_t max_parking_lot_points;
    uint32_t max_path_points;
    uint32_t max_trajectory_points;

    double fake_vehicle_x;
    double fake_vehicle_y;
    double fake_vehicle_theta;
} valet_parking_config_t;

typedef enum valet_parking_gear_position_t {
    VALET_PARKING_GEAR_NEUTRAL = 0,
    VALET_PARKING_GEAR_DRIVE = 1,
    VALET_PARKING_GEAR_REVERSE = 2,
    VALET_PARKING_GEAR_PARKING = 3,
    VALET_PARKING_GEAR_LOW = 4,
    VALET_PARKING_GEAR_INVALID = 5,
    VALET_PARKING_GEAR_NONE = 6
} valet_parking_gear_position_t;

typedef enum valet_parking_obstacle_type_t {
    VALET_PARKING_OBSTACLE_UNKNOWN = 0,
    VALET_PARKING_OBSTACLE_UNKNOWN_MOVABLE = 1,
    VALET_PARKING_OBSTACLE_UNKNOWN_UNMOVABLE = 2,
    VALET_PARKING_OBSTACLE_PEDESTRIAN = 3,
    VALET_PARKING_OBSTACLE_BICYCLE = 4,
    VALET_PARKING_OBSTACLE_VEHICLE = 5
} valet_parking_obstacle_type_t;

typedef struct valet_parking_vehicle_state_t {
    int32_t is_valid;
    uint64_t stamp_ms;
    double x;
    double y;
    double z;
    double heading;
    double linear_velocity;
    double linear_acceleration;
    valet_parking_gear_position_t gear;
} valet_parking_vehicle_state_t;

typedef struct valet_parking_obstacle_t {
    uint32_t id;
    valet_parking_obstacle_type_t type;
    int32_t is_dynamic;
    double center_x;
    double center_y;
    double heading;
    double length;
    double width;
    double velocity_x;
    double velocity_y;
} valet_parking_obstacle_t;

enum {
    VALET_PARKING_OK = 0,
    VALET_PARKING_ERR_INVALID_ARG = -1,
    VALET_PARKING_ERR_STATE = -2,
    VALET_PARKING_ERR_RUNTIME = -3,
};

VALET_PARKING_API const char* valet_parking_get_api_version(void);

VALET_PARKING_API int valet_parking_create(const valet_parking_config_t* config,
                                           valet_parking_handle_t** out_handle);

VALET_PARKING_API int valet_parking_start(valet_parking_handle_t* handle);

VALET_PARKING_API int valet_parking_stop(valet_parking_handle_t* handle);

VALET_PARKING_API int valet_parking_update_vehicle_state(
    valet_parking_handle_t* handle,
    const valet_parking_vehicle_state_t* vehicle_state);

VALET_PARKING_API int valet_parking_clear_vehicle_state(
    valet_parking_handle_t* handle);

VALET_PARKING_API int valet_parking_update_obstacles(
    valet_parking_handle_t* handle,
    const valet_parking_obstacle_t* obstacles,
    uint32_t obstacle_count);

VALET_PARKING_API int valet_parking_clear_obstacles(
    valet_parking_handle_t* handle);

VALET_PARKING_API const char* valet_parking_get_last_error(valet_parking_handle_t* handle);

VALET_PARKING_API void valet_parking_destroy(valet_parking_handle_t* handle);

#ifdef __cplusplus
}
#endif
