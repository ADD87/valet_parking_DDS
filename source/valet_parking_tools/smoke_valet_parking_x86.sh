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
  --slot-mode MODE    SelectedSlot publisher mode. Default: valid.
                       valid|empty|overflow|nan|degenerate-corners|
                       target-moves|parking-seq-changes|
                       multi-lot-seq-switch
  --with-aux-inputs   Publish localization/chassis/obstacle samples before SelectedSlot.
  --aux-mode MODE     Aux publisher mode. Default: all-valid.
                       all-valid|invalid-localization|nan-localization|
                       chassis-only|invalid-obstacles|bad-obstacle-geometry|
                       moving-localization|moving-localization-large|
                       far-localization|far-obstacles|many-obstacles|
                       obstacle-appears|obstacle-disappears
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
slot_mode="valid"
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
    --slot-mode)
      slot_mode="${2:-}"
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

effective_count="${count}"
effective_aux_count="${aux_count}"
effective_aux_interval_ms="${aux_interval_ms}"
if [[ "${slot_mode}" == "target-moves" ||
      "${slot_mode}" == "parking-seq-changes" ||
      "${slot_mode}" == "multi-lot-seq-switch" ]]; then
  if ((effective_count < 6)); then
    effective_count=6
  fi
fi
if [[ "${with_aux_inputs}" == "1" &&
      ( "${aux_mode}" == "moving-localization" ||
        "${aux_mode}" == "moving-localization-large" ||
        "${aux_mode}" == "obstacle-appears" ||
        "${aux_mode}" == "obstacle-disappears" ) &&
      "${disable_aux_input_topics}" != "1" ]]; then
  if ((effective_count < 6)); then
    effective_count=6
  fi
  if ((effective_aux_count < 8)); then
    effective_aux_count=8
  fi
fi

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
if [[ "${with_aux_inputs}" == "1" &&
      ( "${aux_mode}" == "moving-localization" ||
        "${aux_mode}" == "moving-localization-large" ||
        "${aux_mode}" == "far-localization" ||
        "${aux_mode}" == "obstacle-appears" ||
        "${aux_mode}" == "obstacle-disappears" ) &&
      "${disable_aux_input_topics}" != "1" ]]; then
  aux_runs_in_background=1
fi

if [[ "${with_aux_inputs}" == "1" && "${aux_runs_in_background}" != "1" ]]; then
  "${aux_publisher}" --domain-id="${domain_id}" --mode="${aux_mode}" \
    --count="${effective_aux_count}" \
    --interval-ms="${effective_aux_interval_ms}" >"${aux_publisher_log}" 2>&1
  sleep 1
fi

"${subscriber}" --domain-id="${domain_id}" --timeout-ms="${timeout_ms}" --strict \
  >"${subscriber_log}" 2>&1 &
subscriber_pid=$!
if [[ "${aux_runs_in_background}" == "1" ]]; then
  sleep 1
  "${aux_publisher}" --domain-id="${domain_id}" --mode="${aux_mode}" \
    --count="${effective_aux_count}" \
    --interval-ms="${effective_aux_interval_ms}" >"${aux_publisher_log}" 2>&1 &
  aux_publisher_pid=$!
  sleep 0.15
else
  sleep 2
fi

"${publisher}" --domain-id="${domain_id}" --mode="${slot_mode}" --count="${effective_count}" \
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
  runner_wait_pattern="bridged sample #2"
  if [[ "${aux_mode}" == "moving-localization" ]]; then
    runner_wait_pattern="trace_adjust=true"
  elif [[ "${aux_mode}" == "moving-localization-large" ]]; then
    runner_wait_pattern="warm_start_reject=lateral_offset_large"
  elif [[ "${aux_mode}" == "far-localization" ]]; then
    runner_wait_pattern="vehicle_lot_precheck failed"
  elif [[ "${aux_mode}" == "obstacle-appears" ]]; then
    runner_wait_pattern="replan=BLOCK_BY_STATIC_OBSTACLE"
  elif [[ "${aux_mode}" == "obstacle-disappears" ]]; then
    runner_wait_pattern="replan=BLOCK_BY_STATIC_OBSTACLE.*external_obstacles=0"
  fi
  wait_for_runner_log "${runner_wait_pattern}" "${runner_wait_seconds}" || true
