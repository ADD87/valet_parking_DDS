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
  --command-mode MODE ParkingCommand publisher mode. Default: none.
                       none|parking-in|direct-forward|direct-backward|
                       pause|brake|finish|invalid
  --command-count N   ParkingCommand publish count. Default: 1.
  --command-interval-ms N
                       ParkingCommand publish interval. Default: 100.
  --direct-distance M Direct branch distance in meters. Default: 3.0.
  --direct-speed M    Direct branch speed in m/s. Default: 0.8.
  --disable-command-topic
                       Start runner without subscribing command topic.
  --with-aux-inputs   Publish localization/chassis/obstacle samples before SelectedSlot.
  --aux-mode MODE     Aux publisher mode. Default: all-valid.
                       all-valid|invalid-localization|nan-localization|
                       chassis-only|invalid-obstacles|bad-obstacle-geometry|
                       moving-localization|moving-localization-large|
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

effective_count="${count}"
effective_command_count="${command_count}"
effective_command_interval_ms="${command_interval_ms}"
effective_aux_count="${aux_count}"
effective_aux_interval_ms="${aux_interval_ms}"
if [[ "${command_mode}" != "none" ]]; then
  if ((effective_command_count < 8)); then
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
    if grep -Eq "${pattern}" "${runner_log}"; then
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

env "${runner_env[@]}" "${runner}" "${runner_args[@]}" >"${runner_log}" 2>&1 &
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
  sleep 1
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

if [[ "${command_mode}" != "none" ]]; then
  "${command_publisher}" --domain-id="${domain_id}" --mode="${command_mode}" \
    --count="${effective_command_count}" \
    --interval-ms="${effective_command_interval_ms}" \
    --direct-distance="${direct_distance}" \
    --direct-speed="${direct_speed}" \
    --reason="smoke-${command_mode}" >"${command_publisher_log}" 2>&1 &
  command_publisher_pid=$!

  command_wait_pattern="command #[0-9]+ mode="
  case "${command_mode}" in
    direct-forward)
      command_wait_pattern="command #[0-9]+ mode=DIRECT_FORWARD"
      ;;
    direct-backward)
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
    invalid)
      command_wait_pattern="command #[0-9]+ mode=NONE \\(cleared_command\\)"
      ;;
  esac
  wait_for_runner_log "${command_wait_pattern}" 8 || true
fi

"${publisher}" --domain-id="${domain_id}" --mode="${slot_mode}" --count="${effective_count}" \
  --interval-ms="${interval_ms}" >"${publisher_log}" 2>&1

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
grep -E "command #|command rejected|STAGE_CONTROL|OPEN_SPACE_STRAIGHT_PATH|SPEED_OPTIMIZER|PATH_PARTITION|PATH_PROVIDER|bridged sample|aux localization|aux chassis|aux obstacles" "${runner_log}" | tail -n 100 || true
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

require_command_log() {
  local pattern="$1"
  local message="$2"
  if ! grep -Eq "${pattern}" "${command_publisher_log}"; then
    echo "[valet_parking_smoke] ${message}" >&2
    validation_status=8
  fi
}

reject_runner_log() {
  local pattern="$1"
  local message="$2"
  if grep -Eq "${pattern}" "${runner_log}"; then
    echo "[valet_parking_smoke] ${message}" >&2
    validation_status=8
  fi
}

case "${slot_mode}" in
  valid)
    if [[ "${command_mode}" == "none" ]]; then
      if [[ "${expect_path_provider_timeout}" == "1" ]]; then
        require_runner_log "PATH_PROVIDER failed: OpenSpacePathProvider target plan timeout.*provider_status=TARGET_TIMEOUT.*target_timeout=true.*target_cancel=true" \
          "missing expected threaded OpenSpacePathProvider timeout evidence"
        require_runner_log "fallback to ROI seed" \
          "missing controlled ROI-seed fallback after PATH_PROVIDER timeout"
        require_subscriber_log "is_estop=false" \
          "missing subscriber output after PATH_PROVIDER timeout fallback"
      else
        require_runner_log "PATH_PROVIDER ok.*threaded=true.*provider_status=TARGET_READY.*target_source=target_thread" \
          "missing threaded OpenSpacePathProvider target plan evidence"
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
    require_runner_log "PATH_PROVIDER ok.*parking_seq=1.*preplan_candidates=1.*thread_path_ids=\\[[^]]*2" \
      "missing threaded preplan candidate for non-selected parking_seq=2"
    require_runner_log "PATH_PROVIDER ok.*parking_seq=2.*history=generated, replan=TARGET_UPDATE.*reason=target_update" \
      "missing generated path after opt_parking_seq selects parking_seq=2"
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

