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
                       missing-selected-lot|
                       target-moves|parking-seq-changes|
                       multi-lot-seq-switch
  --command-mode MODE ParkingCommand publisher mode. Default: none.
                       none|parking-in|direct-forward|direct-backward|
                       direct-forward-release|direct-backward-release|
                       pause|brake|finish|parking-out-left|
                       parking-out-right|parking-out-front|parking-out-back|
                       invalid
  --command-count N   ParkingCommand publish count. Default: 1.
  --command-interval-ms N
                       ParkingCommand publish interval. Default: 100.
  --command-reset-history
                       Publish command with reset_history=true and require the
                       runner to consume it as a one-shot reset.
  --pre-command-slot-count N
                       Publish N SelectedSlot samples before command smoke so
                       cleanup can be verified with existing path history.
  --direct-distance M Direct branch distance in meters. Default: 3.0.
  --direct-speed M    Direct branch speed in m/s. Default: 0.8.
  --disable-command-topic
                       Start runner without subscribing command topic.
  --with-aux-inputs   Publish localization/chassis/obstacle samples before SelectedSlot.
  --aux-mode MODE     Aux publisher mode. Default: all-valid.
                       all-valid|invalid-localization|nan-localization|
                       chassis-only|invalid-obstacles|bad-obstacle-geometry|
                       moving-localization|moving-localization-large|
                       near-destination|
                       far-localization|far-obstacles|many-obstacles|
                       obstacle-appears|obstacle-disappears
  --aux-chassis-gear GEAR
                       Aux chassis gear. Default: parking.
                       parking|drive|reverse|neutral
  --aux-chassis-speed-mps VALUE
                       Signed aux chassis speed in m/s. Default: 0.0.
  --aux-count N       Aux sample group count. Default: 3.
  --aux-interval-ms N Aux sample group interval. Default: 200.
  --disable-aux-input-topics
                       Start runner without subscribing aux input topics.
  --path-provider-timeout-s N
                       Override OpenSpacePathProvider target timeout through
                       VALET_PARKING_PATH_PROVIDER_TIMEOUT_S for smoke only.
  --expect-path-provider-timeout
                       Expect PATH_PROVIDER TARGET_TIMEOUT instead of normal
                       TARGET_READY for valid/no-command smoke.
  --force-path-partition-fail
                       Force PATH_PARTITION failure through smoke-only env.
  --force-speed-optimizer-fail
                       Force SPEED_OPTIMIZER failure through smoke-only env.
  --force-roi-decider-fail
                       Force ROI_DECIDER failure through smoke-only env.
  --force-straight-path-fail
                       Force OPEN_SPACE_STRAIGHT_PATH failure through
                       smoke-only env.
  --expect-thread-provider-stop
                       Require OpenSpaceThreadManager stop/destructor evidence
                       in runner log after runner exits.
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
command_mode="none"
command_count=1
command_interval_ms=100
command_reset_history=0
pre_command_slot_count=0
direct_distance=3.0
direct_speed=0.8
disable_command_topic=0
with_aux_inputs=0
aux_mode="all-valid"
aux_chassis_gear="parking"
aux_chassis_speed_mps="0.0"
aux_count=3
aux_interval_ms=200
disable_aux_input_topics=0
path_provider_timeout_s=""
expect_path_provider_timeout=0
force_path_partition_fail=0
force_speed_optimizer_fail=0
force_roi_decider_fail=0
force_straight_path_fail=0
expect_thread_provider_stop=0

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
    --command-mode)
      command_mode="${2:-}"
      shift 2
      ;;
    --command-count)
      command_count="${2:-}"
      shift 2
      ;;
    --command-interval-ms)
      command_interval_ms="${2:-}"
      shift 2
      ;;
    --command-reset-history)
      command_reset_history=1
      shift
      ;;
    --pre-command-slot-count)
      pre_command_slot_count="${2:-}"
      shift 2
      ;;
    --direct-distance)
      direct_distance="${2:-}"
      shift 2
      ;;
    --direct-speed)
      direct_speed="${2:-}"
      shift 2
      ;;
    --disable-command-topic)
      disable_command_topic=1
      shift
      ;;
    --with-aux-inputs)
      with_aux_inputs=1
      shift
      ;;
    --aux-mode)
      aux_mode="${2:-}"
      shift 2
      ;;
    --aux-chassis-gear)
      aux_chassis_gear="${2:-}"
      shift 2
      ;;
    --aux-chassis-speed-mps)
      aux_chassis_speed_mps="${2:-}"
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
    --path-provider-timeout-s)
      path_provider_timeout_s="${2:-}"
      shift 2
      ;;
    --expect-path-provider-timeout)
      expect_path_provider_timeout=1
      shift
      ;;
    --force-path-partition-fail)
      force_path_partition_fail=1
      shift
      ;;
    --force-speed-optimizer-fail)
      force_speed_optimizer_fail=1
      shift
      ;;
    --force-roi-decider-fail)
      force_roi_decider_fail=1
      shift
      ;;
    --force-straight-path-fail)
      force_straight_path_fail=1
      shift
      ;;
    --expect-thread-provider-stop)
      expect_thread_provider_stop=1
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

if ! [[ "${pre_command_slot_count}" =~ ^[0-9]+$ ]]; then
  echo "Invalid --pre-command-slot-count: ${pre_command_slot_count}" >&2
  exit 2
fi

effective_count="${count}"
effective_command_count="${command_count}"
effective_command_interval_ms="${command_interval_ms}"
effective_aux_count="${aux_count}"
effective_aux_interval_ms="${aux_interval_ms}"
direct_release_mode=0
command_publish_mode="${command_mode}"
case "${command_mode}" in
  direct-forward-release)
    direct_release_mode=1
    command_publish_mode="direct-forward"
    ;;
  direct-backward-release)
    direct_release_mode=1
    command_publish_mode="direct-backward"
    ;;
esac
if [[ "${command_mode}" != "none" ]]; then
  if [[ "${direct_release_mode}" == "1" ]]; then
    effective_count=4
    if ((effective_command_count < 8)); then
      effective_command_count=8
    fi
  elif ((effective_command_count < 8)); then
    effective_command_count=8
  fi
  if ((effective_command_interval_ms < 200)); then
    effective_command_interval_ms=200
  fi
fi
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
        "${aux_mode}" == "near-destination" ||
        "${aux_mode}" == "obstacle-appears" ||
        "${aux_mode}" == "obstacle-disappears" ) &&
      "${disable_aux_input_topics}" != "1" ]]; then
  if ((effective_count < 6)); then
    effective_count=6
  fi
  if ((effective_aux_count < 8)); then
    effective_aux_count=8
  fi
  if [[ "${aux_mode}" == "near-destination" ]]; then
    if ((effective_count < 12)); then
      effective_count=12
    fi
    if ((effective_aux_count < 14)); then
      effective_aux_count=14
    fi
  fi
fi
if [[ "${expect_path_provider_timeout}" == "1" &&
      -z "${path_provider_timeout_s}" ]]; then
  path_provider_timeout_s="0.001"
fi

runner="${run_root}/app/valet_parking_runner"
subscriber="${run_root}/app/planning_trajectory_mock_subscriber"
publisher="${run_root}/app/selected_slot_mock_publisher"
command_publisher="${run_root}/app/parking_command_mock_publisher"
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
if [[ "${command_mode}" != "none" && ! -x "${command_publisher}" ]]; then
  echo "Runtime binary is missing or not executable: ${command_publisher}" >&2
  exit 1
fi

export LD_LIBRARY_PATH="${run_root}/lib:${LD_LIBRARY_PATH:-}"

log_dir="${run_root}/smoke_latest/domain_${domain_id}_$(date +%Y%m%d_%H%M%S)_$$"
mkdir -p "${log_dir}"
runner_log="${log_dir}/runner.log"
subscriber_log="${log_dir}/subscriber.log"
publisher_log="${log_dir}/publisher.log"
command_publisher_log="${log_dir}/command_publisher.log"
aux_publisher_log="${log_dir}/aux_publisher.log"
command_publisher_status=0
aux_publisher_status=0
: > "${runner_log}"
: > "${subscriber_log}"
: > "${publisher_log}"
: > "${command_publisher_log}"
: > "${aux_publisher_log}"

runner_pid=""
subscriber_pid=""
aux_publisher_pid=""
command_publisher_pid=""
cleanup() {
  if [[ -n "${command_publisher_pid:-}" ]]; then
    kill "${command_publisher_pid}" 2>/dev/null || true
  fi
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
  if [[ -n "${command_publisher_pid:-}" ]]; then
    wait "${command_publisher_pid}" 2>/dev/null || true
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
    if grep -Eq "${pattern}" "${runner_log}" 2>/dev/null; then
      return 0
    fi
    sleep 0.2
  done
  return 1
}

is_less_than_zero() {
  awk -v v="$1" 'BEGIN { exit !((v + 0) < 0) }'
}

is_greater_than_zero() {
  awk -v v="$1" 'BEGIN { exit !((v + 0) > 0) }'
}

runner_args=("--domain-id=${domain_id}")
if [[ "${disable_command_topic}" == "1" ]]; then
  runner_args+=("--disable-command-topic")
fi
if [[ "${disable_aux_input_topics}" == "1" ]]; then
  runner_args+=("--disable-aux-input-topics")
fi

runner_env=()
if [[ -n "${path_provider_timeout_s}" ]]; then
  runner_env+=("VALET_PARKING_PATH_PROVIDER_TIMEOUT_S=${path_provider_timeout_s}")
