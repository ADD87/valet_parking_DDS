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
    uint32_t qos_depth;

    uint32_t max_parking_lots;
    uint32_t max_parking_lot_points;
    uint32_t max_path_points;
    uint32_t max_trajectory_points;

    double fake_vehicle_x;
    double fake_vehicle_y;
    double fake_vehicle_theta;
} valet_parking_config_t;

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

VALET_PARKING_API const char* valet_parking_get_last_error(valet_parking_handle_t* handle);

VALET_PARKING_API void valet_parking_destroy(valet_parking_handle_t* handle);

#ifdef __cplusplus
}
#endif
