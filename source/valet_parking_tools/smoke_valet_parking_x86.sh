#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Usage:
  bash applications/source/valet_parking_tools/smoke_valet_parking_x86.sh [options]

Options:
  --run-root PATH      Built x86 runtime root. Default:
                       <workspace>/out/valet_parking_quick_build/valet_parking_mvp/x86
  --domain-id N       DDS domain id. Default: 12.
  --timeout-ms N      Subscriber timeout. Default: 20000.
  --count N           Mock SelectedSlot publish count. Default: 3.
  --interval-ms N     Mock publish interval. Default: 500.
  --with-aux-inputs   Publish localization/chassis/obstacle samples before SelectedSlot.
  --aux-count N       Aux sample group count. Default: 3.
  --aux-interval-ms N Aux sample group interval. Default: 200.
  --help              Show this help.
EOF
}

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
app_source_root="$(cd "${script_dir}/.." && pwd -P)"
applications_root="$(cd "${app_source_root}/.." && pwd -P)"
workspace_root="$(cd "${applications_root}/.." && pwd -P)"

run_root="${workspace_root}/out/valet_parking_quick_build/valet_parking_mvp/x86"
domain_id=12
timeout_ms=20000
count=3
interval_ms=500
with_aux_inputs=0
aux_count=3
aux_interval_ms=200

while [[ $# -gt 0 ]]; do
  case "$1" in
    --run-root)
      run_root="${2:-}"
      shift 2
      ;;
    --domain-id)
      domain_id="${2:-}"
      shift 2
      ;;
    --timeout-ms)
      timeout_ms="${2:-}"
      shift 2
      ;;
    --count)
      count="${2:-}"
      shift 2
      ;;
    --interval-ms)
      interval_ms="${2:-}"
      shift 2
      ;;
    --with-aux-inputs)
      with_aux_inputs=1
      shift
      ;;
    --aux-count)
      aux_count="${2:-}"
      shift 2
      ;;
    --aux-interval-ms)
      aux_interval_ms="${2:-}"
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

runner="${run_root}/app/valet_parking_runner"
subscriber="${run_root}/app/planning_trajectory_mock_subscriber"
publisher="${run_root}/app/selected_slot_mock_publisher"
aux_publisher="${run_root}/app/aux_input_mock_publisher"

for binary in "${runner}" "${subscriber}" "${publisher}"; do
  if [[ ! -x "${binary}" ]]; then
    echo "Runtime binary is missing or not executable: ${binary}" >&2
    exit 1
  fi
done
if [[ "${with_aux_inputs}" == "1" && ! -x "${aux_publisher}" ]]; then
  echo "Runtime binary is missing or not executable: ${aux_publisher}" >&2
  exit 1
fi

export LD_LIBRARY_PATH="${run_root}/lib:${LD_LIBRARY_PATH:-}"

log_dir="${run_root}/smoke_latest"
mkdir -p "${log_dir}"
runner_log="${log_dir}/runner.log"
subscriber_log="${log_dir}/subscriber.log"
publisher_log="${log_dir}/publisher.log"
aux_publisher_log="${log_dir}/aux_publisher.log"
: > "${runner_log}"
: > "${subscriber_log}"
: > "${publisher_log}"
: > "${aux_publisher_log}"

runner_pid=""
subscriber_pid=""
cleanup() {
  if [[ -n "${subscriber_pid:-}" ]]; then
    kill "${subscriber_pid}" 2>/dev/null || true
  fi
  if [[ -n "${runner_pid:-}" ]]; then
    kill "${runner_pid}" 2>/dev/null || true
  fi
  if [[ -n "${subscriber_pid:-}" ]]; then
    wait "${subscriber_pid}" 2>/dev/null || true
  fi
  if [[ -n "${runner_pid:-}" ]]; then
    wait "${runner_pid}" 2>/dev/null || true
  fi
}
trap cleanup EXIT

"${runner}" --domain-id="${domain_id}" >"${runner_log}" 2>&1 &
runner_pid=$!
sleep 2

if [[ "${with_aux_inputs}" == "1" ]]; then
  "${aux_publisher}" --domain-id="${domain_id}" --count="${aux_count}" \
    --interval-ms="${aux_interval_ms}" >"${aux_publisher_log}" 2>&1
  sleep 1
fi

"${subscriber}" --domain-id="${domain_id}" --timeout-ms="${timeout_ms}" --strict \
  >"${subscriber_log}" 2>&1 &
subscriber_pid=$!
sleep 2

"${publisher}" --domain-id="${domain_id}" --mode=valid --count="${count}" \
  --interval-ms="${interval_ms}" >"${publisher_log}" 2>&1

set +e
wait "${subscriber_pid}"
subscriber_status=$?
set -e
subscriber_pid=""

kill "${runner_pid}" 2>/dev/null || true
wait "${runner_pid}" 2>/dev/null || true
runner_pid=""
trap - EXIT

echo "[valet_parking_smoke] run_root=${run_root}"
echo "[valet_parking_smoke] logs=${log_dir}"
echo "[valet_parking_smoke] subscriber_status=${subscriber_status}"
echo "[valet_parking_smoke] runner status lines:"
grep -E "aux localization|aux chassis|aux obstacles|SPEED_OPTIMIZER|PATH_PARTITION|PATH_PROVIDER|bridged sample" "${runner_log}" | tail -n 30 || true
if [[ "${with_aux_inputs}" == "1" ]]; then
  echo "[valet_parking_smoke] aux publisher:"
  cat "${aux_publisher_log}"
fi
echo "[valet_parking_smoke] subscriber:"
cat "${subscriber_log}"
echo "[valet_parking_smoke] publisher:"
cat "${publisher_log}"

validation_status="${subscriber_status}"
if [[ "${with_aux_inputs}" == "1" ]]; then
  if ! grep -q "aux localization" "${runner_log}"; then
    echo "[valet_parking_smoke] missing aux localization consumption in runner log" >&2
    validation_status=8
  fi
  if ! grep -q "aux chassis" "${runner_log}"; then
    echo "[valet_parking_smoke] missing aux chassis consumption in runner log" >&2
    validation_status=8
  fi
  if ! grep -q "aux obstacles" "${runner_log}"; then
    echo "[valet_parking_smoke] missing aux obstacles consumption in runner log" >&2
    validation_status=8
  fi
  if ! grep -q "external_vehicle=true" "${runner_log}"; then
    echo "[valet_parking_smoke] missing external_vehicle=true in runner log" >&2
    validation_status=8
  fi
  if ! grep -Eq "external_obstacles=[1-9]" "${runner_log}"; then
    echo "[valet_parking_smoke] missing nonzero external_obstacles in runner log" >&2
    validation_status=8
  fi
fi

exit "${validation_status}"