fi
if [[ "${force_path_partition_fail}" == "1" ]]; then
  runner_env+=("VALET_PARKING_FORCE_PATH_PARTITION_FAIL=1")
fi
if [[ "${force_speed_optimizer_fail}" == "1" ]]; then
  runner_env+=("VALET_PARKING_FORCE_SPEED_OPTIMIZER_FAIL=1")
fi
if [[ "${force_roi_decider_fail}" == "1" ]]; then
  runner_env+=("VALET_PARKING_FORCE_ROI_DECIDER_FAIL=1")
fi
if [[ "${force_straight_path_fail}" == "1" ]]; then
  runner_env+=("VALET_PARKING_FORCE_STRAIGHT_PATH_FAIL=1")
fi

env "${runner_env[@]}" "${runner}" "${runner_args[@]}" >"${runner_log}" 2>&1 &
runner_pid=$!
sleep 2

aux_runs_in_background=0
if [[ "${with_aux_inputs}" == "1" &&
      ( "${aux_mode}" == "moving-localization" ||
        "${aux_mode}" == "moving-localization-large" ||
        "${aux_mode}" == "far-localization" ||
        "${aux_mode}" == "near-destination" ||
        "${aux_mode}" == "obstacle-appears" ||
        "${aux_mode}" == "obstacle-disappears" ) &&
      "${disable_aux_input_topics}" != "1" ]]; then
  aux_runs_in_background=1
fi

if [[ "${with_aux_inputs}" == "1" && "${aux_runs_in_background}" != "1" ]]; then
  "${aux_publisher}" --domain-id="${domain_id}" --mode="${aux_mode}" \
    --chassis-gear="${aux_chassis_gear}" \
    --chassis-speed-mps="${aux_chassis_speed_mps}" \
    --count="${effective_aux_count}" \
    --interval-ms="${effective_aux_interval_ms}" >"${aux_publisher_log}" 2>&1
  sleep 1
fi

"${subscriber}" --domain-id="${domain_id}" --timeout-ms="${timeout_ms}" --strict \
  >"${subscriber_log}" 2>&1 &
subscriber_pid=$!
if [[ "${aux_runs_in_background}" == "1" ]]; then
  sleep 0.2
  "${aux_publisher}" --domain-id="${domain_id}" --mode="${aux_mode}" \
    --chassis-gear="${aux_chassis_gear}" \
    --chassis-speed-mps="${aux_chassis_speed_mps}" \
    --count="${effective_aux_count}" \
    --interval-ms="${effective_aux_interval_ms}" >"${aux_publisher_log}" 2>&1 &
  aux_publisher_pid=$!
  sleep 0.15
else
  sleep 2
fi

if [[ "${command_mode}" != "none" ]] &&
   ((pre_command_slot_count > 0)); then
  {
    echo "[valet_parking_smoke] publishing selected_slot before command"
  } >>"${publisher_log}"
  "${publisher}" --domain-id="${domain_id}" --mode="${slot_mode}" \
    --count="${pre_command_slot_count}" \
    --interval-ms="${interval_ms}" >>"${publisher_log}" 2>&1
  wait_for_runner_log "PATH_PROVIDER ok.*history=generated" 10 || true
fi

if [[ "${command_mode}" != "none" ]]; then
  command_args=(
    "--domain-id=${domain_id}"
    "--mode=${command_publish_mode}"
    "--count=${effective_command_count}"
    "--interval-ms=${effective_command_interval_ms}"
    "--direct-distance=${direct_distance}"
    "--direct-speed=${direct_speed}"
    "--reason=smoke-${command_mode}"
  )
  if [[ "${command_reset_history}" == "1" ]]; then
    command_args+=("--reset-history")
  fi
  "${command_publisher}" "${command_args[@]}" >"${command_publisher_log}" 2>&1 &
  command_publisher_pid=$!

  command_wait_pattern="command #[0-9]+ mode="
  case "${command_mode}" in
    direct-forward|direct-forward-release)
      command_wait_pattern="command #[0-9]+ mode=DIRECT_FORWARD"
      ;;
    direct-backward|direct-backward-release)
      command_wait_pattern="command #[0-9]+ mode=DIRECT_BACKWARD"
      ;;
    parking-in)
      command_wait_pattern="command #[0-9]+ mode=PARKING_IN"
      ;;
    pause)
      command_wait_pattern="command #[0-9]+ mode=PAUSE"
      ;;
    brake)
      command_wait_pattern="command #[0-9]+ mode=BRAKE"
      ;;
    finish)
      command_wait_pattern="command #[0-9]+ mode=FINISH"
      ;;
    parking-out-left)
      command_wait_pattern="command #[0-9]+ mode=PARKING_OUT_LEFT"
      ;;
    parking-out-right)
      command_wait_pattern="command #[0-9]+ mode=PARKING_OUT_RIGHT"
      ;;
    parking-out-front)
      command_wait_pattern="command #[0-9]+ mode=PARKING_OUT_FRONT"
      ;;
    parking-out-back)
      command_wait_pattern="command #[0-9]+ mode=PARKING_OUT_BACK"
      ;;
    invalid)
      command_wait_pattern="command #[0-9]+ mode=NONE \\(cleared_command\\)"
      ;;
  esac
  wait_for_runner_log "${command_wait_pattern}" 8 || true
  if [[ "${direct_release_mode}" == "1" &&
        -n "${command_publisher_pid:-}" ]]; then
    set +e
    wait "${command_publisher_pid}"
    command_publisher_status=$?
    set -e
    command_publisher_pid=""
    if [[ "${command_publisher_status}" != "0" ]]; then
      echo "[valet_parking_smoke] command publisher exited with ${command_publisher_status}" >&2
    fi
  fi
fi

"${publisher}" --domain-id="${domain_id}" --mode="${slot_mode}" --count="${effective_count}" \
  --interval-ms="${interval_ms}" >>"${publisher_log}" 2>&1

if ! wait_for_runner_log "bridged sample #" 6; then
  {
    echo
    echo "[valet_parking_smoke] retrying selected_slot after discovery wait miss"
  } >>"${publisher_log}"
  "${publisher}" --domain-id="${domain_id}" --mode="${slot_mode}" \
    --count="${effective_count}" \
    --interval-ms="${interval_ms}" >>"${publisher_log}" 2>&1
  wait_for_runner_log "bridged sample #" 10 || true
fi

if [[ "${direct_release_mode}" == "1" ]]; then
  release_wait_pattern="STAGE_CONTROL DIRECT_FORWARD.*mission_state=DIRECT_CONTROL_ACTIVE"
  release_done_pattern="STAGE_CONTROL DIRECT_FORWARD_RELEASED"
  if [[ "${command_mode}" == "direct-backward-release" ]]; then
    release_wait_pattern="STAGE_CONTROL DIRECT_BACKWARD.*mission_state=DIRECT_CONTROL_ACTIVE"
    release_done_pattern="STAGE_CONTROL DIRECT_BACKWARD_RELEASED"
  fi
  wait_for_runner_log "${release_wait_pattern}" 10 || true
  {
    echo
    echo "[valet_parking_smoke] publishing command clear for ${command_mode}"
  } >>"${command_publisher_log}"
  "${command_publisher}" --domain-id="${domain_id}" --mode=none \
    --count=8 --interval-ms=200 \
    "--reason=smoke-${command_mode}-clear" >>"${command_publisher_log}" 2>&1
  wait_for_runner_log "command #[0-9]+ mode=NONE \\(cleared_command\\)" 8 || true
  {
    echo
    echo "[valet_parking_smoke] publishing selected_slot after command clear"
  } >>"${publisher_log}"
  post_clear_count=4
  if [[ "${slot_mode}" == "target-moves" ||
        "${slot_mode}" == "parking-seq-changes" ||
        "${slot_mode}" == "multi-lot-seq-switch" ]]; then
    post_clear_count=6
  fi
  "${publisher}" --domain-id="${domain_id}" --mode="${slot_mode}" \
    --count="${post_clear_count}" \
    --interval-ms="${interval_ms}" >>"${publisher_log}" 2>&1
  if ! wait_for_runner_log "${release_done_pattern}" 10; then
    {
      echo
      echo "[valet_parking_smoke] retrying selected_slot after release wait miss"
    } >>"${publisher_log}"
    "${publisher}" --domain-id="${domain_id}" --mode="${slot_mode}" \
      --count="${post_clear_count}" \
      --interval-ms="${interval_ms}" >>"${publisher_log}" 2>&1
    wait_for_runner_log "${release_done_pattern}" 20 || true
  fi
fi

if [[ -n "${command_publisher_pid:-}" ]]; then
  set +e
  wait "${command_publisher_pid}"
  command_publisher_status=$?
  set -e
  command_publisher_pid=""
  if [[ "${command_publisher_status}" != "0" ]]; then
    echo "[valet_parking_smoke] command publisher exited with ${command_publisher_status}" >&2
  fi
fi

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
  elif [[ "${aux_mode}" == "near-destination" ]]; then
    runner_wait_pattern="STAGE_CONTROL FINISH_HOLD"
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
grep -E "command #|command rejected|STAGE_CONTROL|OPEN_SPACE_STRAIGHT_PATH|SPEED_OPTIMIZER|PATH_PARTITION|PATH_PROVIDER|bridged sample|aux localization|aux chassis|aux obstacles" "${runner_log}" 2>/dev/null | tail -n 100 || true
if [[ "${command_mode}" != "none" ]]; then
  echo "[valet_parking_smoke] command publisher:"
  cat "${command_publisher_log}"
