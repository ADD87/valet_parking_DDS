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
  --aux-mode MODE     Aux publisher mode. Default: all-valid.
                       all-valid|invalid-localization|nan-localization|
                       chassis-only|invalid-obstacles|bad-obstacle-geometry|
                       moving-localization
  --aux-count N       Aux sample group count. Default: 3.
  --aux-interval-ms N Aux sample group interval. Default: 200.
  --disable-aux-input-topics
                       Start runner without subscribing aux input topics.
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
aux_mode="all-valid"
aux_count=3
aux_interval_ms=200
disable_aux_input_topics=0

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
    --aux-mode)
      aux_mode="${2:-}"
      shift 2
      ;;
    --aux-count)
      aux_count="${2:-}"
      shift 2
      ;;
    --aux-interval-ms)
      aux_interval_ms="${2:-}"
      shift 2
      ;;
    --disable-aux-input-topics)
      disable_aux_input_topics=1
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

log_dir="${run_root}/smoke_latest/domain_${domain_id}_$(date +%Y%m%d_%H%M%S)_$$"
mkdir -p "${log_dir}"
runner_log="${log_dir}/runner.log"
subscriber_log="${log_dir}/subscriber.log"
publisher_log="${log_dir}/publisher.log"
aux_publisher_log="${log_dir}/aux_publisher.log"
aux_publisher_status=0
: > "${runner_log}"
: > "${subscriber_log}"
: > "${publisher_log}"
: > "${aux_publisher_log}"

runner_pid=""
subscriber_pid=""
aux_publisher_pid=""
cleanup() {
  if [[ -n "${aux_publisher_pid:-}" ]]; then
    kill "${aux_publisher_pid}" 2>/dev/null || true
  fi
  if [[ -n "${subscriber_pid:-}" ]]; then
    kill "${subscriber_pid}" 2>/dev/null || true
  fi
  if [[ -n "${runner_pid:-}" ]]; then
    kill "${runner_pid}" 2>/dev/null || true
  fi
  if [[ -n "${subscriber_pid:-}" ]]; then
    wait "${subscriber_pid}" 2>/dev/null || true
  fi
  if [[ -n "${aux_publisher_pid:-}" ]]; then
    wait "${aux_publisher_pid}" 2>/dev/null || true
  fi
  if [[ -n "${runner_pid:-}" ]]; then
    wait "${runner_pid}" 2>/dev/null || true
  fi
}
trap cleanup EXIT

wait_for_runner_log() {
  local pattern="$1"
  local timeout_seconds="$2"
  local deadline=$((SECONDS + timeout_seconds))
  while ((SECONDS < deadline)); do
    if grep -Eq "${pattern}" "${runner_log}"; then
      return 0
    fi
    sleep 0.2
  done
  return 1
}

runner_args=("--domain-id=${domain_id}")
if [[ "${disable_aux_input_topics}" == "1" ]]; then
  runner_args+=("--disable-aux-input-topics")
fi

"${runner}" "${runner_args[@]}" >"${runner_log}" 2>&1 &
runner_pid=$!
sleep 2

aux_runs_in_background=0
if [[ "${with_aux_inputs}" == "1" && "${aux_mode}" == "moving-localization" && "${disable_aux_input_topics}" != "1" ]]; then
  aux_runs_in_background=1
fi

if [[ "${with_aux_inputs}" == "1" && "${aux_runs_in_background}" != "1" ]]; then
  "${aux_publisher}" --domain-id="${domain_id}" --mode="${aux_mode}" \
    --count="${aux_count}" \
    --interval-ms="${aux_interval_ms}" >"${aux_publisher_log}" 2>&1
  sleep 1
fi

"${subscriber}" --domain-id="${domain_id}" --timeout-ms="${timeout_ms}" --strict \
  >"${subscriber_log}" 2>&1 &
subscriber_pid=$!
if [[ "${aux_runs_in_background}" == "1" ]]; then
  sleep 1
  "${aux_publisher}" --domain-id="${domain_id}" --mode="${aux_mode}" \
    --count="${aux_count}" \
    --interval-ms="${aux_interval_ms}" >"${aux_publisher_log}" 2>&1 &
  aux_publisher_pid=$!
  sleep 0.15
else
  sleep 2
fi

"${publisher}" --domain-id="${domain_id}" --mode=valid --count="${count}" \
  --interval-ms="${interval_ms}" >"${publisher_log}" 2>&1

if [[ -n "${aux_publisher_pid:-}" ]]; then
  set +e
  wait "${aux_publisher_pid}"
  aux_publisher_status=$?
  set -e
  aux_publisher_pid=""
  if [[ "${aux_publisher_status}" != "0" ]]; then
    echo "[valet_parking_smoke] aux publisher exited with ${aux_publisher_status}" >&2
  fi
fi

if [[ "${aux_runs_in_background}" == "1" ]]; then
  runner_wait_seconds=$(((timeout_ms + 999) / 1000))
  if ((runner_wait_seconds < 5)); then
    runner_wait_seconds=5
  fi
  wait_for_runner_log "bridged sample #2" "${runner_wait_seconds}" || true
fi

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
grep -E "aux localization|aux chassis|aux obstacles|SPEED_OPTIMIZER|PATH_PARTITION|PATH_PROVIDER|bridged sample" "${runner_log}" | tail -n 80 || true
if [[ "${with_aux_inputs}" == "1" ]]; then
  echo "[valet_parking_smoke] aux publisher:"
  cat "${aux_publisher_log}"
