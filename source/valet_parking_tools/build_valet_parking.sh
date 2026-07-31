#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Usage:
  bash applications/source/valet_parking_tools/build_valet_parking.sh [options]

Options:
  --platform x86|m57|both   Target platform to build. Default: both.
  --mode release|debug      Build mode passed to compile/build.sh. Default: release.
  --jobs N                  Parallel build jobs. Default: nproc.
  --out-dir PATH            Build output directory. Default: <workspace>/out/valet_parking_quick_build.
  --package-dir PATH        Package directory for collected artifacts (libvalet_parking.so, runner, tools).
                            Default: <workspace>/out/valet_parking_package.
  --force-clean             Remove the entire build output directory before building.
  --help                    Show this help.

Examples:
  bash applications/source/valet_parking_tools/build_valet_parking.sh
  bash applications/source/valet_parking_tools/build_valet_parking.sh --platform x86
  bash applications/source/valet_parking_tools/build_valet_parking.sh --platform m57 --jobs 8 --force-clean
EOF
}

# ---- path auto-detection (portable across machines) ----
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
app_source_root="$(cd "${script_dir}/.." && pwd -P)"
applications_root="$(cd "${app_source_root}/.." && pwd -P)"
workspace_root="$(cd "${applications_root}/.." && pwd -P)"

platform="both"
mode="release"
jobs="$(nproc 2>/dev/null || echo 8)"
out_dir="${workspace_root}/out/valet_parking_quick_build"
package_dir=""
force_clean=false

while [[ $# -gt 0 ]]; do
  case "$1" in
    --platform)
      platform="${2:-}"
      shift 2
      ;;
    --mode)
      mode="${2:-}"
      shift 2
      ;;
    --jobs|--j)
      jobs="${2:-}"
      shift 2
      ;;
    --out-dir)
      out_dir="${2:-}"
      shift 2
      ;;
    --package-dir)
      package_dir="${2:-}"
      shift 2
      ;;
    --force-clean)
      force_clean=true
      shift
      ;;
    --help|-h)
      usage
      exit 0
      ;;
    *)
      echo "Unknown argument: $1" >&2
      usage >&2
      exit 2
      ;;
  esac
done

case "${platform}" in
  x86)
    platforms=("x86")
    ;;
  m57)
    platforms=("m57")
    ;;
  both)
    platforms=("x86" "m57")
    ;;
  *)
    echo "Invalid --platform: ${platform}. Expected x86, m57, or both." >&2
    exit 2
    ;;
esac

if [[ "${mode}" != "release" && "${mode}" != "debug" ]]; then
  echo "Invalid --mode: ${mode}. Expected release or debug." >&2
  exit 2
fi

if ! [[ "${jobs}" =~ ^[0-9]+$ ]] || [[ "${jobs}" -le 0 ]]; then
  echo "Invalid --jobs: ${jobs}. Expected a positive integer." >&2
  exit 2
fi