fi
if [[ "${with_aux_inputs}" == "1" ]]; then
  echo "[valet_parking_smoke] aux publisher:"
  cat "${aux_publisher_log}"
fi
echo "[valet_parking_smoke] subscriber:"
cat "${subscriber_log}"
echo "[valet_parking_smoke] publisher:"
cat "${publisher_log}"

validation_status="${subscriber_status}"
if [[ "${command_publisher_status}" != "0" ]]; then
  validation_status="${command_publisher_status}"
fi
if [[ "${aux_publisher_status}" != "0" ]]; then
  validation_status="${aux_publisher_status}"
fi

direct_velocity_conflict_expected=0
if [[ "${with_aux_inputs}" == "1" && "${disable_aux_input_topics}" != "1" ]]; then
  if [[ "${command_mode}" == "direct-forward" &&
        "${aux_chassis_gear}" == "drive" ]] &&
     is_less_than_zero "${aux_chassis_speed_mps}"; then
    direct_velocity_conflict_expected=1
  fi
  if [[ "${command_mode}" == "direct-backward" &&
        "${aux_chassis_gear}" == "reverse" ]] &&
     is_greater_than_zero "${aux_chassis_speed_mps}"; then
    direct_velocity_conflict_expected=1
  fi
fi

require_runner_log() {
  local pattern="$1"
  local message="$2"
  if ! grep -Eq "${pattern}" "${runner_log}" 2>/dev/null; then
    echo "[valet_parking_smoke] ${message}" >&2
    validation_status=8
  fi
}

require_subscriber_log() {
  local pattern="$1"
  local message="$2"
  if ! grep -Eq "${pattern}" "${subscriber_log}" 2>/dev/null; then
    echo "[valet_parking_smoke] ${message}" >&2
    validation_status=8
  fi
}

require_publisher_log() {
  local pattern="$1"
  local message="$2"
  if ! grep -Eq "${pattern}" "${publisher_log}" 2>/dev/null; then
    echo "[valet_parking_smoke] ${message}" >&2
    validation_status=8
  fi
}

require_command_log() {
  local pattern="$1"
  local message="$2"
  if ! grep -Eq "${pattern}" "${command_publisher_log}" 2>/dev/null; then
    echo "[valet_parking_smoke] ${message}" >&2
    validation_status=8
  fi
}

reject_runner_log() {
  local pattern="$1"
  local message="$2"
  if grep -Eq "${pattern}" "${runner_log}" 2>/dev/null; then
    echo "[valet_parking_smoke] ${message}" >&2
    validation_status=8
  fi
}

aux_mode_expects_path_provider_early_stop() {
  [[ "${with_aux_inputs}" == "1" ]] || return 1
  case "${aux_mode}" in
    far-localization|far-obstacles|many-obstacles)
      return 0
      ;;
    *)
      return 1
      ;;
  esac
}

