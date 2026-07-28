# 项目状态快照 037：轻量Stage控制输入Direct/Pause/Finish闭环

- 快照编号：037
- 日期：2026-07-28
- 当前阶段：Fast-2 - `.so` + typed DDS + light Stage command controls
- 阶段状态：PASS_STAGE_COMMAND_CONTROL_X86_M57_STATIC_BLOCKED_BOARD
- 创建原因：用户要求先完成 NEXT-034 到 NEXT-037，最后再集中调试排查问题。本快照记录 `ParkingCommand` IDL、DDS reader、Adapter 轻量分支、mock publisher、smoke 脚本和 x86/m57 验证结果。

---

## 1. 本次完成事项

- [x] NEXT-034：新增轻量 Stage 控制输入契约。
  - `ParkingCommandMode` 支持 `PARKING_IN`、`PARKING_OUT_*`、`DIRECT_FORWARD`、`DIRECT_BACKWARD`、`PAUSE`、`BRAKE`、`FINISH`。
  - `ParkingCommand` 支持 `is_valid`、`mode`、`parking_seq`、`direct_distance_m`、`direct_speed_mps`、`reset_history`、`reason`。
  - 已用 MagnaDDS SDK `idlparser` 重新生成 `generated/*`。
- [x] NEXT-035：接入 `DIRECT_FORWARD/DIRECT_BACKWARD` 轻量直行分支。
  - Adapter 在 ROI_DECIDER 前识别 direct command。
  - 输出 21 点短轨迹。
  - `DIRECT_FORWARD` 输出 drive 档，`DIRECT_BACKWARD` 输出 reverse 档。
  - 原因包含 `STAGE_CONTROL DIRECT_*` 和 `skip=ROI_PATH_PROVIDER_PATH_PARTITION`。
- [x] NEXT-036：接入 `PAUSE/BRAKE/FINISH` 控制分支。
  - 输出非 estop 的单点停止轨迹。
  - `FINISH` 原因包含 `MISSIONFINISHED`。
  - `PARKING_OUT_*` 当前只输出安全停止并标记 `unsupported_in_mvp`。
- [x] NEXT-037：新增 command mock publisher 和 smoke 验证入口。
  - 新增 `source/valet_parking_tools/parking_command_mock_publisher`。
  - `valet_parking_runner` 新增 `--command-topic`、`--disable-command-topic`。
  - `smoke_valet_parking_x86.sh` 新增 `--command-mode`、`--direct-distance`、`--direct-speed`。
  - `planning_trajectory_mock_subscriber` 输出 `gear`、`trajectory_type`、`replan_reason`、`estop.reason`，便于断言。

---

## 2. 本次代码边界

本阶段没有接完整 `FunctionManager`，没有接完整 `OpenSpaceStraightPathProvider`，也没有接真实车端 command Topic 协议。

当前 direct 分支是轻量替代：

```text
ParkingCommand(DIRECT_FORWARD/BACKWARD)
-> Component 缓存最新 command
-> SelectedSlot 触发 Adapter
-> Adapter 直接 BuildDirectTrajectory()
-> PlanningTrajectory
```

它故意跳过：

```text
ROI_DECIDER
PATH_PROVIDER
PATH_PARTITION
SPEED_OPTIMIZER
```

这是对原始 Stage direct 模式“跳过 ROI/PATH_PROVIDER/PATH_PARTITION”的可观测语义复现，不等于完整原车 `OPEN_SPACE_STRAIGHT_PATH`。

---

## 3. 本次验证命令

x86 构建：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform x86 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_stage_command_037
```

x86 smoke：

```bash
# default valid
--domain-id 229

# all-valid aux input regression
--domain-id 222 --with-aux-inputs --aux-mode all-valid

# command controls
--domain-id 223 --command-mode direct-forward
--domain-id 224 --command-mode direct-backward
--domain-id 225 --command-mode pause
--domain-id 226 --command-mode brake
--domain-id 227 --command-mode finish
--domain-id 228 --command-mode invalid
```

m57 交叉编译：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/build_valet_parking.sh \
  --platform m57 \
  --jobs 8 \
  --out-dir /mnt/e/APA/DDS/feature_integration/feature_integration_workspace/out/valet_parking_stage_command_037
```

静态检查：

```bash
git diff --check
bash -n applications/source/valet_parking_tools/smoke_valet_parking_x86.sh
python3 -c "yaml.safe_load(...)"
```

---

## 4. 验收结果