if [[ "${out_dir}" != /* ]]; then
  out_dir="${workspace_root}/${out_dir}"
fi
out_dir="$(realpath -m "${out_dir}")"

if [[ -z "${package_dir}" ]]; then
  package_dir="${workspace_root}/out/valet_parking_package"
fi
if [[ "${package_dir}" != /* ]]; then
  package_dir="${workspace_root}/${package_dir}"
fi
package_dir="$(realpath -m "${package_dir}")"

build_script="${workspace_root}/compile/build.sh"
bom_file="${applications_root}/config/valet_parking_mvp_bom.yaml"

if [[ ! -x "${build_script}" ]]; then
  echo "Build script is not executable or missing: ${build_script}" >&2
  echo "Try: chmod +x ${build_script}" >&2
  exit 1
fi

if [[ ! -f "${bom_file}" ]]; then
  echo "Product BOM is missing: ${bom_file}" >&2
  exit 1
fi

# ---- auto-detect stale CMake cache (path changed after machine move) ----
stale_cache_detected=false
for check_target in "${platforms[@]}"; do
  check_cache="${out_dir}/valet_parking_mvp/${check_target}/build/CMakeCache.txt"
  if [[ -f "${check_cache}" ]]; then
    cached_compile_dir="$(grep -m1 '^CMAKE_HOME_DIRECTORY:INTERNAL=' "${check_cache}" 2>/dev/null | cut -d= -f2- || true)"
    current_compile_dir="${workspace_root}/compile"
    if [[ -n "${cached_compile_dir}" && "${cached_compile_dir}" != "${current_compile_dir}" ]]; then
      echo "[valet_parking] WARNING: stale CMake cache detected for ${check_target}"
      echo "[valet_parking]   cached compile dir : ${cached_compile_dir}"
      echo "[valet_parking]   current compile dir: ${current_compile_dir}"
      echo "[valet_parking]   This happens when workspace path has changed (e.g. moved to another machine)."
      echo "[valet_parking]   Build output will be cleaned automatically to avoid path mismatch errors."
      stale_cache_detected=true
      break
    fi
  fi
done

# ---- force-clean or auto-clean stale output ----
if [[ "${force_clean}" == "true" ]] || [[ "${stale_cache_detected}" == "true" ]]; then
  if [[ "${force_clean}" == "true" ]]; then
    echo "[valet_parking] --force-clean: removing build output directory..."
  fi
  rm -rf "${out_dir}"
  echo "[valet_parking] Build output cleaned: ${out_dir}"
fi

echo "[valet_parking] workspace : ${workspace_root}"
echo "[valet_parking] app root  : ${app_source_root}"
echo "[valet_parking] out dir   : ${out_dir}"
echo "[valet_parking] package   : ${package_dir}"
echo "[valet_parking] platform  : ${platform}"
echo "[valet_parking] mode      : ${mode}"
echo "[valet_parking] jobs      : ${jobs}"

for target in "${platforms[@]}"; do
  echo
  echo "[valet_parking] ==== build ${target} ===="
  bash "${build_script}" \
    --build="${target}" \
    --product-bom-file="${bom_file}" \
    --mode="${mode}" \
    --j="${jobs}" \
    --app-root="${app_source_root}" \
    --out-dir="${out_dir}"

  target_root="${out_dir}/valet_parking_mvp/${target}"
  lib_path="${target_root}/lib/libvalet_parking.so"
  runner_path="${target_root}/app/valet_parking_runner"
  build_log_path="${target_root}/build/build.log"

  if [[ ! -f "${lib_path}" ]]; then
    echo "========================================" >&2
    echo "ERROR: Expected library was not generated: ${lib_path}" >&2
    if [[ -f "${build_log_path}" ]]; then
      echo "" >&2
      echo "Last 40 lines of build log (${build_log_path}):" >&2
      echo "----------------------------------------" >&2
      tail -40 "${build_log_path}" >&2
    else
      echo "" >&2
      echo "Build log not found at: ${build_log_path}" >&2
      echo "Check CMake configure logs in: ${target_root}/build/" >&2
    fi
    echo "========================================" >&2
    exit 1
  fi

  echo "[valet_parking] generated: ${lib_path}"
  file "${lib_path}"

  if command -v readelf >/dev/null 2>&1; then
    echo "[valet_parking] MagnaDDS dependencies:"
    readelf -d "${lib_path}" | grep -E 'libmagna-dds-(core|impl)' || {
      echo "libvalet_parking.so does not list MagnaDDS dependencies." >&2
      exit 1
    }
  fi

  if [[ -f "${runner_path}" ]]; then
    echo "[valet_parking] runner: ${runner_path}"
    file "${runner_path}"
  fi
done

# ---- collect artifacts into package directory ----
echo
echo "[valet_parking] ==== collecting artifacts to package dir ===="
mkdir -p "${package_dir}/lib" "${package_dir}/app"

for target in "${platforms[@]}"; do
  target_root="${out_dir}/valet_parking_mvp/${target}"
  target_pkg="${package_dir}/${target}"
  mkdir -p "${target_pkg}/lib" "${target_pkg}/app"

  lib_src="${target_root}/lib/libvalet_parking.so"
  if [[ -f "${lib_src}" ]]; then
    cp -a "${lib_src}" "${target_pkg}/lib/"
    echo "[valet_parking] collected ${target}: libvalet_parking.so -> ${target_pkg}/lib/"
  fi

  if [[ -f "${target_root}/app/valet_parking_runner" ]]; then
    cp -a "${target_root}/app/valet_parking_runner" "${target_pkg}/app/"
    echo "[valet_parking] collected ${target}: valet_parking_runner -> ${target_pkg}/app/"
  fi

  # collect all mock tools
  for tool in \
    aux_input_mock_publisher \
    parking_command_mock_publisher \
    selected_slot_mock_publisher \
    planning_trajectory_mock_subscriber; do
    tool_path="${target_root}/app/${tool}"
    if [[ -f "${tool_path}" ]]; then
      cp -a "${tool_path}" "${target_pkg}/app/"
      echo "[valet_parking] collected ${target}: ${tool} -> ${target_pkg}/app/"
    fi
  done

  # emit per-platform manifest
  {
    echo "platform=${target}"
    echo "mode=${mode}"
    echo "timestamp=$(date -Iseconds)"
    echo "workspace=${workspace_root}"
    echo ""
    echo "[files]"
    find "${target_pkg}" -type f | sort
    echo ""
    echo "[file_types]"
    file "${target_pkg}/"**/* 2>/dev/null || true
    echo ""
    echo "[sha256]"
    find "${target_pkg}" -type f -print0 | sort -z | xargs -0 sha256sum 2>/dev/null || true
  } >"${target_pkg}/MANIFEST.txt"

  echo "[valet_parking] manifest: ${target_pkg}/MANIFEST.txt"
done

echo
echo "[valet_parking] build completed."
echo "[valet_parking] artifacts collected in: ${package_dir}"