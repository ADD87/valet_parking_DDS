#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Usage:
  bash applications/source/valet_parking_tools/package_valet_parking_m57.sh [options]

Options:
  --run-root PATH       Built m57 runtime root. Default:
                        <workspace>/out/valet_parking_batch_042_046/valet_parking_mvp/m57
  --package-root PATH   Output package root. Default:
                        <workspace>/out/valet_parking_m57_package
  --name NAME           Package name prefix. Default: valet_parking_m57_runtime
  --no-tools            Include only valet_parking_runner, not mock tools.
  --no-tar              Do not create .tar.gz.
  --help                Show this help.
EOF
}

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
app_source_root="$(cd "${script_dir}/.." && pwd -P)"
applications_root="$(cd "${app_source_root}/.." && pwd -P)"
workspace_root="$(cd "${applications_root}/.." && pwd -P)"

run_root="${workspace_root}/out/valet_parking_batch_042_046/valet_parking_mvp/m57"
package_root="${workspace_root}/out/valet_parking_m57_package"
package_name="valet_parking_m57_runtime"
include_tools=1
create_tar=1

while [[ $# -gt 0 ]]; do
  case "$1" in
    --run-root)
      run_root="${2:-}"
      shift 2
      ;;
    --package-root)
      package_root="${2:-}"
      shift 2
      ;;
    --name)
      package_name="${2:-}"
      shift 2
      ;;
    --no-tools)
      include_tools=0
      shift
      ;;
    --no-tar)
      create_tar=0
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

if [[ -z "${run_root}" || -z "${package_root}" || -z "${package_name}" ]]; then
  echo "run-root, package-root and name must be non-empty." >&2
  exit 2
fi

required_files=(
  "${run_root}/app/valet_parking_runner"
  "${run_root}/lib/libvalet_parking.so"
  "${run_root}/lib/libmagna-dds-core.so.1.0.0"
  "${run_root}/lib/libmagna-dds-impl.so"
)

for required in "${required_files[@]}"; do
  if [[ ! -e "${required}" ]]; then
    echo "Missing required m57 runtime file: ${required}" >&2
    exit 1
  fi
done

timestamp="$(date +%Y%m%d_%H%M%S)"
package_dir="${package_root}/${package_name}_${timestamp}"

mkdir -p "${package_dir}/app" "${package_dir}/lib"

cp -a "${run_root}/app/valet_parking_runner" "${package_dir}/app/"
if [[ "${include_tools}" == "1" ]]; then
  for tool in \
    aux_input_mock_publisher \
    parking_command_mock_publisher \
    planning_trajectory_mock_subscriber \
    selected_slot_mock_publisher; do
    if [[ -x "${run_root}/app/${tool}" ]]; then
      cp -a "${run_root}/app/${tool}" "${package_dir}/app/"
    fi
  done
fi

cp -a "${run_root}/lib/libvalet_parking.so" "${package_dir}/lib/"
cp -a "${run_root}/lib/libmagna-dds-core.so"* "${package_dir}/lib/"
cp -a "${run_root}/lib/libmagna-dds-impl.so"* "${package_dir}/lib/"

cat >"${package_dir}/run_valet_parking_runner.sh" <<'EOF'
#!/usr/bin/env bash
set -euo pipefail

package_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
export LD_LIBRARY_PATH="${package_dir}/lib:${LD_LIBRARY_PATH:-}"

exec "${package_dir}/app/valet_parking_runner" "$@"
EOF
chmod +x "${package_dir}/run_valet_parking_runner.sh"

cat >"${package_dir}/README_RUNTIME.md" <<'EOF'
# Valet Parking m57 Runtime Package

This package is generated from the current m57 build output.

Run on the m57 board:

```bash
chmod +x run_valet_parking_runner.sh app/*
./run_valet_parking_runner.sh --domain-id=0
```

Override topics when integrating with real vehicle topics:

```bash
./run_valet_parking_runner.sh \
  --domain-id=0 \
  --in-topic=/selected_slot \
  --out-topic=/planning/trajectory \
  --command-topic=/parking/command \
  --localization-topic=/localization/estimate \
  --chassis-topic=/chassis/state \
  --obstacle-topic=/perception/obstacles
```

This package only proves files are ready to run on an aarch64 Linux target. It
does not prove board-side DDS communication has passed.
EOF

{
  echo "package_name=${package_name}"
  echo "created_at=${timestamp}"
  echo "source_run_root=${run_root}"
  echo
  echo "[files]"
  find "${package_dir}" -maxdepth 3 \( -type f -o -type l \) | sort
  echo
  echo "[file]"
  file \
    "${package_dir}/app/valet_parking_runner" \
    "${package_dir}/lib/libvalet_parking.so" \
    "${package_dir}/lib/libmagna-dds-core.so.1.0.0" \
    "${package_dir}/lib/libmagna-dds-impl.so"
  echo
  echo "[runner_dynamic]"
  readelf -d "${package_dir}/app/valet_parking_runner" | grep -E "NEEDED|RPATH|RUNPATH" || true
  echo
  echo "[libvalet_parking_dynamic]"
  readelf -d "${package_dir}/lib/libvalet_parking.so" | grep -E "NEEDED|RPATH|RUNPATH" || true
  echo
  echo "[sha256]"
  find "${package_dir}/app" "${package_dir}/lib" -maxdepth 1 -type f -print0 |
    sort -z | xargs -0 sha256sum
} >"${package_dir}/PACKAGE_MANIFEST.txt"

if [[ "${create_tar}" == "1" ]]; then
  tar_path="${package_dir}.tar.gz"
  tar -czf "${tar_path}" -C "${package_root}" "$(basename "${package_dir}")"
  echo "[package_valet_parking_m57] package_dir=${package_dir}"
  echo "[package_valet_parking_m57] tar=${tar_path}"
else
  echo "[package_valet_parking_m57] package_dir=${package_dir}"
fi
