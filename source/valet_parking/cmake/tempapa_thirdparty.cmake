# TempAPA third-party dependency bridge for the standalone parking algorithms.
#
# TempAPA is packaged per target architecture under thirdparty/.  The source
# checkout is a reference only; it must not become an implicit build input.
# A developer may still override the package with -DTEMPAPA_LIB_ROOT=... or
# the TEMPAPA_LIB_ROOT environment variable.

if(NOT DEFINED TEMPAPA_LIB_ROOT OR "${TEMPAPA_LIB_ROOT}" STREQUAL "")
    if(DEFINED ENV{TEMPAPA_LIB_ROOT} AND NOT "$ENV{TEMPAPA_LIB_ROOT}" STREQUAL "")
        set(TEMPAPA_LIB_ROOT "$ENV{TEMPAPA_LIB_ROOT}")
    endif()
endif()

if(NOT TEMPAPA_LIB_ROOT)
    if(NOT DEFINED CMAKE_CROSS_BUILD OR "${CMAKE_CROSS_BUILD}" STREQUAL "")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
            set(CMAKE_CROSS_BUILD "m57")
        else()
            set(CMAKE_CROSS_BUILD "x86")
        endif()
    endif()

    if(NOT CMAKE_CROSS_BUILD MATCHES "^(x86|m57)$")
        message(FATAL_ERROR
            "[valet_parking] Unsupported TempAPA target '${CMAKE_CROSS_BUILD}'. "
            "Expected x86 or m57.")
    endif()

    # CMAKE_SOURCE_DIR = compile/ ; thirdparty is at ../thirdparty
    set(TEMPAPA_LIB_ROOT
        "${CMAKE_SOURCE_DIR}/../thirdparty/${CMAKE_CROSS_BUILD}/tempapa")
endif()

if(NOT TEMPAPA_LIB_ROOT)
    message(FATAL_ERROR
        "[valet_parking] TempAPA third-party root was not found. "
        "Provide -DTEMPAPA_LIB_ROOT=/path/to/package or set TEMPAPA_LIB_ROOT. ")
endif()

get_filename_component(TEMPAPA_LIB_ROOT "${TEMPAPA_LIB_ROOT}" REALPATH)
set(TEMPAPA_INSTALL_ROOT "${TEMPAPA_LIB_ROOT}/local")
set(TEMPAPA_INCLUDE_ROOT "${TEMPAPA_INSTALL_ROOT}/include")
set(TEMPAPA_LIBRARY_ROOT "${TEMPAPA_INSTALL_ROOT}/lib")

if(NOT EXISTS "${TEMPAPA_INCLUDE_ROOT}" OR NOT EXISTS "${TEMPAPA_LIBRARY_ROOT}")
    message(FATAL_ERROR
        "[valet_parking] TempAPA third-party root is incomplete: ${TEMPAPA_LIB_ROOT}. "
        "Expected local/include and local/lib.")
endif()

set(TEMPAPA_SOLVER_RUNTIME_DEFAULT OFF)
if(EXISTS "${TEMPAPA_LIBRARY_ROOT}/libipopt.so" AND
   EXISTS "${TEMPAPA_LIBRARY_ROOT}/libcoinmumps.so" AND
   EXISTS "${TEMPAPA_LIBRARY_ROOT}/libblas.so" AND
   EXISTS "${TEMPAPA_LIBRARY_ROOT}/liblapack.so" AND
   EXISTS "${TEMPAPA_LIBRARY_ROOT}/libgfortran.so.5")
    set(TEMPAPA_SOLVER_RUNTIME_DEFAULT ON)
endif()
option(TEMPAPA_ENABLE_SOLVER_RUNTIME
    "Link the target-compatible CppAD/Ipopt/MUMPS TempAPA solver runtime"
    ${TEMPAPA_SOLVER_RUNTIME_DEFAULT})
option(TEMPAPA_ENABLE_NLP_SMOOTHER
    "Compile the parking-stage lite NLP smoother when solver runtime is available"
    ${TEMPAPA_SOLVER_RUNTIME_DEFAULT})