if [[ "${command_mode}" != "none" ]]; then
  case "${command_mode}" in
    parking-in)
      require_command_log "enum=PARKING_IN" \
        "missing PARKING_IN command publisher evidence"
      require_runner_log "command #[0-9]+ mode=PARKING_IN" \
        "missing PARKING_IN command consumption in runner log"
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
      require_runner_log "OPEN_SPACE_STRAIGHT_PATH" \
        "missing DIRECT_FORWARD OPEN_SPACE_STRAIGHT_PATH execution evidence"
      require_runner_log "SPEED_OPTIMIZER ok" \
        "missing DIRECT_FORWARD speed optimizer handoff evidence"
      require_runner_log "direct_speed_bound_max=${direct_speed}" \
        "missing DIRECT_FORWARD direct speed bound mapping evidence"
      reject_runner_log "ROI_DECIDER ok" \
        "DIRECT_FORWARD should not run ROI_DECIDER in the OPEN_SPACE_STRAIGHT_PATH branch"
      if [[ "${direct_velocity_conflict_expected}" == "1" ]]; then
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
    direct-backward)
      require_command_log "enum=DIRECT_BACKWARD" \
        "missing DIRECT_BACKWARD command publisher evidence"
      require_runner_log "command #[0-9]+ mode=DIRECT_BACKWARD" \
        "missing DIRECT_BACKWARD command consumption in runner log"
      require_runner_log "STAGE_CONTROL DIRECT_BACKWARD.*skip=ROI_PATH_PROVIDER_PATH_PARTITION" \
        "missing DIRECT_BACKWARD stage-control trajectory reason"
      require_runner_log "OPEN_SPACE_STRAIGHT_PATH" \
        "missing DIRECT_BACKWARD OPEN_SPACE_STRAIGHT_PATH execution evidence"
      require_runner_log "SPEED_OPTIMIZER ok" \
        "missing DIRECT_BACKWARD speed optimizer handoff evidence"
      require_runner_log "direct_speed_bound_max=${direct_speed}" \
        "missing DIRECT_BACKWARD direct speed bound mapping evidence"
      reject_runner_log "ROI_DECIDER ok" \
        "DIRECT_BACKWARD should not run ROI_DECIDER in the OPEN_SPACE_STRAIGHT_PATH branch"
      if [[ "${direct_velocity_conflict_expected}" == "1" ]]; then
        require_runner_log "OPEN_SPACE_STRAIGHT_PATH stop_path.*velocity_direction_conflict" \
          "missing DIRECT_BACKWARD velocity direction conflict stop_path evidence"
        require_subscriber_log "length=0" \
          "DIRECT_BACKWARD velocity conflict should publish a zero-length stop path"
      fi
      require_subscriber_log "gear=2" \
        "missing reverse gear in DIRECT_BACKWARD subscriber output"
      require_subscriber_log "is_estop=false" \
        "missing non-estop trajectory for DIRECT_BACKWARD command mode"
      require_subscriber_log "reason: replan=.*DIRECT_BACKWARD.*OPEN_SPACE_STRAIGHT_PATH" \
        "missing DIRECT_BACKWARD OPEN_SPACE_STRAIGHT_PATH reason in subscriber output"
      ;;
    pause)
      require_command_log "enum=PAUSE" \
        "missing PAUSE command publisher evidence"
      require_runner_log "command #[0-9]+ mode=PAUSE" \
        "missing PAUSE command consumption in runner log"
      require_runner_log "STAGE_CONTROL PAUSE.*skip=ROI_PATH_PROVIDER_PATH_PARTITION" \
        "missing PAUSE stage-control reason"
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
      require_runner_log "STAGE_CONTROL FINISH.*MISSIONFINISHED.*skip=ROI_PATH_PROVIDER_PATH_PARTITION" \
        "missing FINISH/MISSIONFINISHED stage-control reason"
      reject_runner_log "ROI_DECIDER ok" \
        "FINISH should not run ROI_DECIDER"
      require_subscriber_log "points=1" \
        "missing one-point stop trajectory for FINISH command mode"
      require_subscriber_log "is_estop=false" \
        "missing non-estop stop trajectory for FINISH command mode"
      require_subscriber_log "reason: replan=.*MISSIONFINISHED" \
        "missing MISSIONFINISHED reason in subscriber output"
      ;;
    invalid)
      require_command_log "is_valid=false" \
        "missing invalid command publisher evidence"
      require_runner_log "command #[0-9]+ mode=NONE \\(cleared_command\\)" \
        "missing invalid command clearing in runner log"
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

if [[ "${expect_thread_provider_stop}" == "1" ]]; then
  require_runner_log "OpenSpaceThreadManager stopped, search_threads=[1-9][0-9]*, target_thread_joined=true" \
    "missing OpenSpaceThreadManager stop/destructor evidence"
fi

exit "${validation_status}"
