# Module metadata
set(MODULE_NAME planning_trajectory_mock_subscriber)
set(MODULE_DEPENDS thirdparty::magna-dds-core pthread)

# Source discovery
set(SOURCES
    main.cpp
)

# Create executable
add_executable(${MODULE_NAME} ${SOURCES})

# Link dependencies
target_link_libraries(${MODULE_NAME} PRIVATE ${MODULE_DEPENDS})
target_compile_features(${MODULE_NAME} PRIVATE cxx_std_17)

# Standard install macro provided by middleware_compile
default_install()