case "${slot_mode}" in
  valid)
    if [[ "${command_mode}" == "none" ]]; then
      if [[ "${expect_path_provider_timeout}" == "1" ]]; then
        require_runner_log "PATH_PROVIDER failed: OpenSpacePathProvider target plan timeout.*provider_status=TARGET_TIMEOUT.*target_timeout=true.*target_cancel=true" \
          "missing expected threaded OpenSpacePathProvider timeout evidence"
        require_runner_log "fallback to ROI seed" \
          "missing controlled ROI-seed fallback after PATH_PROVIDER timeout"
        require_runner_log "STAGE_OUTPUT fallback.*fallback_event=path_provider_failed.*fallback_action=publish_roi_seed.*runtime_lifecycle_event=path_provider_fallback" \
          "missing Stage fallback lifecycle evidence after PATH_PROVIDER timeout"
        require_subscriber_log "is_estop=false" \
          "missing subscriber output after PATH_PROVIDER timeout fallback"
      elif [[ "${force_roi_decider_fail}" == "1" ]]; then
        require_runner_log "ROI_DECIDER failed: forced_by_smoke_env.*task_contract=lightweight_open_space_task_projection.*task_contract_record=roi_decider_output" \
          "missing forced ROI_DECIDER failure task projection"
        require_runner_log "STAGE_OUTPUT fallback.*fallback_event=roi_decider_failed.*fallback_action=publish_estop.*runtime_lifecycle_event=roi_decider_failed_fallback" \
          "missing Stage fallback lifecycle evidence after ROI_DECIDER failure"
        require_subscriber_log "is_estop=true" \
          "missing subscriber estop after forced ROI_DECIDER failure"
      elif [[ "${force_path_partition_fail}" == "1" ]]; then
        require_runner_log "PATH_PROVIDER ok.*threaded=true.*provider_status=TARGET_READY.*target_source=target_thread" \
          "missing threaded OpenSpacePathProvider target plan evidence before forced PathPartition failure"
        require_runner_log "PATH_PARTITION failed: forced_by_smoke_env.*task_contract=lightweight_open_space_task_projection.*task_contract_record=path_partition_output" \
          "missing forced PathPartition failure task projection"
        require_runner_log "fallback to PATH_PROVIDER" \
          "missing controlled PathProvider fallback after PathPartition failure"
        require_runner_log "STAGE_OUTPUT fallback.*fallback_event=path_partition_failed.*fallback_action=publish_path_provider_path.*runtime_lifecycle_event=path_partition_fallback" \
          "missing Stage fallback lifecycle evidence after PathPartition failure"
        require_subscriber_log "is_estop=false" \
          "missing subscriber output after PathPartition fallback"
      elif [[ "${force_speed_optimizer_fail}" == "1" ]]; then
        require_runner_log "PATH_PROVIDER ok.*threaded=true.*provider_status=TARGET_READY.*target_source=target_thread" \
          "missing threaded OpenSpacePathProvider target plan evidence before forced SpeedOptimizer failure"
        require_runner_log "PATH_PARTITION ok.*task_contract=lightweight_open_space_task_projection.*task_contract_record=path_partition_output" \
          "missing PathPartition task projection before forced SpeedOptimizer failure"
        require_runner_log "SPEED_OPTIMIZER failed: forced_by_smoke_env.*task_contract=lightweight_open_space_task_projection.*task_contract_record=speed_optimizer_output" \
          "missing forced SpeedOptimizer failure task projection"
        require_runner_log "fallback to PATH_PARTITION" \
          "missing controlled PathPartition fallback after SpeedOptimizer failure"
        require_runner_log "STAGE_OUTPUT fallback.*fallback_event=speed_optimizer_failed.*fallback_action=publish_path_partition_path.*runtime_lifecycle_event=speed_optimizer_fallback" \
          "missing Stage fallback lifecycle evidence after SpeedOptimizer failure"
        require_runner_log "STAGE_OUTPUT open_space.*speed_optimizer=fallback.*trajectory_type=SPEED_FALLBACK" \
          "missing open-space Stage fallback projection after SpeedOptimizer failure"
        require_subscriber_log "is_estop=false" \
          "missing subscriber output after SpeedOptimizer fallback"
      elif aux_mode_expects_path_provider_early_stop; then
        :
      else
        require_runner_log "PATH_PROVIDER ok.*threaded=true.*provider_status=TARGET_READY.*target_source=target_thread" \
          "missing threaded OpenSpacePathProvider target plan evidence"
        require_runner_log "ROI_DECIDER ok.*task_contract=lightweight_open_space_task_projection.*task_contract_record=roi_decider_output" \
          "missing ROI task projection contract"
        require_runner_log "ROI_DECIDER ok.*open_space_info_contract=roi_output.*open_space_path_info_id=1.*dest_region_points=[0-9]+.*dest_region_area=[0-9]" \
          "missing ROI/OpenSpaceInfo minimal contract evidence"
        require_runner_log "PATH_PROVIDER_PRECHECK ok.*task_contract=lightweight_open_space_task_projection.*task_contract_record=path_provider_precheck" \
          "missing PathProvider PreCheck task projection contract"
        require_runner_log "PATH_PROVIDER_PRECHECK ok.*collision_contract=geometry_precheck_only.*collision_input_source=roi_and_external_segments.*wheel_mask_contract=not_exposed_in_current_mvp.*wheel_mask_input_source=none.*wheel_mask_idl_extension=required_before_vehicle_integration" \
          "missing PreCheck collision/wheel-mask observable contract"
        require_runner_log "PATH_PROVIDER ok.*task_contract=lightweight_open_space_task_projection.*task_contract_record=path_provider_output" \
          "missing PathProvider task projection contract"
        require_runner_log "PATH_PROVIDER ok.*open_space_info_contract=path_provider_output.*path_info_id=1.*dest_region_points=[0-9]+.*dest_region_area=[0-9].*planning_context_contract=path_provider_runtime_projection.*path_history_state=(generated|reused).*planning_context_path_id=1.*planning_context_replan_reason=.*target_update_writeback=(true|false)" \
          "missing PathProvider/OpenSpaceInfo and PlanningContext path writeback evidence"
        require_runner_log "PATH_PARTITION ok.*task_contract=lightweight_open_space_task_projection.*task_contract_record=path_partition_output" \
          "missing PathPartition task projection contract"
        require_runner_log "PATH_PARTITION ok.*decision_name=.*finish_name=.*destination_reached=(true|false)" \
          "missing PathPartition decision/finish/destination evidence"
        require_runner_log "PATH_PARTITION ok.*open_space_info_contract=path_partition_output.*chosen_path_contract=chosen_partitioned_path.*chosen_path_points=[0-9]+.*chosen_path_gear=[0-9]+" \
          "missing PathPartition/OpenSpaceInfo chosen path evidence"
        require_runner_log "SPEED_OPTIMIZER ok.*task_contract=lightweight_open_space_task_projection.*task_contract_record=speed_optimizer_output" \
          "missing SpeedOptimizer task projection contract"
        require_runner_log "SPEED_OPTIMIZER ok.*stage_name=(INIT|RUNNING|WAITOBSTACLE|WAITREPLAN)" \
          "missing SpeedOptimizer interactive stage name"
        require_runner_log "SPEED_OPTIMIZER ok.*open_space_info_contract=speed_optimizer_output.*chosen_path_points=[0-9]+.*stop_path=(true|false).*speed_optimizer_trajectory_points=[0-9]+.*wheel_mask_considered=false" \
          "missing SpeedOptimizer/OpenSpaceInfo trajectory contract"
      require_runner_log "STAGE_OUTPUT open_space.*original_flow_branch=normal_open_space.*task_chain=ROI_DECIDER>PATH_PROVIDER>PATH_PARTITION>SPEED_OPTIMIZER.*path_decision=.*finish_status=.*destination_reached=(true|false).*target_gear=[0-9]+.*trajectory_type=(NORMAL|SHORT_PATH).*parking_status=(running|wait_obstacle|wait_replan|prepare_finish|mission_finished|stop_by_path_partition).*finish_priority=finish_over_interactive" \
        "missing Stage output contract aligned with ValetParkingStageParking flow"
        require_runner_log "STAGE_OUTPUT open_space.*stage_contract=lightweight_valet_parking_stage_projection.*stage_contract_record=open_space_output.*status_transport=replan_reason_text.*dds_field_extension=required_before_vehicle_integration" \
          "missing centralized Stage text-contract boundary"
        require_runner_log "STAGE_OUTPUT open_space.*stage_process_contract=lightweight_stage_skeleton.*stage_process_methods=Process>SetParkingType>ExecuteTaskOnOpenSpace>IsReadyToFinishStage>FinishScenario.*frame_lite_contract=stub_frame_bridge.*open_space_info_lite_contract=stub_open_space_bridge.*planning_context_lite_contract=stub_planning_context_bridge" \
          "missing lightweight Stage skeleton/Frame/OpenSpaceInfo/PlanningContext bridge contract"
        require_runner_log "STAGE_OUTPUT open_space.*mission_state_contract=lightweight_stage_projection.*mission_state=(MISSION_RUNNING|WAIT_OBSTACLE|WAIT_REPLAN|PREPARE_FINISH|STOP_BY_PATH_PARTITION|MISSION_FINISHED).*next_stage=(PARKING|FINISH).*finish_scenario_intent=(true|false).*finish_scenario_contract=diagnostic_only" \
          "missing lightweight MissionState/next_stage/FinishScenario contract"
        require_runner_log "STAGE_OUTPUT open_space.*finish_condition=destination_reached_and_standstill.*finish_ready=(true|false).*finish_consecutive_frames=[0-9]+.*finish_required_frames=[0-9]+.*vehicle_standstill=(true|false).*stage_finish_state=(READY|HOLDING|WAITING)" \
          "missing lightweight IsReadyToFinishStage state evidence"
        require_runner_log "STAGE_OUTPUT open_space.*function_manager_source=selected_slot.*function_manager_sys_mode=RPA.*function_manager_sys_command=PARKINCONTROL.*function_manager_sys_run_state=(PARKSTART|PARKING).*function_manager_sys_warning_info=NO_WARNING.*function_manager_parking_type=PARKING_IN" \
          "missing FunctionManager projection evidence for normal open-space branch"
        require_runner_log "STAGE_OUTPUT open_space.*runtime_lifecycle_contract=lightweight_stage_runtime_projection.*runtime_lifecycle_event=normal_open_space.*stage_exit_action=continue_parking.*path_history_action=keep_for_reuse.*speed_frame_action=keep_for_speed_warm_start.*direct_state_action=already_clear" \
          "missing normal open-space runtime lifecycle evidence"
        reject_runner_log "provider_status=TARGET_TIMEOUT" \
          "unexpected threaded OpenSpacePathProvider timeout"
      fi
    fi
    ;;
  target-moves)
    require_publisher_log "published sample [1-3]/[0-9]+ .*target=base" \
      "missing base target samples in target-moves slot mode"
    require_publisher_log "published sample [4-6]/[0-9]+ .*target=moved" \
      "missing moved target samples in target-moves slot mode"
    require_runner_log "PATH_PROVIDER ok.*history=generated, replan=TARGET_UPDATE.*reason=target_update" \
      "missing generated path after selected slot target update"
    require_runner_log "PATH_PROVIDER ok.*replan=TARGET_UPDATE.*planning_context_contract=path_provider_runtime_projection.*target_update_writeback=true" \
      "missing PlanningContext target update writeback after moved selected slot"
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
    require_runner_log "PATH_PROVIDER ok.*replan=TARGET_UPDATE.*planning_context_contract=path_provider_runtime_projection.*target_update_writeback=true" \
      "missing PlanningContext target update writeback after parking_seq/path_id update"
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
    require_runner_log "PATH_PROVIDER ok.*parking_seq=1.*preplan_candidates=1.*thread_path_ids=\\[[^]]*2" \
      "missing threaded preplan candidate for non-selected parking_seq=2"
    require_runner_log "PATH_PROVIDER ok.*parking_seq=2.*history=generated, replan=TARGET_UPDATE.*reason=target_update" \
      "missing generated path after opt_parking_seq selects parking_seq=2"
    require_runner_log "PATH_PROVIDER ok.*parking_seq=2.*replan=TARGET_UPDATE.*planning_context_contract=path_provider_runtime_projection.*target_update_writeback=true" \
      "missing PlanningContext target update writeback after multi-lot seq switch"
    require_runner_log "PATH_PROVIDER ok.*parking_seq=2.*preplan_candidates=1.*thread_path_ids=\\[[^]]*1" \
      "missing threaded preplan candidate for non-selected parking_seq=1"
    require_runner_log "PATH_PROVIDER ok.*parking_seq=2.*target_source=preplan_candidate" \
      "missing target reuse of preplanned candidate for selected parking_seq=2"
    require_runner_log "PATH_PROVIDER ok.*parking_seq=2.*history=reused, replan=NONE.*generated_count=2" \
      "missing history reuse after selected parking_seq=2 becomes stable"
    require_subscriber_log "is_estop=false" \
      "missing non-estop trajectory for multi-lot-seq-switch slot mode"
    ;;
  degenerate-corners)
    require_runner_log "selected parking lot corner geometry is degenerate" \
      "missing degenerate corner geometry rejection in runner log"
    require_runner_log "STAGE_OUTPUT fallback.*fallback_event=parking_lot_convert_failed.*fallback_action=publish_estop.*runtime_lifecycle_event=parking_lot_convert_failed_fallback" \
      "missing Stage fallback lifecycle evidence for degenerate-corners slot mode"
    require_runner_log "estop=true" \
      "missing runner estop for degenerate-corners slot mode"
    require_subscriber_log "is_estop=true" \
      "missing subscriber estop for degenerate-corners slot mode"
    ;;
  empty|overflow|nan|missing-selected-lot)
    case "${slot_mode}" in
      empty)
        require_runner_log "selected_slot.is_valid is false" \
          "missing selected_slot invalid rejection for empty slot mode"
        require_runner_log "STAGE_OUTPUT fallback.*fallback_event=selected_slot_invalid.*fallback_action=publish_estop.*runtime_lifecycle_event=selected_slot_invalid_fallback" \
          "missing Stage fallback lifecycle evidence for empty slot mode"
        ;;
      overflow)
        require_runner_log "selected_slot count exceeds parking_lots size" \
          "missing selected_slot count overflow rejection"
        require_runner_log "STAGE_OUTPUT fallback.*fallback_event=selected_slot_count_overflow.*fallback_action=publish_estop.*runtime_lifecycle_event=selected_slot_count_overflow_fallback" \
          "missing Stage fallback lifecycle evidence for overflow slot mode"
        ;;
      nan)
        require_runner_log "STAGE_OUTPUT fallback.*fallback_event=parking_lot_convert_failed.*fallback_action=publish_estop.*runtime_lifecycle_event=parking_lot_convert_failed_fallback" \
          "missing Stage fallback lifecycle evidence for nan slot mode"
        ;;
      missing-selected-lot)
        require_runner_log "selected_slot opt_parking_seq is unavailable.*opt_parking_seq=99.*available_parking_seq=\\[1\\]" \
          "missing selected_lot_unavailable rejection for missing-selected-lot mode"
        require_runner_log "STAGE_OUTPUT fallback.*fallback_event=selected_lot_unavailable.*fallback_action=publish_estop.*original_flow_branch=normal_open_space_early_input.*runtime_lifecycle_event=selected_lot_unavailable_fallback" \
          "missing Stage fallback lifecycle evidence for missing-selected-lot mode"
        reject_runner_log "ROI_DECIDER ok" \
          "missing-selected-lot should fail before ROI_DECIDER"
        ;;
    esac
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

