# Module metadata
set(MODULE_NAME valet_parking)
set(MODULE_INTERFACE include/valet_parking_c_api.h)
set(MODULE_DEPENDS thirdparty::magna-dds-core thirdparty::magna-dds-impl thirdparty::eigen pthread)

get_filename_component(STANDALONE_PARKING_ROOT
    "${APP_SOURCE_ROOT}/../../../../parking_algorithm_standalone"
    ABSOLUTE
)

if(NOT EXISTS "${STANDALONE_PARKING_ROOT}/planning/tasks/deciders/open_space_decider/open_space_roi_decider.h")
    message(FATAL_ERROR
        "[valet_parking] parking_algorithm_standalone not found at: ${STANDALONE_PARKING_ROOT}")
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

# Source discovery
set(SOURCES
    src/valet_parking_component.cpp
    src/valet_parking_stage_parking_adapter.cpp
    src/valet_parking_c_api.cpp
    generated/valet_parking_topics.cpp
    generated/valet_parking_topicsTopicDataType.cpp
    ${STANDALONE_ROI_SOURCES}
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

set_source_files_properties(${STANDALONE_ROI_SOURCES} PROPERTIES
    COMPILE_OPTIONS "-w;-include;limits"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${MODULE_NAME} PRIVATE -fvisibility=hidden)
endif()

# Standard install macro provided by middleware_compile
default_install()
