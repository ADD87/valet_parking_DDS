# Module metadata
set(MODULE_NAME sample_math)
set(MODULE_INTERFACE include/math_utils.h)
set(MODULE_DEPENDS thirdparty::log thirdparty::nlohmann pthread)

# Source discovery
set(SOURCES
    main.cpp
    src/math_utils.cpp
)

# Include directories
include_directories(include)

# Create executable
add_executable(${MODULE_NAME} ${SOURCES})

# Link dependencies
target_link_libraries(${MODULE_NAME} PRIVATE ${MODULE_DEPENDS})

# Standard install macro provided by middleware_compile
default_install()