if [[ "${command_mode}" != "none" ]]; then
  if [[ "${command_reset_history}" == "1" ]]; then
    require_command_log "reset_history=true" \
      "missing reset_history=true in command publisher log"
    require_runner_log "command #[0-9]+ mode=.*reset_history=true" \
      "missing reset_history=true in runner command consumption log"
    require_runner_log "command reset_history consumed mode=.*\\(one_shot\\)" \
      "missing one-shot reset_history consumption log"
    require_runner_log "(STAGE_CONTROL .*reset_history=true|STAGE_OUTPUT open_space.*function_manager_reset_history=true)" \
      "missing reset_history=true in stage-control or Stage output reason"
  fi
  if ((pre_command_slot_count > 0)); then
    require_publisher_log "publishing selected_slot before command" \
      "missing pre-command SelectedSlot publisher marker"
    require_runner_log "PATH_PROVIDER ok.*history=generated.*generated_count=1" \
      "missing pre-command generated path history"
  fi

  case "${command_mode}" in
    parking-in)
      require_command_log "enum=PARKING_IN" \
        "missing PARKING_IN command publisher evidence"
      require_runner_log "command #[0-9]+ mode=PARKING_IN" \
        "missing PARKING_IN command consumption in runner log"
      require_runner_log "STAGE_OUTPUT open_space.*function_manager_source=parking_command.*function_manager_sys_mode=RPA.*function_manager_sys_command=PARKINCONTROL.*function_manager_sys_run_state=(PARKSTART|PARKING).*function_manager_sys_warning_info=NO_WARNING.*function_manager_parking_type=PARKING_IN" \
        "missing PARKING_IN FunctionManager projection evidence"
      require_subscriber_log "is_estop=false" \
        "missing non-estop trajectory with PARKING_IN command mode"
      ;;
    direct-forward)
      require_command_log "enum=DIRECT_FORWARD" \
        "missing DIRECT_FORWARD command publisher evidence"
      require_runner_log "command #[0-9]+ mode=DIRECT_FORWARD" \
        "missing DIRECT_FORWARD command consumption in runner log"
      require_runner_log "STAGE_CONTROL DIRECT_FORWARD.*skip=ROI_PATH_PROVIDER_PATH_PARTITION" \
        "missing DIRECT_FORWARD stage-control trajectory reason"
      require_runner_log "STAGE_CONTROL DIRECT_FORWARD.*command_action=DIRECT_FORWARD.*stage_status=direct_control.*task=OPEN_SPACE_STRAIGHT_PATH.*original_flow_branch=direct_open_space" \
        "missing DIRECT_FORWARD standardized stage-control contract"
      require_runner_log "STAGE_CONTROL DIRECT_FORWARD.*stage_contract=lightweight_valet_parking_stage_projection.*stage_contract_record=stage_control.*status_transport=replan_reason_text" \
        "missing DIRECT_FORWARD centralized Stage control contract"
      require_runner_log "STAGE_CONTROL DIRECT_FORWARD.*stage_process_contract=lightweight_stage_skeleton.*stage_skeleton_branch=direct_open_space.*stage_skeleton_task_chain=OPEN_SPACE_STRAIGHT_PATH>SPEED_OPTIMIZER.*frame_lite_contract=stub_frame_bridge.*open_space_info_lite_contract=stub_open_space_bridge.*planning_context_lite_contract=stub_planning_context_bridge" \
        "missing DIRECT_FORWARD lightweight Stage skeleton bridge contract"
      require_runner_log "STAGE_CONTROL DIRECT_FORWARD.*function_manager_source=parking_command.*function_manager_sys_command=FORWARDCONTROL.*function_manager_sys_run_state=PARKING.*function_manager_sys_warning_info=NO_WARNING.*function_manager_parking_type=DIRECT_FORWARD" \
        "missing DIRECT_FORWARD FunctionManager projection evidence"
      require_runner_log "STAGE_CONTROL DIRECT_FORWARD.*mission_state_contract=lightweight_stage_projection.*mission_state=DIRECT_CONTROL_ACTIVE.*next_stage=PARKING.*finish_scenario_intent=false" \
        "missing DIRECT_FORWARD active MissionState lifecycle evidence"
      require_runner_log "STAGE_CONTROL DIRECT_FORWARD.*finish_condition=direct_command_inactive_and_standstill.*direct_command_active=true.*direct_command_inactive=false.*direct_finish_ready=false.*direct_stage_finish_state=WAITING" \
        "missing DIRECT_FORWARD direct finish inactive-command contract"
      require_runner_log "STAGE_CONTROL DIRECT_FORWARD.*runtime_lifecycle_contract=lightweight_stage_runtime_projection.*runtime_lifecycle_event=direct_control_active.*stage_exit_action=continue_direct_control.*direct_state_action=keep_direct_command" \
        "missing DIRECT_FORWARD active runtime lifecycle evidence"
      if [[ "${force_straight_path_fail}" == "1" ]]; then
        require_runner_log "STAGE_OUTPUT fallback.*parking_status=direct_path_fallback_stop" \
          "missing DIRECT_FORWARD straight path fallback parking_status"
      else
        require_runner_log "STAGE_CONTROL DIRECT_FORWARD.*trajectory_type=NORMAL.*parking_status=direct_(stop_path|moving)" \
          "missing DIRECT_FORWARD trajectory_type/parking_status diagnostics"
      fi
      require_runner_log "OPEN_SPACE_STRAIGHT_PATH" \
        "missing DIRECT_FORWARD OPEN_SPACE_STRAIGHT_PATH execution evidence"
      if [[ "${force_straight_path_fail}" == "1" ]]; then
        require_runner_log "OPEN_SPACE_STRAIGHT_PATH failed: forced_by_smoke_env.*task_contract=lightweight_direct_task_projection.*task_contract_record=open_space_straight_path_output.*task_contract_chain=OPEN_SPACE_STRAIGHT_PATH>SPEED_OPTIMIZER" \
          "missing forced DIRECT_FORWARD straight path failure evidence"
        require_runner_log "STAGE_OUTPUT fallback.*fallback_event=open_space_straight_path_failed.*fallback_action=publish_stage_control_stop.*runtime_lifecycle_event=direct_straight_path_fallback" \
          "missing DIRECT_FORWARD straight path Stage fallback evidence"
        reject_runner_log "SPEED_OPTIMIZER ok" \
          "DIRECT_FORWARD straight path failure should not run SpeedOptimizer"
        require_subscriber_log "points=1" \
          "DIRECT_FORWARD straight path failure should publish one-point stop"
      elif [[ "${force_speed_optimizer_fail}" == "1" ]]; then
        require_runner_log "SPEED_OPTIMIZER failed: forced_by_smoke_env.*task_contract=lightweight_direct_task_projection.*task_contract_record=direct_speed_optimizer_output.*task_contract_chain=OPEN_SPACE_STRAIGHT_PATH>SPEED_OPTIMIZER" \
          "missing forced DIRECT_FORWARD speed optimizer failure evidence"
        require_runner_log "fallback to OPEN_SPACE_STRAIGHT_PATH" \
          "missing DIRECT_FORWARD fallback to straight path after speed failure"
        require_runner_log "STAGE_OUTPUT fallback.*fallback_event=direct_speed_optimizer_failed.*fallback_action=(publish_open_space_straight_path|publish_stage_control_stop).*runtime_lifecycle_event=direct_speed_optimizer_fallback" \
          "missing DIRECT_FORWARD speed fallback Stage evidence"
      else
        require_runner_log "SPEED_OPTIMIZER ok.*task_contract=lightweight_direct_task_projection.*task_contract_record=direct_speed_optimizer_output.*task_contract_chain=OPEN_SPACE_STRAIGHT_PATH>SPEED_OPTIMIZER" \
          "missing DIRECT_FORWARD speed optimizer handoff evidence"
        require_runner_log "direct_speed_bound_max=${direct_speed}" \
          "missing DIRECT_FORWARD direct speed bound mapping evidence"
      fi
      reject_runner_log "ROI_DECIDER ok" \
        "DIRECT_FORWARD should not run ROI_DECIDER in the OPEN_SPACE_STRAIGHT_PATH branch"
      if [[ "${direct_velocity_conflict_expected}" == "1" &&
            "${force_straight_path_fail}" != "1" ]]; then
        require_runner_log "OPEN_SPACE_STRAIGHT_PATH stop_path.*velocity_direction_conflict" \
          "missing DIRECT_FORWARD velocity direction conflict stop_path evidence"
        require_subscriber_log "length=0" \
          "DIRECT_FORWARD velocity conflict should publish a zero-length stop path"
      fi
      require_subscriber_log "is_estop=false" \
        "missing non-estop trajectory for DIRECT_FORWARD command mode"
      require_subscriber_log "reason: replan=.*DIRECT_FORWARD.*OPEN_SPACE_STRAIGHT_PATH" \
        "missing DIRECT_FORWARD OPEN_SPACE_STRAIGHT_PATH reason in subscriber output"
      ;;
    direct-forward-release)
      require_command_log "enum=DIRECT_FORWARD" \
        "missing DIRECT_FORWARD command publisher evidence for release flow"
      require_command_log "enum=NONE" \
        "missing command clear publisher evidence for DIRECT_FORWARD release flow"
      require_runner_log "command #[0-9]+ mode=DIRECT_FORWARD" \
        "missing DIRECT_FORWARD command consumption in release flow"
      require_runner_log "command #[0-9]+ mode=NONE \\(cleared_command\\)" \
        "missing command clear consumption in DIRECT_FORWARD release flow"
      require_runner_log "STAGE_CONTROL DIRECT_FORWARD.*mission_state=DIRECT_CONTROL_ACTIVE.*direct_command_active=true.*direct_finish_ready=false" \
        "missing DIRECT_FORWARD active half before release"
      require_runner_log "STAGE_CONTROL DIRECT_FORWARD_RELEASED.*command_action=DIRECT_FORWARD_RELEASED.*stage_status=mission_finished.*task=DIRECT_COMMAND_RELEASE" \
        "missing DIRECT_FORWARD_RELEASED stage-control branch"
      require_runner_log "STAGE_CONTROL DIRECT_FORWARD_RELEASED.*stage_contract=lightweight_valet_parking_stage_projection.*stage_contract_record=stage_control.*status_transport=replan_reason_text" \
        "missing DIRECT_FORWARD_RELEASED centralized Stage control contract"
      require_runner_log "STAGE_CONTROL DIRECT_FORWARD_RELEASED.*function_manager_source=cleared_direct_command.*function_manager_sys_command=FORWARDCONTROL.*function_manager_sys_run_state=QUIT.*function_manager_parking_type=DIRECT_FORWARD.*function_manager_command=NONE" \
        "missing released DIRECT_FORWARD FunctionManager projection evidence"
      require_runner_log "STAGE_CONTROL DIRECT_FORWARD_RELEASED.*mission_state_contract=lightweight_stage_projection.*mission_state=DIRECT_FINISH_READY.*next_stage=FINISH.*finish_scenario_intent=true.*finish_scenario_contract=diagnostic_only" \
        "missing DIRECT_FORWARD release MissionState/next_stage evidence"
      require_runner_log "STAGE_CONTROL DIRECT_FORWARD_RELEASED.*finish_condition=direct_command_inactive_and_standstill.*direct_command_active=false.*direct_command_inactive=true.*direct_finish_ready=true.*direct_stage_finish_state=READY.*previous_direct_command=DIRECT_FORWARD.*trajectory_type=SHORT_PATH.*parking_status=mission_finished" \
        "missing DIRECT_FORWARD inactive-command finish-ready contract"
      require_runner_log "STAGE_CONTROL DIRECT_FORWARD_RELEASED.*runtime_lifecycle_contract=lightweight_stage_runtime_projection.*runtime_lifecycle_event=direct_release_ready.*stage_exit_action=latch_finish_hold_after_publish.*path_history_action=keep_until_stage_reset.*speed_frame_action=keep_until_stage_reset.*direct_state_action=clear_released_command" \
        "missing DIRECT_FORWARD release runtime latch evidence"
      require_runner_log "STAGE_CONTROL FINISH_HOLD.*stage_status=mission_finished.*task=STAGE_FINISH_HOLD" \
        "missing FINISH_HOLD branch after DIRECT_FORWARD release"
      if ((pre_command_slot_count > 0)); then
        require_runner_log "STAGE_CONTROL DIRECT_FORWARD_RELEASED.*path_history_available=true.*path_history_action=keep_until_stage_reset" \
          "missing DIRECT_FORWARD release hold of existing path history"
      fi
      require_runner_log "STAGE_CONTROL DIRECT_FORWARD_RELEASED.*skip=ROI_PATH_PROVIDER_PATH_PARTITION" \
        "missing DIRECT_FORWARD_RELEASED skip evidence for ROI/PathProvider/PathPartition"
      require_subscriber_log "is_estop=false" \
        "missing non-estop trajectory for DIRECT_FORWARD release command mode"
      ;;
    direct-backward)
      require_command_log "enum=DIRECT_BACKWARD" \
        "missing DIRECT_BACKWARD command publisher evidence"
      require_runner_log "command #[0-9]+ mode=DIRECT_BACKWARD" \
        "missing DIRECT_BACKWARD command consumption in runner log"
      require_runner_log "STAGE_CONTROL DIRECT_BACKWARD.*skip=ROI_PATH_PROVIDER_PATH_PARTITION" \
        "missing DIRECT_BACKWARD stage-control trajectory reason"
      require_runner_log "STAGE_CONTROL DIRECT_BACKWARD.*command_action=DIRECT_BACKWARD.*stage_status=direct_control.*task=OPEN_SPACE_STRAIGHT_PATH.*original_flow_branch=direct_open_space" \
        "missing DIRECT_BACKWARD standardized stage-control contract"
      require_runner_log "STAGE_CONTROL DIRECT_BACKWARD.*stage_contract=lightweight_valet_parking_stage_projection.*stage_contract_record=stage_control.*status_transport=replan_reason_text" \
        "missing DIRECT_BACKWARD centralized Stage control contract"
      require_runner_log "STAGE_CONTROL DIRECT_BACKWARD.*stage_process_contract=lightweight_stage_skeleton.*stage_skeleton_branch=direct_open_space.*stage_skeleton_task_chain=OPEN_SPACE_STRAIGHT_PATH>SPEED_OPTIMIZER.*frame_lite_contract=stub_frame_bridge.*open_space_info_lite_contract=stub_open_space_bridge.*planning_context_lite_contract=stub_planning_context_bridge" \
        "missing DIRECT_BACKWARD lightweight Stage skeleton bridge contract"
      require_runner_log "STAGE_CONTROL DIRECT_BACKWARD.*function_manager_source=parking_command.*function_manager_sys_command=BACKWARDCONTROL.*function_manager_sys_run_state=PARKING.*function_manager_sys_warning_info=NO_WARNING.*function_manager_parking_type=DIRECT_BACKWARD" \
        "missing DIRECT_BACKWARD FunctionManager projection evidence"
      require_runner_log "STAGE_CONTROL DIRECT_BACKWARD.*mission_state_contract=lightweight_stage_projection.*mission_state=DIRECT_CONTROL_ACTIVE.*next_stage=PARKING.*finish_scenario_intent=false" \
        "missing DIRECT_BACKWARD active MissionState lifecycle evidence"
      require_runner_log "STAGE_CONTROL DIRECT_BACKWARD.*finish_condition=direct_command_inactive_and_standstill.*direct_command_active=true.*direct_command_inactive=false.*direct_finish_ready=false.*direct_stage_finish_state=WAITING" \
        "missing DIRECT_BACKWARD direct finish inactive-command contract"
      require_runner_log "STAGE_CONTROL DIRECT_BACKWARD.*runtime_lifecycle_contract=lightweight_stage_runtime_projection.*runtime_lifecycle_event=direct_control_active.*stage_exit_action=continue_direct_control.*direct_state_action=keep_direct_command" \
        "missing DIRECT_BACKWARD active runtime lifecycle evidence"
      if [[ "${force_straight_path_fail}" == "1" ]]; then
        require_runner_log "STAGE_OUTPUT fallback.*parking_status=direct_path_fallback_stop" \
          "missing DIRECT_BACKWARD straight path fallback parking_status"
      else
        require_runner_log "STAGE_CONTROL DIRECT_BACKWARD.*trajectory_type=NORMAL.*parking_status=direct_(stop_path|moving)" \
          "missing DIRECT_BACKWARD trajectory_type/parking_status diagnostics"
      fi
      require_runner_log "OPEN_SPACE_STRAIGHT_PATH" \
        "missing DIRECT_BACKWARD OPEN_SPACE_STRAIGHT_PATH execution evidence"
      if [[ "${force_straight_path_fail}" == "1" ]]; then
        require_runner_log "OPEN_SPACE_STRAIGHT_PATH failed: forced_by_smoke_env.*task_contract=lightweight_direct_task_projection.*task_contract_record=open_space_straight_path_output.*task_contract_chain=OPEN_SPACE_STRAIGHT_PATH>SPEED_OPTIMIZER" \
          "missing forced DIRECT_BACKWARD straight path failure evidence"
        require_runner_log "STAGE_OUTPUT fallback.*fallback_event=open_space_straight_path_failed.*fallback_action=publish_stage_control_stop.*runtime_lifecycle_event=direct_straight_path_fallback" \
          "missing DIRECT_BACKWARD straight path Stage fallback evidence"
        reject_runner_log "SPEED_OPTIMIZER ok" \
          "DIRECT_BACKWARD straight path failure should not run SpeedOptimizer"
        require_subscriber_log "points=1" \
          "DIRECT_BACKWARD straight path failure should publish one-point stop"
      elif [[ "${force_speed_optimizer_fail}" == "1" ]]; then
        require_runner_log "SPEED_OPTIMIZER failed: forced_by_smoke_env.*task_contract=lightweight_direct_task_projection.*task_contract_record=direct_speed_optimizer_output.*task_contract_chain=OPEN_SPACE_STRAIGHT_PATH>SPEED_OPTIMIZER" \
          "missing forced DIRECT_BACKWARD speed optimizer failure evidence"
        require_runner_log "fallback to OPEN_SPACE_STRAIGHT_PATH" \
          "missing DIRECT_BACKWARD fallback to straight path after speed failure"
        require_runner_log "STAGE_OUTPUT fallback.*fallback_event=direct_speed_optimizer_failed.*fallback_action=(publish_open_space_straight_path|publish_stage_control_stop).*runtime_lifecycle_event=direct_speed_optimizer_fallback" \
          "missing DIRECT_BACKWARD speed fallback Stage evidence"
      else
        require_runner_log "SPEED_OPTIMIZER ok.*task_contract=lightweight_direct_task_projection.*task_contract_record=direct_speed_optimizer_output.*task_contract_chain=OPEN_SPACE_STRAIGHT_PATH>SPEED_OPTIMIZER" \
          "missing DIRECT_BACKWARD speed optimizer handoff evidence"
        require_runner_log "direct_speed_bound_max=${direct_speed}" \
          "missing DIRECT_BACKWARD direct speed bound mapping evidence"
      fi
      reject_runner_log "ROI_DECIDER ok" \
        "DIRECT_BACKWARD should not run ROI_DECIDER in the OPEN_SPACE_STRAIGHT_PATH branch"
      if [[ "${direct_velocity_conflict_expected}" == "1" &&
            "${force_straight_path_fail}" != "1" ]]; then
        require_runner_log "OPEN_SPACE_STRAIGHT_PATH stop_path.*velocity_direction_conflict" \
          "missing DIRECT_BACKWARD velocity direction conflict stop_path evidence"
        require_subscriber_log "length=0" \
          "DIRECT_BACKWARD velocity conflict should publish a zero-length stop path"
      fi
      if [[ "${force_straight_path_fail}" != "1" ]]; then
        require_subscriber_log "gear=2" \
          "missing reverse gear in DIRECT_BACKWARD subscriber output"
      fi
      require_subscriber_log "is_estop=false" \
        "missing non-estop trajectory for DIRECT_BACKWARD command mode"
      require_subscriber_log "reason: replan=.*DIRECT_BACKWARD.*OPEN_SPACE_STRAIGHT_PATH" \
        "missing DIRECT_BACKWARD OPEN_SPACE_STRAIGHT_PATH reason in subscriber output"
      ;;
    direct-backward-release)
      require_command_log "enum=DIRECT_BACKWARD" \
        "missing DIRECT_BACKWARD command publisher evidence for release flow"
      require_command_log "enum=NONE" \
        "missing command clear publisher evidence for DIRECT_BACKWARD release flow"
      require_runner_log "command #[0-9]+ mode=DIRECT_BACKWARD" \
        "missing DIRECT_BACKWARD command consumption in release flow"
      require_runner_log "command #[0-9]+ mode=NONE \\(cleared_command\\)" \
        "missing command clear consumption in DIRECT_BACKWARD release flow"
      require_runner_log "STAGE_CONTROL DIRECT_BACKWARD.*mission_state=DIRECT_CONTROL_ACTIVE.*direct_command_active=true.*direct_finish_ready=false" \
        "missing DIRECT_BACKWARD active half before release"
      require_runner_log "STAGE_CONTROL DIRECT_BACKWARD_RELEASED.*command_action=DIRECT_BACKWARD_RELEASED.*stage_status=mission_finished.*task=DIRECT_COMMAND_RELEASE" \
        "missing DIRECT_BACKWARD_RELEASED stage-control branch"
      require_runner_log "STAGE_CONTROL DIRECT_BACKWARD_RELEASED.*stage_contract=lightweight_valet_parking_stage_projection.*stage_contract_record=stage_control.*status_transport=replan_reason_text" \
        "missing DIRECT_BACKWARD_RELEASED centralized Stage control contract"
      require_runner_log "STAGE_CONTROL DIRECT_BACKWARD_RELEASED.*function_manager_source=cleared_direct_command.*function_manager_sys_command=BACKWARDCONTROL.*function_manager_sys_run_state=QUIT.*function_manager_parking_type=DIRECT_BACKWARD.*function_manager_command=NONE" \
        "missing released DIRECT_BACKWARD FunctionManager projection evidence"
      require_runner_log "STAGE_CONTROL DIRECT_BACKWARD_RELEASED.*mission_state_contract=lightweight_stage_projection.*mission_state=DIRECT_FINISH_READY.*next_stage=FINISH.*finish_scenario_intent=true.*finish_scenario_contract=diagnostic_only" \
        "missing DIRECT_BACKWARD release MissionState/next_stage evidence"
      require_runner_log "STAGE_CONTROL DIRECT_BACKWARD_RELEASED.*finish_condition=direct_command_inactive_and_standstill.*direct_command_active=false.*direct_command_inactive=true.*direct_finish_ready=true.*direct_stage_finish_state=READY.*previous_direct_command=DIRECT_BACKWARD.*trajectory_type=SHORT_PATH.*parking_status=mission_finished" \
        "missing DIRECT_BACKWARD inactive-command finish-ready contract"
      require_runner_log "STAGE_CONTROL DIRECT_BACKWARD_RELEASED.*runtime_lifecycle_contract=lightweight_stage_runtime_projection.*runtime_lifecycle_event=direct_release_ready.*stage_exit_action=latch_finish_hold_after_publish.*path_history_action=keep_until_stage_reset.*speed_frame_action=keep_until_stage_reset.*direct_state_action=clear_released_command" \
        "missing DIRECT_BACKWARD release runtime latch evidence"
      require_runner_log "STAGE_CONTROL FINISH_HOLD.*stage_status=mission_finished.*task=STAGE_FINISH_HOLD" \
        "missing FINISH_HOLD branch after DIRECT_BACKWARD release"
      if ((pre_command_slot_count > 0)); then
        require_runner_log "STAGE_CONTROL DIRECT_BACKWARD_RELEASED.*path_history_available=true.*path_history_action=keep_until_stage_reset" \
          "missing DIRECT_BACKWARD release hold of existing path history"
      fi
      require_runner_log "STAGE_CONTROL DIRECT_BACKWARD_RELEASED.*skip=ROI_PATH_PROVIDER_PATH_PARTITION" \
        "missing DIRECT_BACKWARD_RELEASED skip evidence for ROI/PathProvider/PathPartition"
      require_subscriber_log "is_estop=false" \
        "missing non-estop trajectory for DIRECT_BACKWARD release command mode"
      ;;
    pause)
      require_command_log "enum=PAUSE" \
        "missing PAUSE command publisher evidence"
      require_runner_log "command #[0-9]+ mode=PAUSE" \
        "missing PAUSE command consumption in runner log"
      require_runner_log "STAGE_CONTROL PAUSE.*skip=ROI_PATH_PROVIDER_PATH_PARTITION" \
        "missing PAUSE stage-control reason"
      require_runner_log "STAGE_CONTROL PAUSE.*command_action=PAUSE.*stage_status=paused.*task=STAGE_CONTROL_STOP.*trajectory_type=SHORT_PATH.*parking_status=stage_control_stop" \
        "missing PAUSE standardized stop contract"
      require_runner_log "STAGE_CONTROL PAUSE.*function_manager_source=parking_command.*function_manager_sys_command=STOPPARKINROUTE.*function_manager_sys_run_state=PAUSE.*function_manager_sys_warning_info=SYSTEM_PAUSE_0x0B.*function_manager_parking_type=UNCHANGED" \
        "missing PAUSE FunctionManager projection evidence"
      require_runner_log "STAGE_CONTROL PAUSE.*mission_state_contract=lightweight_stage_projection.*mission_state=MISSION_PAUSED.*next_stage=PARKING.*finish_scenario_intent=false" \
        "missing PAUSE MissionState projection evidence"
      reject_runner_log "ROI_DECIDER ok" \
        "PAUSE should not run ROI_DECIDER"
      require_subscriber_log "points=1" \
        "missing one-point stop trajectory for PAUSE command mode"
      require_subscriber_log "is_estop=false" \
        "missing non-estop stop trajectory for PAUSE command mode"
      ;;
    brake)
      require_command_log "enum=BRAKE" \
        "missing BRAKE command publisher evidence"
      require_runner_log "command #[0-9]+ mode=BRAKE" \
        "missing BRAKE command consumption in runner log"
      require_runner_log "STAGE_CONTROL BRAKE.*skip=ROI_PATH_PROVIDER_PATH_PARTITION" \
        "missing BRAKE stage-control reason"
      require_runner_log "STAGE_CONTROL BRAKE.*command_action=BRAKE.*stage_status=braking.*task=STAGE_CONTROL_STOP.*trajectory_type=SHORT_PATH.*parking_status=stage_control_stop" \
        "missing BRAKE standardized stop contract"
      require_runner_log "STAGE_CONTROL BRAKE.*function_manager_source=parking_command.*function_manager_sys_command=BRAKECONTROL.*function_manager_sys_run_state=STRAIGHTBRAKE.*function_manager_sys_warning_info=NO_WARNING.*function_manager_parking_type=NOSTATE" \
        "missing BRAKE/NOSTATE FunctionManager projection evidence"
      require_runner_log "STAGE_CONTROL BRAKE.*mission_state_contract=lightweight_stage_projection.*mission_state=MISSION_BRAKING.*next_stage=PARKING.*finish_scenario_intent=false" \
        "missing BRAKE MissionState projection evidence"
      reject_runner_log "ROI_DECIDER ok" \
        "BRAKE should not run ROI_DECIDER"
      require_subscriber_log "points=1" \
        "missing one-point stop trajectory for BRAKE command mode"
      require_subscriber_log "is_estop=false" \
        "missing non-estop stop trajectory for BRAKE command mode"
      ;;
    finish)
      require_command_log "enum=FINISH" \
        "missing FINISH command publisher evidence"
      require_runner_log "command #[0-9]+ mode=FINISH" \
        "missing FINISH command consumption in runner log"
      require_runner_log "STAGE_CONTROL FINISH.*skip=ROI_PATH_PROVIDER_PATH_PARTITION.*MISSIONFINISHED" \
        "missing FINISH/MISSIONFINISHED stage-control reason"
      require_runner_log "STAGE_CONTROL FINISH.*command_action=FINISH.*stage_status=mission_finished.*task=STAGE_CONTROL_STOP.*finish_status=MISSIONFINISHED.*trajectory_type=SHORT_PATH.*parking_status=mission_finished" \
        "missing FINISH standardized stop contract"
      require_runner_log "STAGE_CONTROL FINISH.*function_manager_source=parking_command.*function_manager_sys_command=PARKINGFINISH.*function_manager_sys_run_state=QUIT.*function_manager_sys_warning_info=NO_WARNING.*function_manager_parking_type=NOSTATE" \
        "missing FINISH/NOSTATE FunctionManager projection evidence"
      require_runner_log "STAGE_CONTROL FINISH.*mission_state_contract=lightweight_stage_projection.*mission_state=MISSION_FINISHED.*next_stage=FINISH.*finish_scenario_intent=true" \
        "missing FINISH MissionState/FinishScenario projection evidence"
      reject_runner_log "ROI_DECIDER ok" \
        "FINISH should not run ROI_DECIDER"
      require_subscriber_log "points=1" \
        "missing one-point stop trajectory for FINISH command mode"
      require_subscriber_log "is_estop=false" \
        "missing non-estop stop trajectory for FINISH command mode"
      require_subscriber_log "reason: replan=.*MISSIONFINISHED" \
        "missing MISSIONFINISHED reason in subscriber output"
      ;;
    parking-out-left|parking-out-right|parking-out-front|parking-out-back)
      parking_out_enum=""
      case "${command_mode}" in
        parking-out-left)
          parking_out_enum="PARKING_OUT_LEFT"
          ;;
        parking-out-right)
          parking_out_enum="PARKING_OUT_RIGHT"
          ;;
        parking-out-front)
          parking_out_enum="PARKING_OUT_FRONT"
          ;;
        parking-out-back)
          parking_out_enum="PARKING_OUT_BACK"
          ;;
      esac
      require_command_log "enum=${parking_out_enum}" \
        "missing ${parking_out_enum} command publisher evidence"
      require_runner_log "command #[0-9]+ mode=${parking_out_enum}" \
        "missing ${parking_out_enum} command consumption in runner log"
      require_runner_log "STAGE_CONTROL ${parking_out_enum}.*command_action=${parking_out_enum}.*stage_status=unsupported.*task=UNSUPPORTED_PARKING_OUT.*trajectory_type=SHORT_PATH.*parking_status=parking_out_unsupported.*unsupported_in_mvp" \
        "missing ${parking_out_enum} unsupported parking-out contract"
      require_runner_log "STAGE_CONTROL ${parking_out_enum}.*function_manager_source=parking_command.*function_manager_sys_run_state=PARKOUT.*function_manager_sys_warning_info=NO_WARNING.*function_manager_parking_type=${parking_out_enum}" \
        "missing ${parking_out_enum} FunctionManager unsupported projection evidence"
      require_runner_log "STAGE_CONTROL ${parking_out_enum}.*mission_state_contract=lightweight_stage_projection.*mission_state=UNSUPPORTED_PARKING_OUT.*next_stage=PARKING.*finish_scenario_intent=false" \
        "missing ${parking_out_enum} MissionState unsupported projection evidence"
      reject_runner_log "ROI_DECIDER ok" \
        "${parking_out_enum} should not run ROI_DECIDER in MVP unsupported branch"
      require_subscriber_log "points=1" \
        "missing one-point stop trajectory for ${parking_out_enum}"
      require_subscriber_log "is_estop=false" \
        "missing non-estop stop trajectory for ${parking_out_enum}"
      require_subscriber_log "reason: replan=.*parking_out_unsupported" \
        "missing parking_out_unsupported reason in subscriber output"
      ;;
    invalid)
      require_command_log "is_valid=false" \
        "missing invalid command publisher evidence"
      require_runner_log "command #[0-9]+ mode=NONE \\(cleared_command\\)" \
        "missing invalid command clearing in runner log"
      reject_runner_log "STAGE_CONTROL" \
        "invalid command should clear command state instead of producing a stage-control branch"
      require_runner_log "STAGE_OUTPUT open_space.*function_manager_source=selected_slot.*function_manager_sys_command=PARKINCONTROL.*function_manager_sys_run_state=(PARKSTART|PARKING).*function_manager_parking_type=PARKING_IN.*function_manager_command=NONE" \
        "missing selected_slot/default FunctionManager projection after invalid command clear"
      require_subscriber_log "is_estop=false" \
        "missing normal non-estop trajectory after invalid command is cleared"
      ;;
    *)
      echo "[valet_parking_smoke] unknown command mode for validation: ${command_mode}" >&2
      validation_status=8
      ;;
  esac