| 验收项 | 结果 | 证据 |
|---|---|---|
| IDL 重新生成 | PASS | `generated/*` 已包含 `ParkingCommand` 和 `ParkingCommandTopicDataType` |
| x86 构建 | PASS | `out/valet_parking_stage_command_037/valet_parking_mvp/x86/lib/libvalet_parking.so` |
| x86 valid 回归 | PASS | `domain_229_20260728_190647_1500` |
| x86 all-valid 辅助输入回归 | PASS | `domain_222_20260728_190610_1179` |
| x86 direct-forward | PASS | `domain_223_20260728_190436_767`，21 点短轨迹，`gear=1` |
| x86 direct-backward | PASS | `domain_224_20260728_190450_909`，21 点短轨迹，`gear=2` |
| x86 pause | PASS | `domain_225_20260728_190524_743`，1 点停止轨迹，`is_estop=false` |
| x86 brake | PASS | `domain_226_20260728_190537_870`，1 点停止轨迹，`is_estop=false` |
| x86 finish | PASS | `domain_227_20260728_190551_1012`，1 点停止轨迹，原因包含 `MISSIONFINISHED` |
| x86 invalid command | PASS | `domain_228_20260728_190629_1338`，清 command 后回到普通泊车链路 |
| smoke 脚本 command discovery | PASS | command publisher 后台持续发布，runner 收到 command 后再发 `SelectedSlot` |
| m57 `libvalet_parking.so` 生成 | PASS | `out/valet_parking_stage_command_037/valet_parking_mvp/m57/lib/libvalet_parking.so` |
| m57 ELF 架构 | PASS | `ELF 64-bit LSB shared object, ARM aarch64` |
| m57 MagnaDDS 依赖 | PASS | `libmagna-dds-core.so.1`、`libmagna-dds-impl.so` |
| m57 板端运行 | NOT_RUN | 当前无 m57 板端，不能标记为板端通过 |

---

## 5. 调试记录

- 并行跑多个 DDS smoke 会造成 `DomainParticipant` 创建失败；后续本机 smoke 应串行执行。
- 当前环境中 `domain-id >= 231` 会出现 `failed to create DomainParticipant`；本次证据使用 222-230 范围内 domain。
- command publisher 如果只发 1 条并立即退出，runner 可能因 DDS discovery 尚未匹配而收不到 command；脚本已改为 command publisher 后台持续发布至少 8 条，并等待 runner 收到 command 后再发 `SelectedSlot`。

---

## 6. 本阶段修改文件

- `applications/source/Docs/valet_parking_magnadds/07_轻量Stage控制输入与034到037接入说明.md`
- `applications/source/Docs/valet_parking_magnadds/README.md`
- `applications/source/Docs/valet_parking_magnadds/STATUS.yaml`
- `applications/source/Docs/valet_parking_magnadds/status_snapshots/037_项目状态快照_轻量Stage控制输入DirectPauseFinish闭环.md`
- `applications/config/valet_parking_mvp_bom.yaml`
- `applications/source/valet_parking/idl/valet_parking_topics.idl`
- `applications/source/valet_parking/generated/valet_parking_topics.h`
- `applications/source/valet_parking/generated/valet_parking_topics.cpp`
- `applications/source/valet_parking/generated/valet_parking_topicsTopicDataType.h`
- `applications/source/valet_parking/generated/valet_parking_topicsTopicDataType.cpp`
- `applications/source/valet_parking/include/valet_parking_c_api.h`
- `applications/source/valet_parking/src/valet_parking_c_api.cpp`
- `applications/source/valet_parking/src/valet_parking_component.h`
- `applications/source/valet_parking/src/valet_parking_component.cpp`
- `applications/source/valet_parking/src/valet_parking_stage_parking_adapter.h`
- `applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp`
- `applications/source/valet_parking/README.md`
- `applications/source/valet_parking_tools/valet_parking_runner/main.cpp`
- `applications/source/valet_parking_tools/parking_command_mock_publisher/CMakeLists.txt`
- `applications/source/valet_parking_tools/parking_command_mock_publisher/main.cpp`
- `applications/source/valet_parking_tools/planning_trajectory_mock_subscriber/main.cpp`
- `applications/source/valet_parking_tools/smoke_valet_parking_x86.sh`

---

## 7. 下一步

进入 NEXT-038：评估是否把当前轻量 direct branch 升级为完整 `OpenSpaceStraightPathProvider`/`OPEN_SPACE_STRAIGHT_PATH` 本地化接入；同时继续保持当前 `ParkingCommand` 临时 Topic 与真实车端 command 协议未对齐的边界说明。

建议先做差距评估和输入输出契约表，不要直接大搬完整 Stage/Frame/DependencyInjector。