fi

if [[ "${slot_mode}" == "degenerate-corners" ]]; then
  runner_wait_seconds=$(((timeout_ms + 999) / 1000))
  if ((runner_wait_seconds < 5)); then
    runner_wait_seconds=5
  fi
  wait_for_runner_log "selected parking lot corner geometry is degenerate" \
    "${runner_wait_seconds}" || true
fi
if [[ "${slot_mode}" == "target-moves" ||
      "${slot_mode}" == "parking-seq-changes" ||
      "${slot_mode}" == "multi-lot-seq-switch" ]]; then
  runner_wait_seconds=$(((timeout_ms + 999) / 1000))
  if ((runner_wait_seconds < 5)); then
    runner_wait_seconds=5
  fi
  wait_for_runner_log "replan=TARGET_UPDATE" "${runner_wait_seconds}" || true
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

require_runner_log() {
  local pattern="$1"
  local message="$2"
  if ! grep -Eq "${pattern}" "${runner_log}"; then
    echo "[valet_parking_smoke] ${message}" >&2
    validation_status=8
  fi
}

require_subscriber_log() {
  local pattern="$1"
  local message="$2"
  if ! grep -Eq "${pattern}" "${subscriber_log}"; then
    echo "[valet_parking_smoke] ${message}" >&2
    validation_status=8
  fi
}

require_publisher_log() {
  local pattern="$1"
  local message="$2"
  if ! grep -Eq "${pattern}" "${publisher_log}"; then
    echo "[valet_parking_smoke] ${message}" >&2
    validation_status=8
  fi
}

case "${slot_mode}" in
  valid)
    ;;
  target-moves)
    require_publisher_log "published sample [1-3]/[0-9]+ .*target=base" \
      "missing base target samples in target-moves slot mode"
    require_publisher_log "published sample [4-6]/[0-9]+ .*target=moved" \
      "missing moved target samples in target-moves slot mode"
    require_runner_log "PATH_PROVIDER ok.*history=generated, replan=TARGET_UPDATE.*reason=target_update" \
      "missing generated path after selected slot target update"
    require_runner_log "PATH_PROVIDER ok.*history=reused, replan=NONE.*generated_count=2" \
      "missing history reuse after moved selected slot becomes stable"
    require_subscriber_log "is_estop=false" \
      "missing non-estop trajectory for target-moves slot mode"
    ;;
  parking-seq-changes)
    require_publisher_log "published sample [1-3]/[0-9]+ .*parking_seq=1" \
      "missing initial parking_seq=1 samples in parking-seq-changes slot mode"
    require_publisher_log "published sample [4-6]/[0-9]+ .*parking_seq=2" \
      "missing changed parking_seq=2 samples in parking-seq-changes slot mode"
    require_runner_log "PATH_PROVIDER ok.*history=generated, replan=TARGET_UPDATE.*reason=target_update" \
      "missing generated path after selected slot parking_seq/path_id update"
    require_runner_log "PATH_PROVIDER ok.*history=reused, replan=NONE.*generated_count=2" \
      "missing history reuse after changed parking_seq/path_id becomes stable"
    require_subscriber_log "is_estop=false" \
      "missing non-estop trajectory for parking-seq-changes slot mode"
    ;;
  multi-lot-seq-switch)
    require_publisher_log "published sample [1-3]/[0-9]+ .*lots=2 .*parking_seq=1 .*target=slot1" \
      "missing initial multi-lot samples selecting parking_seq=1"
    require_publisher_log "published sample [4-6]/[0-9]+ .*lots=2 .*parking_seq=2 .*target=slot2" \
      "missing switched multi-lot samples selecting parking_seq=2"
    require_runner_log "PATH_PROVIDER ok.*parking_seq=1.*history=generated" \
      "missing generated path for selected parking_seq=1"
    require_runner_log "PATH_PROVIDER ok.*parking_seq=2.*history=generated, replan=TARGET_UPDATE.*reason=target_update" \
      "missing generated path after opt_parking_seq selects parking_seq=2"
    require_runner_log "PATH_PROVIDER ok.*parking_seq=2.*history=reused, replan=NONE.*generated_count=2" \
      "missing history reuse after selected parking_seq=2 becomes stable"
    require_subscriber_log "is_estop=false" \
      "missing non-estop trajectory for multi-lot-seq-switch slot mode"
    ;;
  degenerate-corners)
    require_runner_log "selected parking lot corner geometry is degenerate" \
      "missing degenerate corner geometry rejection in runner log"
    require_runner_log "estop=true" \
      "missing runner estop for degenerate-corners slot mode"
    require_subscriber_log "is_estop=true" \
      "missing subscriber estop for degenerate-corners slot mode"
    ;;
  empty|overflow|nan)
    require_runner_log "estop=true" \
      "missing runner estop for invalid slot mode ${slot_mode}"
    require_subscriber_log "is_estop=true" \
      "missing subscriber estop for invalid slot mode ${slot_mode}"
    ;;
  *)
    echo "[valet_parking_smoke] unknown slot mode for validation: ${slot_mode}" >&2
    validation_status=8
    ;;
