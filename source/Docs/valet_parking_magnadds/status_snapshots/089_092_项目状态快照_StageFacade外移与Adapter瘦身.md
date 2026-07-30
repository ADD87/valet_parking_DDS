# 项目状态快照：BATCH-089_092 StageFacade 外移与 Adapter 瘦身

- 快照编号：089_092
- 日期：2026-07-30
- 当前阶段：Fast-2
- 阶段状态：PASSED
- 创建原因：继续执行“Adapter 瘦身，原车流程骨架和血管先接上”的计划。

---

## 1. 本次完成事项

- [x] 批量扫描 Adapter 中的 Stage skeleton helper。
- [x] 新增 `valet_parking_stage_facade_lite.h/.cpp`。
- [x] 将 Stage skeleton 输入构造、lite 上下文更新、direct release/finish hold 标记从 Adapter 外移。
- [x] 将 `CountPathProviderPoints()` 从 Adapter 外移到 Stage facade，作为 OpenSpaceInfo lite 写回支撑。
- [x] `CMakeLists.txt` 纳入新的 `valet_parking_stage_facade_lite.cpp`。
- [x] x86/m57 构建通过，x86 normal/direct/batch smoke 通过。

---

## 2. 本批架构结论

Adapter 长期不应该有几千行。

本批不是一次性重写 Adapter，而是做第一刀可验证的外移：

```text
ValetParkingStageParkingAdapter
  -> 保留 DDS/runtime 私有类型
  -> 保留算法调用顺序
  -> 只把当前状态映射为 StageFacadeInputLite

ValetParkingStageFacadeLite
  -> 负责 Stage skeleton 输入构造
  -> 负责 Frame/OpenSpaceInfo/PlanningContext lite 更新
  -> 负责 direct release / finish hold 的 StageContext 标记

ValetParkingStageProcessLite
  -> 负责 Process/SetParkingType/ExecuteTaskOnOpenSpace/IsReadyToFinishStage/FinishScenario 方法序列投影
```

Adapter 行数从约 `5217` 降到 `5073`。这不是终点，但已经把“继续堆诊断字符串”的方向扭回到“Stage 层承接业务”。

---

## 3. 当前交付物

| 文件/产物 | 状态 | 说明 |
|---|---|---|
| `applications/source/valet_parking/src/valet_parking_stage_facade_lite.h` | 新增 | Stage facade 输入、分支枚举、上下文更新接口 |
| `applications/source/valet_parking/src/valet_parking_stage_facade_lite.cpp` | 新增 | Stage skeleton 输入构造、MarkStage* 更新函数、CountPathProviderPoints |
| `applications/source/valet_parking/src/valet_parking_stage_parking_adapter.cpp` | 修改 | 删除 Stage helper 主体实现，保留薄桥接 |
| `applications/source/valet_parking/CMakeLists.txt` | 修改 | 编译新增 facade 源文件 |
| `applications/source/Docs/valet_parking_magnadds/STATUS.yaml` | 修改 | 更新当前状态、验证矩阵和下一步 |
| `applications/source/Docs/valet_parking_magnadds/05_TempAPA原始流程复现差距对照表.md` | 修改 | 追加本批差异收敛 |
| `applications/source/Docs/valet_parking_magnadds/14_原始流程差异收敛执行记录.md` | 修改 | 追加本批执行记录 |

---

## 4. 验证

通过：

```text
git diff --check
bash -n source/valet_parking_tools/build_valet_parking.sh
bash -n source/valet_parking_tools/smoke_valet_parking_x86.sh
bash -n source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh
bash source/valet_parking_tools/build_valet_parking.sh --out-dir /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_089_092
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_089_092/valet_parking_mvp/x86 --domain-id 187 --timeout-ms 25000
bash source/valet_parking_tools/smoke_valet_parking_x86.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_089_092/valet_parking_mvp/x86 --domain-id 188 --command-mode direct-forward --timeout-ms 25000
bash source/valet_parking_tools/smoke_valet_parking_batch_042_046.sh --run-root /mnt/e/APA/DDS/feature_integration/out/valet_parking_flow_gap_089_092/valet_parking_mvp/x86 --first-domain-id 189 --timeout-ms 25000
```

产物：

```text
out/valet_parking_flow_gap_089_092/valet_parking_mvp/x86/lib/libvalet_parking.so
out/valet_parking_flow_gap_089_092/valet_parking_mvp/m57/lib/libvalet_parking.so
```

smoke 证据：

```text
normal open-space: domain_187_20260730_073421_727
direct-forward: domain_188_20260730_073438_904
batch first-domain-id=189: all smoke cases passed
batch last domain: domain_223_20260730_074008_5483
```

---

## 5. 仍保留的差异和风险

- Stage 输出契约构造仍主要在 Adapter。
- `StageFacadeInputLite` 仍是从 Adapter runtime 私有类型映射出来的过渡结构。
- formal typed DDS 字段仍未替代 `replan_reason/estop.reason` 文本契约。
- Adapter 仍有 5000+ 行，需要继续拆。
- m57 只完成交叉编译和 ELF/依赖检查，未做板端 runtime。

---

## 6. 下一步

进入 `BATCH-093_096`：

```text
NEXT-093：批量扫描 Stage 输出契约构造函数的依赖边界
NEXT-094：优先外移 AppendStageProjectionContract、BuildOpenSpaceStageOutputContract、BuildFallbackStageOutputContract、AppendStageControlContract 中可脱离 Adapter 私有类型的部分
NEXT-095：统一 x86/m57 build、normal/direct/batch smoke
NEXT-096：更新文档、提交并推送 applications Git
```

执行原则：继续让 Adapter 退回 DDS/算法调用壳，让 Stage facade/contract 文件承接业务流程和输出契约。
