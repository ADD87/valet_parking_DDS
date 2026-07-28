# Module metadata
set(MODULE_NAME valet_parking)
set(MODULE_INTERFACE include/valet_parking_c_api.h)
set(MODULE_DEPENDS thirdparty::magna-dds-core thirdparty::magna-dds-impl thirdparty::eigen pthread)

get_filename_component(STANDALONE_PARKING_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/algorithm/parking_algorithm_standalone"
    ABSOLUTE
)

if(NOT EXISTS "${STANDALONE_PARKING_ROOT}/planning/tasks/deciders/open_space_decider/open_space_roi_decider.h")
    message(FATAL_ERROR
        "[valet_parking] local parking_algorithm_standalone subset not found at: ${STANDALONE_PARKING_ROOT}")
endif()

set(STANDALONE_ROI_SOURCES
    ${STANDALONE_PARKING_ROOT}/common/math/aabox2d.cc
    ${STANDALONE_PARKING_ROOT}/common/math/angle.cc
    ${STANDALONE_PARKING_ROOT}/common/math/sin_table.cc
    ${STANDALONE_PARKING_ROOT}/common/math/atan_table.cc
    ${STANDALONE_PARKING_ROOT}/common/math/box2d.cc
    ${STANDALONE_PARKING_ROOT}/common/math/cartesian_frenet_conversion.cc
    ${STANDALONE_PARKING_ROOT}/common/math/line_segment2d.cc
    ${STANDALONE_PARKING_ROOT}/common/math/math_utils.cc
    ${STANDALONE_PARKING_ROOT}/common/math/polygon2d.cc
    ${STANDALONE_PARKING_ROOT}/common/math/search.cc
    ${STANDALONE_PARKING_ROOT}/common/math/vec2d.cc
    ${STANDALONE_PARKING_ROOT}/common/math/linear_interpolation.cc
    ${STANDALONE_PARKING_ROOT}/common/configs/vehicle_config_helper_simple.cc
    ${STANDALONE_PARKING_ROOT}/planning/tasks/deciders/open_space_decider/open_space_roi_decider.cc
    ${STANDALONE_PARKING_ROOT}/planning/tasks/deciders/open_space_decider/open_space_obstacle.cc
    ${STANDALONE_PARKING_ROOT}/planning/tasks/deciders/open_space_decider/open_space_fine_tuning.cc
)

set(STANDALONE_PATH_PROVIDER_SOURCES
    ${STANDALONE_PARKING_ROOT}/common/math/path_matcher.cc
    ${STANDALONE_PARKING_ROOT}/planning/common/path/discretized_path.cc
    ${STANDALONE_PARKING_ROOT}/planning/open_space/coarse_path_generator/node3d.cc
    ${STANDALONE_PARKING_ROOT}/planning/open_space/coarse_path_generator/path_generator.cc
    ${STANDALONE_PARKING_ROOT}/planning/open_space/coarse_path_generator/shape/reeds_shepp_path.cc
    ${STANDALONE_PARKING_ROOT}/planning/open_space/coarse_path_generator/hybrid_a_star.cc
    ${STANDALONE_PARKING_ROOT}/planning/open_space/open_space_thread/open_space_thread_manager.cc
    ${STANDALONE_PARKING_ROOT}/planning/tasks/optimizers/open_space_path_generation/open_space_path_generator.cc
    ${STANDALONE_PARKING_ROOT}/planning/tasks/optimizers/open_space_path_generation/open_space_path_provider.cc
)

set(STANDALONE_PATH_PARTITION_SOURCES
    ${STANDALONE_PARKING_ROOT}/planning/tasks/optimizers/open_space_path_partition/open_space_path_partition.cc
)

set(STANDALONE_SPEED_OPTIMIZER_SOURCES
    ${STANDALONE_PARKING_ROOT}/planning/common/speed/speed_data.cc
    ${STANDALONE_PARKING_ROOT}/planning/common/trajectory/discretized_trajectory.cc
    ${STANDALONE_PARKING_ROOT}/planning/common/trajectory1d/piecewise_acceleration_trajectory1d.cc
    ${STANDALONE_PARKING_ROOT}/planning/tasks/optimizers/open_space_speed_optimizer/path_handle.cc
    ${STANDALONE_PARKING_ROOT}/planning/tasks/optimizers/open_space_speed_optimizer/st_sample_cost.cc
    ${STANDALONE_PARKING_ROOT}/planning/tasks/optimizers/open_space_speed_optimizer/st_sample_curves.cc
    ${STANDALONE_PARKING_ROOT}/planning/tasks/optimizers/open_space_speed_optimizer/open_space_speed_optimizer.cc
)

set(STANDALONE_STRAIGHT_PATH_SOURCES
    ${STANDALONE_PARKING_ROOT}/planning/tasks/optimizers/open_space_straight_path/open_space_straight_path_provider.cc
)

# Source discovery
set(SOURCES
    src/valet_parking_component.cpp
    src/valet_parking_stage_parking_adapter.cpp
    src/valet_parking_c_api.cpp
    generated/valet_parking_topics.cpp
    generated/valet_parking_topicsTopicDataType.cpp
    ${STANDALONE_ROI_SOURCES}
    ${STANDALONE_PATH_PROVIDER_SOURCES}
    ${STANDALONE_PATH_PARTITION_SOURCES}
    ${STANDALONE_SPEED_OPTIMIZER_SOURCES}
    ${STANDALONE_STRAIGHT_PATH_SOURCES}
)

# Include directories
include_directories(BEFORE include src src/compat generated)

# Create shared library
add_library(${MODULE_NAME} SHARED ${SOURCES})
target_include_directories(${MODULE_NAME}
    PUBLIC include
    BEFORE PRIVATE src src/compat generated
)
target_include_directories(${MODULE_NAME}
    SYSTEM PRIVATE ${STANDALONE_PARKING_ROOT}
)

# Link dependencies
target_link_libraries(${MODULE_NAME} PRIVATE ${MODULE_DEPENDS})
target_compile_features(${MODULE_NAME} PRIVATE cxx_std_17)

set_source_files_properties(${STANDALONE_ROI_SOURCES} ${STANDALONE_PATH_PROVIDER_SOURCES} ${STANDALONE_PATH_PARTITION_SOURCES} ${STANDALONE_SPEED_OPTIMIZER_SOURCES} ${STANDALONE_STRAIGHT_PATH_SOURCES} PROPERTIES
    COMPILE_OPTIONS "-w;-include;limits"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${MODULE_NAME} PRIVATE -fvisibility=hidden)
endif()

# Standard install macro provided by middleware_compile
default_install()