fi
echo "[valet_parking_smoke] subscriber:"
cat "${subscriber_log}"
echo "[valet_parking_smoke] publisher:"
cat "${publisher_log}"

validation_status="${subscriber_status}"
if [[ "${aux_publisher_status}" != "0" ]]; then
  validation_status="${aux_publisher_status}"
fi
if [[ "${with_aux_inputs}" == "1" ]]; then
  require_log() {
    local pattern="$1"
    local message="$2"
    if ! grep -Eq "${pattern}" "${runner_log}"; then
      echo "[valet_parking_smoke] ${message}" >&2
      validation_status=8
    fi
  }

  reject_log() {
    local pattern="$1"
    local message="$2"
    if grep -Eq "${pattern}" "${runner_log}"; then
      echo "[valet_parking_smoke] ${message}" >&2
      validation_status=8
    fi
  }

  if [[ "${disable_aux_input_topics}" == "1" ]]; then
    reject_log "aux localization|aux chassis|aux obstacles" \
      "runner consumed aux samples while aux input topics are disabled"
    require_log "external_vehicle=false" \
      "missing external_vehicle=false with disabled aux input topics"
    require_log "external_obstacles=0" \
      "missing external_obstacles=0 with disabled aux input topics"
  else
    case "${aux_mode}" in
      all-valid)
        require_log "aux localization #[0-9]+" \
          "missing aux localization consumption in runner log"
        require_log "aux chassis #[0-9]+" \
          "missing aux chassis consumption in runner log"
        require_log "aux obstacles #[0-9]+ \\(count=[1-9]" \
          "missing aux obstacle consumption in runner log"
        require_log "external_vehicle=true" \
          "missing external_vehicle=true in runner log"
        require_log "external_obstacles=[1-9]" \
          "missing nonzero external_obstacles in runner log"
        ;;
      invalid-localization|nan-localization)
        require_log "aux localization invalid #[0-9]+" \
          "missing invalid aux localization handling in runner log"
        require_log "aux chassis #[0-9]+" \
          "missing aux chassis consumption in runner log"
        require_log "aux obstacles #[0-9]+ \\(count=[1-9]" \
          "missing aux obstacle consumption in runner log"
        require_log "external_vehicle=false" \
          "missing external_vehicle=false after invalid localization"
        require_log "external_obstacles=[1-9]" \
          "missing retained external obstacles after invalid localization"
        ;;
      chassis-only)
        require_log "aux chassis #[0-9]+" \
          "missing aux chassis consumption in runner log"
        reject_log "aux localization" \
          "unexpected localization consumption in chassis-only mode"
        reject_log "aux obstacles" \
          "unexpected obstacle consumption in chassis-only mode"
        require_log "external_vehicle=false" \
          "missing external_vehicle=false in chassis-only mode"
        require_log "external_obstacles=0" \
          "missing external_obstacles=0 in chassis-only mode"
        ;;
      invalid-obstacles)
        require_log "aux localization #[0-9]+" \
          "missing aux localization consumption in invalid-obstacles mode"
        require_log "aux chassis #[0-9]+" \
          "missing aux chassis consumption in invalid-obstacles mode"
        require_log "aux obstacles invalid #[0-9]+" \
          "missing invalid aux obstacle handling in runner log"
        require_log "external_vehicle=true" \
          "missing external_vehicle=true in invalid-obstacles mode"
        require_log "external_obstacles=0" \
          "missing external_obstacles=0 after invalid obstacles"
        ;;
      bad-obstacle-geometry)
        require_log "aux localization #[0-9]+" \
          "missing aux localization consumption in bad-obstacle-geometry mode"
        require_log "aux chassis #[0-9]+" \
          "missing aux chassis consumption in bad-obstacle-geometry mode"
        require_log "aux obstacles rejected #[0-9]+" \
          "missing rejected aux obstacle handling in runner log"
        require_log "external_vehicle=true" \
          "missing external_vehicle=true in bad-obstacle-geometry mode"
        require_log "external_obstacles=0" \
          "missing external_obstacles=0 after bad obstacle geometry"
        ;;
      moving-localization)
        require_log "aux localization #[0-9]+ .*x=0" \
          "missing initial moving localization sample"
        require_log "aux localization #[0-9]+ .*y=0\\.38" \
          "missing shifted moving localization sample"
        require_log "external_vehicle=true" \
          "missing external_vehicle=true in moving-localization mode"
        require_log "external_obstacles=[1-9]" \
          "missing retained external obstacles in moving-localization mode"
        require_log "replan=TRACE_REPLAN" \
          "missing TRACE_REPLAN in moving-localization mode"
        require_log "warm_start=history_splice" \
          "missing history_splice warm start in moving-localization mode"
        require_log "warm_start_points=[1-9][0-9]*" \
          "missing nonzero warm_start_points in moving-localization mode"
        require_log "strategy_kappa_cost=true" \
          "missing enabled kappa cost in moving-localization mode"
        require_log "strategy_limit_steer=true" \
          "missing enabled steer limit in moving-localization mode"
        ;;
      *)
        echo "[valet_parking_smoke] unknown aux mode for validation: ${aux_mode}" >&2
        validation_status=8
        ;;
    esac
  fi
fi

exit "${validation_status}"
