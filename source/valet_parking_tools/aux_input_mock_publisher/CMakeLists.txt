# Module metadata
set(MODULE_NAME aux_input_mock_publisher)
set(MODULE_DEPENDS thirdparty::magna-dds-core thirdparty::magna-dds-impl pthread)

# Source discovery
set(SOURCES
    main.cpp
    ../../valet_parking/generated/valet_parking_topics.cpp
    ../../valet_parking/generated/valet_parking_topicsTopicDataType.cpp
)

# Create executable
add_executable(${MODULE_NAME} ${SOURCES})
target_include_directories(${MODULE_NAME} PRIVATE ../../valet_parking/generated)

# Link dependencies
target_link_libraries(${MODULE_NAME} PRIVATE ${MODULE_DEPENDS})
target_compile_features(${MODULE_NAME} PRIVATE cxx_std_17)

# Standard install macro provided by middleware_compile
default_install()