# The checked-in Temp_Lib binaries are currently x86-64.  Never allow CMake
# to silently link them into the m57/aarch64 target.
find_program(TEMPAPA_FILE_COMMAND file)
function(tempapa_validate_library_architecture LIBRARY_PATH)
    if(NOT TEMPAPA_FILE_COMMAND OR NOT EXISTS "${LIBRARY_PATH}")
        return()
    endif()

    execute_process(
        COMMAND "${TEMPAPA_FILE_COMMAND}" -L -b "${LIBRARY_PATH}"
        OUTPUT_VARIABLE TEMPAPA_FILE_DESCRIPTION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
        if(NOT TEMPAPA_FILE_DESCRIPTION MATCHES "x86-64")
            message(FATAL_ERROR
                "[valet_parking] TempAPA library architecture mismatch: ${LIBRARY_PATH}\n"
                "Target processor: ${CMAKE_SYSTEM_PROCESSOR}\n"
                "Detected: ${TEMPAPA_FILE_DESCRIPTION}")
        endif()
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
        if(NOT TEMPAPA_FILE_DESCRIPTION MATCHES "(aarch64|AArch64)")
            message(FATAL_ERROR
                "[valet_parking] TempAPA library architecture mismatch: ${LIBRARY_PATH}\n"
                "Target processor: ${CMAKE_SYSTEM_PROCESSOR}\n"
                "Detected: ${TEMPAPA_FILE_DESCRIPTION}\n"
                "Use an aarch64 Temp_Lib build for m57; host x86-64 binaries are not valid.")
        endif()
    endif()
endfunction()

function(tempapa_define_interface TARGET_NAME)
    if(NOT TARGET "${TARGET_NAME}")
        add_library("${TARGET_NAME}" INTERFACE IMPORTED GLOBAL)
    endif()
    set_target_properties("${TARGET_NAME}" PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ARGN}")
endfunction()

function(tempapa_define_shared TARGET_NAME LIBRARY_PATH)
    if(NOT TARGET "${TARGET_NAME}")
        add_library("${TARGET_NAME}" SHARED IMPORTED GLOBAL)
    endif()
    set_target_properties("${TARGET_NAME}" PROPERTIES
        IMPORTED_LOCATION "${LIBRARY_PATH}"
        INTERFACE_LINK_DIRECTORIES "${TEMPAPA_LIBRARY_ROOT}"
        INTERFACE_LINK_OPTIONS "-Wl,-rpath-link,${TEMPAPA_LIBRARY_ROOT}")
endfunction()

# Header-only/source packages.
if(EXISTS "${TEMPAPA_LIB_ROOT}/eigen-3.4.0/Eigen/Core")
    set(TEMPAPA_EIGEN_INCLUDE_DIR "${TEMPAPA_LIB_ROOT}/eigen-3.4.0")
elseif(EXISTS "${TEMPAPA_LIB_ROOT}/eigen/Eigen/Core")
    set(TEMPAPA_EIGEN_INCLUDE_DIR "${TEMPAPA_LIB_ROOT}/eigen")
endif()

if(EXISTS "${TEMPAPA_LIB_ROOT}/boost_1_76_0/boost/version.hpp")
    set(TEMPAPA_BOOST_INCLUDE_DIR "${TEMPAPA_LIB_ROOT}/boost_1_76_0")
elseif(EXISTS "${TEMPAPA_LIB_ROOT}/boost/boost/version.hpp")
    set(TEMPAPA_BOOST_INCLUDE_DIR "${TEMPAPA_LIB_ROOT}/boost")
endif()

if(EXISTS "${TEMPAPA_LIB_ROOT}/abseil-cpp/absl/strings/str_cat.h")
    set(TEMPAPA_ABSEIL_INCLUDE_DIR "${TEMPAPA_LIB_ROOT}/abseil-cpp")
endif()

if(EXISTS "${TEMPAPA_INCLUDE_ROOT}/cppad/cppad.hpp")
    set(TEMPAPA_CPPAD_INCLUDE_DIR "${TEMPAPA_INCLUDE_ROOT}")
elseif(EXISTS "${TEMPAPA_LIB_ROOT}/CppAD/include/cppad/cppad.hpp")
    set(TEMPAPA_CPPAD_INCLUDE_DIR "${TEMPAPA_LIB_ROOT}/CppAD/include")
endif()

foreach(TEMPAPA_REQUIRED_INCLUDE
        TEMPAPA_EIGEN_INCLUDE_DIR
        TEMPAPA_BOOST_INCLUDE_DIR
        TEMPAPA_ABSEIL_INCLUDE_DIR
        TEMPAPA_CPPAD_INCLUDE_DIR)
    if(NOT DEFINED ${TEMPAPA_REQUIRED_INCLUDE})
        message(FATAL_ERROR
            "[valet_parking] TempAPA third-party header is missing: ${TEMPAPA_REQUIRED_INCLUDE}. "
            "Root: ${TEMPAPA_LIB_ROOT}")
    endif()
endforeach()

# Binary packages.  Clearing the cache avoids retaining a path from an older
# checkout after TEMPAPA_LIB_ROOT is changed.
foreach(TEMPAPA_LIBRARY_CACHE
        TEMPAPA_OSQP_LIBRARY
        TEMPAPA_CPPAD_LIBRARY
        TEMPAPA_IPOPT_LIBRARY
        TEMPAPA_COINMUMPS_LIBRARY
        TEMPAPA_COINASL_LIBRARY
        TEMPAPA_IPOPT_AMPL_LIBRARY
        TEMPAPA_BLAS_LIBRARY
        TEMPAPA_LAPACK_LIBRARY
        TEMPAPA_GFORTRAN_LIBRARY
        TEMPAPA_QDLDL_LIBRARY)
    unset(${TEMPAPA_LIBRARY_CACHE} CACHE)
    unset(${TEMPAPA_LIBRARY_CACHE})
endforeach()

set(TEMPAPA_OSQP_LIBRARY "${TEMPAPA_LIBRARY_ROOT}/libosqp.so")
if(NOT EXISTS "${TEMPAPA_OSQP_LIBRARY}")
    unset(TEMPAPA_OSQP_LIBRARY)
endif()
if(TEMPAPA_ENABLE_SOLVER_RUNTIME)
    set(TEMPAPA_CPPAD_LIBRARY "${TEMPAPA_LIBRARY_ROOT}/libcppad_lib.so")
    set(TEMPAPA_IPOPT_LIBRARY "${TEMPAPA_LIBRARY_ROOT}/libipopt.so")
    set(TEMPAPA_COINMUMPS_LIBRARY "${TEMPAPA_LIBRARY_ROOT}/libcoinmumps.so")
    set(TEMPAPA_COINASL_LIBRARY "${TEMPAPA_LIBRARY_ROOT}/libcoinasl.so")
    set(TEMPAPA_IPOPT_AMPL_LIBRARY "${TEMPAPA_LIBRARY_ROOT}/libipoptamplinterface.so")
    set(TEMPAPA_BLAS_LIBRARY "${TEMPAPA_LIBRARY_ROOT}/libblas.so")
    set(TEMPAPA_LAPACK_LIBRARY "${TEMPAPA_LIBRARY_ROOT}/liblapack.so")
    set(TEMPAPA_OPENBLAS_LIBRARY "${TEMPAPA_LIBRARY_ROOT}/libopenblas.so.0")
    set(TEMPAPA_GFORTRAN_LIBRARY "${TEMPAPA_LIBRARY_ROOT}/libgfortran.so.5")
    foreach(TEMPAPA_SOLVER_LIBRARY
            TEMPAPA_CPPAD_LIBRARY
            TEMPAPA_IPOPT_LIBRARY
            TEMPAPA_COINMUMPS_LIBRARY
            TEMPAPA_COINASL_LIBRARY
            TEMPAPA_IPOPT_AMPL_LIBRARY)
        if(NOT EXISTS "${${TEMPAPA_SOLVER_LIBRARY}}")
            unset(${TEMPAPA_SOLVER_LIBRARY})
        endif()
    endforeach()
endif()
set(TEMPAPA_QDLDL_LIBRARY "${TEMPAPA_LIBRARY_ROOT}/libqdldl.so")
if(NOT EXISTS "${TEMPAPA_QDLDL_LIBRARY}")
    unset(TEMPAPA_QDLDL_LIBRARY)
endif()

if(EXISTS "${TEMPAPA_INCLUDE_ROOT}/osqp/osqp.h")
    set(TEMPAPA_OSQP_INCLUDE_DIR "${TEMPAPA_INCLUDE_ROOT}")
endif()
if(EXISTS "${TEMPAPA_INCLUDE_ROOT}/coin/IpTNLP.hpp" OR
   EXISTS "${TEMPAPA_INCLUDE_ROOT}/coin-or/IpTNLP.hpp")
    set(TEMPAPA_IPOPT_INCLUDE_DIR "${TEMPAPA_INCLUDE_ROOT}")
endif()

set(TEMPAPA_REQUIRED_PATHS
    TEMPAPA_OSQP_INCLUDE_DIR
    TEMPAPA_OSQP_LIBRARY)
if(TEMPAPA_ENABLE_SOLVER_RUNTIME)
    list(APPEND TEMPAPA_REQUIRED_PATHS
        TEMPAPA_IPOPT_INCLUDE_DIR
        TEMPAPA_IPOPT_LIBRARY
        TEMPAPA_CPPAD_LIBRARY
        TEMPAPA_COINMUMPS_LIBRARY
        TEMPAPA_BLAS_LIBRARY
        TEMPAPA_LAPACK_LIBRARY
        TEMPAPA_GFORTRAN_LIBRARY)
endif()

foreach(TEMPAPA_REQUIRED_PATH IN LISTS TEMPAPA_REQUIRED_PATHS)
    if(NOT DEFINED ${TEMPAPA_REQUIRED_PATH} OR
       "${${TEMPAPA_REQUIRED_PATH}}" STREQUAL "")
        message(FATAL_ERROR
            "[valet_parking] Required TempAPA dependency is missing: ${TEMPAPA_REQUIRED_PATH}. "
            "Root: ${TEMPAPA_LIB_ROOT}")
    endif()
endforeach()

foreach(TEMPAPA_RUNTIME_LIBRARY
        TEMPAPA_OSQP_LIBRARY
        TEMPAPA_CPPAD_LIBRARY
        TEMPAPA_IPOPT_LIBRARY
        TEMPAPA_COINMUMPS_LIBRARY
        TEMPAPA_COINASL_LIBRARY
        TEMPAPA_IPOPT_AMPL_LIBRARY
        TEMPAPA_BLAS_LIBRARY
        TEMPAPA_LAPACK_LIBRARY
        TEMPAPA_GFORTRAN_LIBRARY
        TEMPAPA_QDLDL_LIBRARY)
    if(DEFINED ${TEMPAPA_RUNTIME_LIBRARY} AND
       NOT "${${TEMPAPA_RUNTIME_LIBRARY}}" STREQUAL "")
        tempapa_validate_library_architecture("${${TEMPAPA_RUNTIME_LIBRARY}}")
    endif()
endforeach()

tempapa_define_interface(thirdparty::tempapa-eigen
    "${TEMPAPA_EIGEN_INCLUDE_DIR}")
tempapa_define_interface(thirdparty::tempapa-boost
    "${TEMPAPA_BOOST_INCLUDE_DIR}")
tempapa_define_interface(thirdparty::tempapa-abseil
    "${TEMPAPA_ABSEIL_INCLUDE_DIR}")
tempapa_define_interface(thirdparty::tempapa-cppad
    "${TEMPAPA_CPPAD_INCLUDE_DIR}")

# Imported shared libraries are registered as thirdparty::* targets so the
# existing default_install() recursion deploys their versioned .so files.
tempapa_define_shared(thirdparty::tempapa-osqp "${TEMPAPA_OSQP_LIBRARY}")

set_property(TARGET thirdparty::tempapa-osqp PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES "${TEMPAPA_OSQP_INCLUDE_DIR}")

set(TEMPAPA_LINK_LIBRARIES
    "thirdparty::tempapa-eigen;thirdparty::tempapa-boost;thirdparty::tempapa-abseil;thirdparty::tempapa-cppad;thirdparty::tempapa-osqp")

if(TEMPAPA_ENABLE_SOLVER_RUNTIME)
    tempapa_define_shared(thirdparty::tempapa-cppad-lib "${TEMPAPA_CPPAD_LIBRARY}")
    tempapa_define_shared(thirdparty::tempapa-ipopt "${TEMPAPA_IPOPT_LIBRARY}")
    tempapa_define_shared(thirdparty::tempapa-coinmumps "${TEMPAPA_COINMUMPS_LIBRARY}")
    tempapa_define_shared(thirdparty::tempapa-blas "${TEMPAPA_BLAS_LIBRARY}")
    tempapa_define_shared(thirdparty::tempapa-lapack "${TEMPAPA_LAPACK_LIBRARY}")
    tempapa_define_shared(thirdparty::tempapa-gfortran "${TEMPAPA_GFORTRAN_LIBRARY}")

    if(EXISTS "${TEMPAPA_OPENBLAS_LIBRARY}")
        tempapa_define_shared(thirdparty::tempapa-openblas "${TEMPAPA_OPENBLAS_LIBRARY}")
    endif()

    set_property(TARGET thirdparty::tempapa-cppad-lib PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${TEMPAPA_CPPAD_INCLUDE_DIR}")
    set_property(TARGET thirdparty::tempapa-ipopt PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${TEMPAPA_IPOPT_INCLUDE_DIR}")
    set_property(TARGET thirdparty::tempapa-coinmumps PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${TEMPAPA_IPOPT_INCLUDE_DIR}")
    set_property(TARGET thirdparty::tempapa-blas PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${TEMPAPA_INCLUDE_ROOT}")
    set_property(TARGET thirdparty::tempapa-lapack PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${TEMPAPA_INCLUDE_ROOT}")
    set_property(TARGET thirdparty::tempapa-gfortran PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${TEMPAPA_INCLUDE_ROOT}")
    if(TARGET thirdparty::tempapa-openblas)
        set_property(TARGET thirdparty::tempapa-blas APPEND PROPERTY
            INTERFACE_LINK_LIBRARIES thirdparty::tempapa-openblas)
        set_property(TARGET thirdparty::tempapa-lapack APPEND PROPERTY
            INTERFACE_LINK_LIBRARIES thirdparty::tempapa-openblas)
    endif()
    set(TEMPAPA_MUMPS_LINK_LIBRARIES
        "thirdparty::tempapa-lapack;thirdparty::tempapa-blas;thirdparty::tempapa-gfortran;${CMAKE_DL_LIBS};pthread")
    if(TARGET thirdparty::tempapa-openblas)
        list(APPEND TEMPAPA_MUMPS_LINK_LIBRARIES thirdparty::tempapa-openblas)
    endif()
    set_property(TARGET thirdparty::tempapa-coinmumps APPEND PROPERTY
        INTERFACE_LINK_LIBRARIES
        "${TEMPAPA_MUMPS_LINK_LIBRARIES}")

    set_property(TARGET thirdparty::tempapa-cppad APPEND PROPERTY
        INTERFACE_LINK_LIBRARIES thirdparty::tempapa-cppad-lib)
    set_property(TARGET thirdparty::tempapa-ipopt APPEND PROPERTY
        INTERFACE_LINK_LIBRARIES
        "thirdparty::tempapa-coinmumps;thirdparty::tempapa-lapack;thirdparty::tempapa-blas;thirdparty::tempapa-gfortran;${CMAKE_DL_LIBS};pthread")
    list(APPEND TEMPAPA_LINK_LIBRARIES thirdparty::tempapa-ipopt)
endif()

if(TEMPAPA_COINASL_LIBRARY)
    tempapa_define_shared(thirdparty::tempapa-coinasl "${TEMPAPA_COINASL_LIBRARY}")
    set_property(TARGET thirdparty::tempapa-coinasl PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${TEMPAPA_IPOPT_INCLUDE_DIR}")
endif()
if(TEMPAPA_IPOPT_AMPL_LIBRARY)
    tempapa_define_shared(thirdparty::tempapa-ipopt-ampl "${TEMPAPA_IPOPT_AMPL_LIBRARY}")
    set_property(TARGET thirdparty::tempapa-ipopt-ampl PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${TEMPAPA_IPOPT_INCLUDE_DIR}")
    set_property(TARGET thirdparty::tempapa-ipopt-ampl APPEND PROPERTY
        INTERFACE_LINK_LIBRARIES "thirdparty::tempapa-ipopt;thirdparty::tempapa-coinasl")
endif()
if(TEMPAPA_QDLDL_LIBRARY)
    tempapa_define_shared(thirdparty::tempapa-qdldl "${TEMPAPA_QDLDL_LIBRARY}")
    set_property(TARGET thirdparty::tempapa-qdldl PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${TEMPAPA_OSQP_INCLUDE_DIR}")
endif()

# Set TEMPAPA_BUILD_ABSEIL=ON when the real Abseil implementation is needed.
# The current standalone compatibility headers intentionally remain the default
# to avoid mixing two providers for the same absl/ include names.
option(TEMPAPA_BUILD_ABSEIL
    "Build Abseil from Temp_Lib instead of using standalone compatibility headers"
    OFF)
if(TEMPAPA_BUILD_ABSEIL)
    set(ABSL_BUILD_TESTING OFF CACHE BOOL "" FORCE)
    add_subdirectory("${TEMPAPA_LIB_ROOT}/abseil-cpp"
        "${CMAKE_BINARY_DIR}/thirdparty_tempapa_abseil" EXCLUDE_FROM_ALL)
    set_property(TARGET thirdparty::tempapa-abseil APPEND PROPERTY
        INTERFACE_LINK_LIBRARIES "absl::strings;absl::str_format")
endif()

# One bundle target keeps the application dependency declaration small and lets
# default_install() recursively deploy all Temp_Lib shared libraries.
if(NOT TARGET thirdparty::tempapa)
    add_library(thirdparty::tempapa INTERFACE IMPORTED GLOBAL)
endif()
set_target_properties(thirdparty::tempapa PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES
        "${TEMPAPA_EIGEN_INCLUDE_DIR};${TEMPAPA_BOOST_INCLUDE_DIR};${TEMPAPA_ABSEIL_INCLUDE_DIR};${TEMPAPA_CPPAD_INCLUDE_DIR};${TEMPAPA_OSQP_INCLUDE_DIR};${TEMPAPA_IPOPT_INCLUDE_DIR}"
    INTERFACE_LINK_LIBRARIES "${TEMPAPA_LINK_LIBRARIES}")

message(STATUS "[valet_parking] TempAPA third-party root: ${TEMPAPA_LIB_ROOT}")
message(STATUS "[valet_parking] TempAPA include root: ${TEMPAPA_INCLUDE_ROOT}")
message(STATUS "[valet_parking] TempAPA library root: ${TEMPAPA_LIBRARY_ROOT}")
message(STATUS "[valet_parking] TempAPA Eigen: ${TEMPAPA_EIGEN_INCLUDE_DIR}")
message(STATUS "[valet_parking] TempAPA Boost: ${TEMPAPA_BOOST_INCLUDE_DIR}")
message(STATUS "[valet_parking] TempAPA Abseil: ${TEMPAPA_ABSEIL_INCLUDE_DIR}")
message(STATUS "[valet_parking] TempAPA CppAD: ${TEMPAPA_CPPAD_INCLUDE_DIR}")
message(STATUS "[valet_parking] TempAPA Ipopt: ${TEMPAPA_IPOPT_LIBRARY}")
message(STATUS "[valet_parking] TempAPA OSQP: ${TEMPAPA_OSQP_LIBRARY}")
message(STATUS "[valet_parking] TempAPA solver runtime: ${TEMPAPA_ENABLE_SOLVER_RUNTIME}")
