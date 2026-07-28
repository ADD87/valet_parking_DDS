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
  --out-dir PATH            Output directory. Default: <workspace>/out/valet_parking_quick_build.
  --help                    Show this help.

Examples:
  bash applications/source/valet_parking_tools/build_valet_parking.sh
  bash applications/source/valet_parking_tools/build_valet_parking.sh --platform x86
  bash applications/source/valet_parking_tools/build_valet_parking.sh --platform m57 --jobs 8
EOF
}

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
app_source_root="$(cd "${script_dir}/.." && pwd -P)"
applications_root="$(cd "${app_source_root}/.." && pwd -P)"
workspace_root="$(cd "${applications_root}/.." && pwd -P)"

platform="both"
mode="release"
jobs="$(nproc 2>/dev/null || echo 8)"
out_dir="${workspace_root}/out/valet_parking_quick_build"

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

build_script="${workspace_root}/compile/build.sh"
bom_file="${applications_root}/config/valet_parking_mvp_bom.yaml"

if [[ ! -x "${build_script}" ]]; then
  echo "Build script is not executable or missing: ${build_script}" >&2
  exit 1
fi

if [[ ! -f "${bom_file}" ]]; then
  echo "Product BOM is missing: ${bom_file}" >&2
  exit 1
fi

echo "[valet_parking] workspace: ${workspace_root}"
echo "[valet_parking] app root : ${app_source_root}"
echo "[valet_parking] out dir  : ${out_dir}"
echo "[valet_parking] platform : ${platform}"
echo "[valet_parking] mode     : ${mode}"
echo "[valet_parking] jobs     : ${jobs}"

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

  if [[ ! -f "${lib_path}" ]]; then
    echo "Expected library was not generated: ${lib_path}" >&2
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

echo
echo "[valet_parking] build completed."
