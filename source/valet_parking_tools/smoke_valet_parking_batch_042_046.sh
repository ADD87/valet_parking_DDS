#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Usage:
  bash applications/source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh [options]

Options:
  --run-root PATH          Built x86 runtime root. Default:
                           <workspace>/out/valet_parking_batch_042_046/valet_parking_mvp/x86
  --first-domain-id N      First DDS domain id. Default: 170.
  --timeout-ms N           Subscriber timeout for each smoke. Default: 25000.
  --help                   Show this help.

This batch runs the BATCH-042_046 command/status hardening smoke matrix:
  valid, timeout, multi-lot, direct, reset_history, pause/brake/finish,
  invalid clear, parking-out unsupported, and core target/obstacle regressions.
  BATCH-065_068 also reuses this matrix for finish-boundary and combined-state
  regressions.
EOF
}

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
app_source_root="$(cd "${script_dir}/.." && pwd -P)"
applications_root="$(cd "${app_source_root}/.." && pwd -P)"
workspace_root="$(cd "${applications_root}/.." && pwd -P)"

run_root="${workspace_root}/out/valet_parking_batch_042_046/valet_parking_mvp/x86"
first_domain_id=170
timeout_ms=25000

while [[ $# -gt 0 ]]; do
  case "$1" in
    --run-root)
      run_root="${2:-}"
      shift 2
      ;;
    --first-domain-id)
      first_domain_id="${2:-}"
      shift 2
      ;;
    --timeout-ms)
      timeout_ms="${2:-}"
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

if ! [[ "${first_domain_id}" =~ ^[0-9]+$ ]]; then
  echo "Invalid --first-domain-id: ${first_domain_id}" >&2
  exit 2
fi
if ! [[ "${timeout_ms}" =~ ^[0-9]+$ ]] || [[ "${timeout_ms}" -le 0 ]]; then
  echo "Invalid --timeout-ms: ${timeout_ms}" >&2
  exit 2
fi

smoke_script="${script_dir}/smoke_valet_parking_x86.sh"
if [[ ! -x "${smoke_script}" && ! -f "${smoke_script}" ]]; then
  echo "Smoke script is missing: ${smoke_script}" >&2
  exit 1
fi

domain_id="${first_domain_id}"
failed_cases=()

run_case() {
  local label="$1"
  shift
  echo
  echo "[batch_042_046] ==== ${label} domain=${domain_id} ===="
  if ! bash "${smoke_script}" \
      --run-root "${run_root}" \
      --domain-id "${domain_id}" \
      --timeout-ms "${timeout_ms}" \
      "$@"; then
    failed_cases+=("${label}@${domain_id}")
  fi
  domain_id=$((domain_id + 1))
}

run_case "valid-threaded-provider" \
  --expect-thread-provider-stop

run_case "timeout-cancel-fallback" \
  --expect-path-provider-timeout \
  --expect-thread-provider-stop

run_case "multi-lot-preplan-candidate" \
  --slot-mode multi-lot-seq-switch \
  --expect-thread-provider-stop

run_case "destination-reached-finish-boundary" \
  --with-aux-inputs \
  --aux-mode near-destination \
  --aux-chassis-gear drive \
  --expect-thread-provider-stop

run_case "direct-forward-stop-contract" \
  --command-mode direct-forward

run_case "direct-backward-stop-contract" \
  --command-mode direct-backward

run_case "direct-forward-moving-contract" \
  --command-mode direct-forward \
  --with-aux-inputs \
  --aux-mode all-valid \
  --aux-chassis-gear drive \
  --direct-speed 1.2

run_case "direct-backward-moving-contract" \
  --command-mode direct-backward \
  --with-aux-inputs \
  --aux-mode all-valid \
  --aux-chassis-gear reverse \
  --direct-speed 1.2

run_case "direct-forward-release-finish-contract" \
  --command-mode direct-forward-release

run_case "direct-backward-release-finish-contract" \
  --command-mode direct-backward-release

run_case "direct-release-restore-target-update" \
  --slot-mode target-moves \
  --command-mode direct-forward-release

run_case "direct-reset-history-one-shot" \
  --command-mode direct-forward \
  --command-reset-history

run_case "parking-in-reset-target-update" \
  --slot-mode target-moves \
  --command-mode parking-in \
  --command-reset-history

run_case "pause-stop-contract" \
  --command-mode pause

run_case "brake-stop-contract" \
  --command-mode brake

run_case "finish-stop-contract" \
  --command-mode finish

run_case "invalid-command-clear" \
  --command-mode invalid

run_case "parking-out-left-unsupported" \
  --command-mode parking-out-left

run_case "target-update-regression" \
  --slot-mode target-moves

run_case "path-id-update-regression" \
  --slot-mode parking-seq-changes

run_case "far-obstacles-regression" \
  --with-aux-inputs \
  --aux-mode far-obstacles

if (( ${#failed_cases[@]} > 0 )); then
  echo
  echo "[batch_042_046] failed cases:"
  printf '  %s\n' "${failed_cases[@]}"
  exit 8
fi

echo
echo "[batch_042_046] all smoke cases passed."
