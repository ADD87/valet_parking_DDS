# Module metadata
set(MODULE_NAME sample_sort)
set(MODULE_INTERFACE include/sort_utils.h)
set(MODULE_DEPENDS thirdparty::jsoncpp)

# Source discovery
set(SOURCES
    main.cpp
    src/sort_utils.cpp
)

# Include directories
include_directories(include)

# Create executable
add_executable(${MODULE_NAME} ${SOURCES})

# Link dependencies
target_link_libraries(${MODULE_NAME} PRIVATE ${MODULE_DEPENDS})

# Standard install macro provided by middleware_compile
default_install()
