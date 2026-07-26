# Module metadata
set(MODULE_NAME valet_parking)
set(MODULE_INTERFACE include/valet_parking_c_api.h)
set(MODULE_DEPENDS thirdparty::magna-dds-core thirdparty::magna-dds-impl pthread)

# Source discovery
set(SOURCES
    src/valet_parking_component.cpp
    src/valet_parking_c_api.cpp
    generated/valet_parking_topics.cpp
    generated/valet_parking_topicsTopicDataType.cpp
)

# Include directories
include_directories(include src generated)

# Create shared library
add_library(${MODULE_NAME} SHARED ${SOURCES})
target_include_directories(${MODULE_NAME} PUBLIC include PRIVATE generated)

# Link dependencies
target_link_libraries(${MODULE_NAME} PRIVATE ${MODULE_DEPENDS})
target_compile_features(${MODULE_NAME} PRIVATE cxx_std_17)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${MODULE_NAME} PRIVATE -fvisibility=hidden)
endif()

# Standard install macro provided by middleware_compile
default_install()