fi

if [[ "${with_aux_inputs}" == "1" ]]; then
  require_log() {
    local pattern="$1"
    local message="$2"
    if ! grep -Eq "${pattern}" "${runner_log}" 2>/dev/null; then
      echo "[valet_parking_smoke] ${message}" >&2
      validation_status=8
    fi
  }

  require_aux_log() {
    local pattern="$1"
    local message="$2"
    if ! grep -Eq "${pattern}" "${aux_publisher_log}" 2>/dev/null; then
      echo "[valet_parking_smoke] ${message}" >&2
      validation_status=8
    fi
  }

  reject_log() {
    local pattern="$1"
    local message="$2"
    if grep -Eq "${pattern}" "${runner_log}" 2>/dev/null; then
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
      near-destination)
        require_aux_log "published sample group [1-6]/[0-9]+ localization=valid" \
          "missing initial origin aux publisher evidence before near-destination"
        require_aux_log "published sample group ([7-9]|1[0-4])/[0-9]+ localization=near-destination-valid" \
          "missing near-destination aux publisher evidence"
        require_log "aux localization #[0-9]+ \\(x=0(\\.00)?, y=0" \
          "missing initial origin localization consumption in near-destination mode"
        require_log "aux localization #[0-9]+ \\(x=7\\.31[0-9]*, y=1\\.76" \
          "missing near-destination localization consumption in runner log"
        require_log "aux chassis #[0-9]+ .*speed_mps=0" \
          "missing standstill chassis consumption in near-destination mode"
        require_log "PATH_PARTITION ok.*finish_name=REACH_TARGET.*destination_reached=true" \
          "missing PathPartition destination reached evidence"
        require_log "STAGE_OUTPUT open_space.*stage_status=mission_finished.*destination_reached=true.*trajectory_type=SHORT_PATH.*parking_status=mission_finished" \
          "missing mission_finished Stage output for destination reached"
        require_log "STAGE_OUTPUT open_space.*mission_state=MISSION_FINISHED.*next_stage=FINISH.*finish_scenario_intent=true" \
          "missing MissionState FINISH projection for destination reached"
        require_log "STAGE_OUTPUT open_space.*finish_ready=true.*finish_ready_condition=true.*finish_consecutive_frames=[2-9][0-9]*.*stage_finish_state=READY" \
          "missing consecutive-frame finish-ready evidence"
        require_log "STAGE_OUTPUT open_space.*runtime_lifecycle_event=stage_finish_ready.*stage_exit_action=latch_finish_hold_after_publish.*path_history_action=keep_until_stage_reset.*speed_frame_action=keep_until_stage_reset" \
          "missing finish-ready runtime lifecycle latch evidence"
        require_log "STAGE_CONTROL FINISH_HOLD.*function_manager_source=stage_finish_latched.*function_manager_sys_command=PARKINGFINISH.*function_manager_sys_run_state=QUIT.*function_manager_parking_type=PARKING_IN" \
          "missing FINISH_HOLD FunctionManager projection after Stage finish latch"
        require_log "STAGE_CONTROL FINISH_HOLD.*runtime_lifecycle_event=stage_finish_hold.*stage_exit_action=hold_until_reset_history_or_new_stage.*path_history_action=keep_until_stage_reset.*speed_frame_action=keep_until_stage_reset.*parking_status=mission_finished" \
          "missing FINISH_HOLD runtime lifecycle evidence"
        require_subscriber_log "is_estop=false" \
          "missing non-estop trajectory in near-destination finish mode"
        ;;
      far-localization)
        require_log "aux localization #[0-9]+ .*x=1000" \
          "missing far localization sample"
        require_log "vehicle_lot_precheck failed: vehicle outside selected lot envelope" \
          "missing vehicle_lot_precheck failure in far-localization mode"
        require_log "STAGE_OUTPUT fallback.*fallback_event=vehicle_lot_precheck_failed.*fallback_action=publish_estop.*runtime_lifecycle_event=vehicle_lot_precheck_failed_fallback" \
          "missing Stage fallback lifecycle evidence after vehicle-lot precheck failure"
        if ! grep -Eq "is_estop=true" "${subscriber_log}" 2>/dev/null; then
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
        require_log "PATH_PROVIDER_PRECHECK failed: obstacle_segment_outside_xy_bounds\\[[0-9]+\\].*collision_contract=geometry_precheck_only.*wheel_mask_contract=not_exposed_in_current_mvp" \
          "missing collision/wheel-mask contract on far-obstacles precheck failure"
        require_log "STAGE_OUTPUT fallback.*fallback_event=precheck_failed.*fallback_action=publish_estop.*runtime_lifecycle_event=precheck_fallback" \
          "missing Stage fallback lifecycle evidence after far-obstacles precheck failure"
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
        require_log "PATH_PROVIDER_PRECHECK failed: too_many_obstacle_segments=[0-9]+.*collision_contract=geometry_precheck_only.*wheel_mask_contract=not_exposed_in_current_mvp" \
          "missing collision/wheel-mask contract on many-obstacles precheck failure"
        require_log "STAGE_OUTPUT fallback.*fallback_event=precheck_failed.*fallback_action=publish_estop.*runtime_lifecycle_event=precheck_fallback" \
          "missing Stage fallback lifecycle evidence after many-obstacles precheck failure"
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

if [[ "${expect_thread_provider_stop}" == "1" ]]; then
  require_runner_log "OpenSpaceThreadManager stopped, search_threads=[1-9][0-9]*, target_thread_joined=true" \
    "missing OpenSpaceThreadManager stop/destructor evidence"
fi

exit "${validation_status}"