esac

if [[ "${with_aux_inputs}" == "1" ]]; then
  require_log() {
    local pattern="$1"
    local message="$2"
    if ! grep -Eq "${pattern}" "${runner_log}"; then
      echo "[valet_parking_smoke] ${message}" >&2
      validation_status=8
    fi
  }

  require_aux_log() {
    local pattern="$1"
    local message="$2"
    if ! grep -Eq "${pattern}" "${aux_publisher_log}"; then
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
        require_log "aux localization #[0-9]+ \\(x=0(\\.00)?, y=0" \
          "missing initial moving localization sample"
        require_log "aux localization #[0-9]+ .*x=0\\.8" \
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
        require_log "trace_adjust=true" \
          "missing trace adjust strategy in moving-localization mode"
        require_log "trace_adjust_reject=accepted" \
          "missing accepted trace adjust diagnostic in moving-localization mode"
        require_log "trace_adjust_points=[1-9][0-9]*" \
          "missing trace adjust path points in moving-localization mode"
        require_log "trace_adjust_path_length=[1-9]" \
          "missing trace adjust path length in moving-localization mode"
        ;;
      moving-localization-large)
        require_log "aux localization #[0-9]+ \\(x=0(\\.00)?, y=0" \
          "missing initial large moving localization sample"
        require_log "aux localization #[0-9]+ .*x=1\\.4" \
          "missing shifted large moving localization sample"
        require_log "external_vehicle=true" \
          "missing external_vehicle=true in moving-localization-large mode"
        require_log "external_obstacles=[1-9]" \
          "missing retained external obstacles in moving-localization-large mode"
        require_log "replan=TRACE_REPLAN" \
          "missing TRACE_REPLAN in moving-localization-large mode"
        require_log "warm_start=none" \
          "missing rejected warm start source in moving-localization-large mode"
        require_log "warm_start_reject=lateral_offset_large" \
          "missing lateral_offset_large warm start diagnostic"
        require_log "warm_start_points=0" \
          "missing zero warm_start_points after large localization shift"
        require_log "trace_adjust=false" \
          "missing disabled trace adjust after rejected warm start"
        require_log "trace_adjust_reject=no_trace_path" \
          "missing no_trace_path trace adjust diagnostic"
        require_log "trace_adjust_path_length=0" \
          "missing zero trace adjust path length after rejected warm start"
        ;;
      far-localization)
        require_log "aux localization #[0-9]+ .*x=1000" \
          "missing far localization sample"
        require_log "vehicle_lot_precheck failed: vehicle outside selected lot envelope" \
          "missing vehicle_lot_precheck failure in far-localization mode"
        if ! grep -Eq "is_estop=true" "${subscriber_log}"; then
          echo "[valet_parking_smoke] missing estop trajectory in far-localization mode" >&2
          validation_status=8
        fi
        ;;
      far-obstacles)
        require_log "aux localization #[0-9]+" \
          "missing aux localization consumption in far-obstacles mode"
        require_log "aux chassis #[0-9]+" \
          "missing aux chassis consumption in far-obstacles mode"
        require_log "aux obstacles #[0-9]+ \\(count=1\\)" \
          "missing aux obstacle consumption in far-obstacles mode"
        require_log "PATH_PROVIDER_PRECHECK failed: obstacle_segment_outside_xy_bounds\\[[0-9]+\\]" \
          "missing obstacle local bounds precheck failure"
        require_log "estop=true" \
          "missing runner estop for far-obstacles mode"
        require_subscriber_log "is_estop=true" \
          "missing subscriber estop for far-obstacles mode"
        ;;
      many-obstacles)
        require_log "aux localization #[0-9]+" \
          "missing aux localization consumption in many-obstacles mode"
        require_log "aux chassis #[0-9]+" \
          "missing aux chassis consumption in many-obstacles mode"
        require_log "aux obstacles #[0-9]+ \\(count=128\\)" \
          "missing 128-obstacle aux sample consumption in many-obstacles mode"
        require_log "PATH_PROVIDER_PRECHECK failed: too_many_obstacle_segments=[0-9]+" \
          "missing obstacle segment overload precheck failure"
        require_log "estop=true" \
          "missing runner estop for many-obstacles mode"
        require_subscriber_log "is_estop=true" \
          "missing subscriber estop for many-obstacles mode"
        ;;
      obstacle-appears)
        require_log "aux localization #[0-9]+" \
          "missing aux localization consumption in obstacle-appears mode"
        require_log "aux chassis #[0-9]+" \
          "missing aux chassis consumption in obstacle-appears mode"
        require_aux_log "published sample group [1-3]/[0-9]+ .*obstacles=skipped obstacle_count=0" \
          "missing initial skipped obstacle groups in obstacle-appears mode"
        require_log "aux obstacles #[0-9]+ \\(count=1\\)" \
          "missing appeared aux obstacle consumption in runner log"
        require_log "external_obstacles=0" \
          "missing initial empty obstacle planning state"
        require_log "PATH_PROVIDER ok.*history=generated, replan=BLOCK_BY_STATIC_OBSTACLE.*reason=obstacles_changed" \
          "missing generated path after obstacle signature change"
        require_log "PATH_PROVIDER ok.*history=reused, replan=NONE.*external_obstacles=1" \
          "missing history reuse after appeared obstacle becomes stable"
        require_log "external_obstacles=1" \
          "missing planning state with appeared obstacle"
        ;;
      obstacle-disappears)
        require_log "aux localization #[0-9]+" \
          "missing aux localization consumption in obstacle-disappears mode"
        require_log "aux chassis #[0-9]+" \
          "missing aux chassis consumption in obstacle-disappears mode"
        require_aux_log "published sample group [1-3]/[0-9]+ .*obstacles=present-valid obstacle_count=1" \
          "missing initial present obstacle groups in obstacle-disappears mode"
        require_aux_log "published sample group [4-8]/[0-9]+ .*obstacles=cleared-valid obstacle_count=0" \
          "missing cleared obstacle groups in obstacle-disappears mode"
        require_log "aux obstacles #[0-9]+ \\(count=1\\)" \
          "missing initial aux obstacle consumption in obstacle-disappears mode"
        require_log "aux obstacles #[0-9]+ \\(count=0\\)" \
          "missing cleared aux obstacle consumption in runner log"
        require_log "external_obstacles=1" \
          "missing initial obstacle planning state"
        require_log "PATH_PROVIDER ok.*history=generated, replan=BLOCK_BY_STATIC_OBSTACLE.*reason=obstacles_changed.*external_obstacles=0" \
          "missing generated path after obstacle clear signature change"
        require_log "PATH_PROVIDER ok.*history=reused, replan=NONE.*external_obstacles=0" \
          "missing history reuse after obstacles remain cleared"
        ;;
      *)
        echo "[valet_parking_smoke] unknown aux mode for validation: ${aux_mode}" >&2
        validation_status=8
        ;;
    esac
  fi
fi

exit "${validation_status}"
