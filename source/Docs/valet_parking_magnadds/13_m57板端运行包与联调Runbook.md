# m57 板端运行包与联调 Runbook

日期：2026-07-29

## 目的

本文记录 `NEXT-048` 的结果：把 m57 交叉编译产物整理成可拷贝到板端的运行包，并写清板端联调步骤。

注意：当前没有 m57 板端实测结果。本文只能说明“包已准备好、ELF 是 aarch64、依赖已随包”，不能把状态写成板端运行通过。

## 一键打包

在 WSL 中执行：

```bash
cd /mnt/e/APA/DDS/feature_integration/feature_integration_workspace
bash applications/source/valet_parking_tools/package_valet_parking_m57.sh
```

默认输入：

```text
out/valet_parking_batch_042_046/valet_parking_mvp/m57
```

默认输出：

```text
out/valet_parking_m57_package/valet_parking_m57_runtime_<timestamp>
out/valet_parking_m57_package/valet_parking_m57_runtime_<timestamp>.tar.gz
```

## 本次验证记录

本次已在 WSL 中完成脚本语法检查和实际打包：

```text
bash -n applications/source/valet_parking_tools/package_valet_parking_m57.sh
bash applications/source/valet_parking_tools/package_valet_parking_m57.sh
```

生成结果：

```text
out/valet_parking_m57_package/valet_parking_m57_runtime_20260729_081921
out/valet_parking_m57_package/valet_parking_m57_runtime_20260729_081921.tar.gz
```

`PACKAGE_MANIFEST.txt` 已记录：

```text
[file]
valet_parking_runner: ELF 64-bit LSB executable, ARM aarch64
libvalet_parking.so: ELF 64-bit LSB shared object, ARM aarch64
libmagna-dds-core.so.1.0.0: ELF 64-bit LSB shared object, ARM aarch64
libmagna-dds-impl.so: ELF 64-bit LSB shared object, ARM aarch64

[runner_dynamic]
NEEDED libvalet_parking.so
NEEDED libmagna-dds-core.so.1
NEEDED libmagna-dds-impl.so

[libvalet_parking_dynamic]
NEEDED libmagna-dds-core.so.1
NEEDED libmagna-dds-impl.so
RPATH $ORIGIN:$ORIGIN/../lib:...
```

这只能作为“运行包内容和 aarch64 依赖证据已准备”的验收，不能替代 m57 板端启动和真实 DDS discovery 验收。

## 包内容

默认包内包含：

```text
app/valet_parking_runner
app/selected_slot_mock_publisher
app/planning_trajectory_mock_subscriber
app/parking_command_mock_publisher
app/aux_input_mock_publisher
lib/libvalet_parking.so
lib/libmagna-dds-core.so
lib/libmagna-dds-core.so.1
lib/libmagna-dds-core.so.1.0.0
lib/libmagna-dds-impl.so
run_valet_parking_runner.sh
README_RUNTIME.md
PACKAGE_MANIFEST.txt
```

`run_valet_parking_runner.sh` 会自动设置：

```bash
LD_LIBRARY_PATH=<package>/lib:$LD_LIBRARY_PATH
```

因此板端运行时不需要依赖构建机上的绝对 RPATH。当前 `libvalet_parking.so` 的 RPATH 里仍可看到构建机路径，这是构建脚本 install 时写入的；上板时以 `LD_LIBRARY_PATH` 和随包 `lib/` 为准。

## 板端启动步骤

1. 拷贝包到板端：

```bash
scp out/valet_parking_m57_package/valet_parking_m57_runtime_<timestamp>.tar.gz root@<m57-ip>:/userdata/
```

2. 在板端解压：

```bash
cd /userdata
tar -xzf valet_parking_m57_runtime_<timestamp>.tar.gz
cd valet_parking_m57_runtime_<timestamp>
chmod +x run_valet_parking_runner.sh app/*
```

3. 启动 runner：

```bash
./run_valet_parking_runner.sh --domain-id=0
```

4. 如果真实车端 Topic 名不同，用参数覆盖：

```bash
./run_valet_parking_runner.sh \
  --domain-id=<domain> \
  --in-topic=<real-selected-slot-topic> \
  --out-topic=<real-planning-trajectory-topic> \
  --command-topic=<real-parking-command-topic> \
  --localization-topic=<real-localization-topic> \
  --chassis-topic=<real-chassis-topic> \
  --obstacle-topic=<real-obstacle-topic>
```

5. 如果暂时没有 command 或辅助输入 Topic，可禁用：

```bash
./run_valet_parking_runner.sh --domain-id=<domain> --disable-command-topic --disable-aux-input-topics
```

## 板端自检顺序

先查文件和架构：

```bash
file app/valet_parking_runner lib/libvalet_parking.so
readelf -d lib/libvalet_parking.so | grep NEEDED
```

再查动态库是否能找到：

```bash
export LD_LIBRARY_PATH=$PWD/lib:$LD_LIBRARY_PATH
ldd app/valet_parking_runner
ldd lib/libvalet_parking.so
```

再启动 DDS 进程：

```bash
./run_valet_parking_runner.sh --domain-id=<domain>
```

最后检查真实通信：

```text
Domain 一致
Topic name 一致
Type name / IDL 一致
QoS 兼容
subscriber 已启动并完成 discovery
write/take_next_sample 返回 OK
valid_data 为 true
```

这套顺序来自 MagnaDDS 培训材料的最小链路检查清单。

## 常见问题

| 现象 | 优先排查 |
|---|---|
| `No such file or directory` 但文件存在 | aarch64 loader `/lib/ld-linux-aarch64.so.1` 是否存在 |
| `libmagna-dds-core.so.1` not found | 是否在包目录执行，`LD_LIBRARY_PATH` 是否包含 `$PWD/lib` |
| 创建 DomainParticipant 失败 | Domain id、DDS backend 运行环境、板端网络/权限 |
| runner 启动但收不到车位 | Domain、Topic、Type、QoS 是否匹配 |
| 收到 Topic 但算法 estop | 查看 `replan_reason/estop.reason`，确认车位角点、定位、障碍物是否在局部边界内 |
| 出库命令没有轨迹 | 当前 MVP 对 `PARKING_OUT_*` 明确返回 unsupported stop，尚未接出库算法 |

## 验收边界

可以标记：

```text
m57 package prepared
m57 ELF aarch64
m57 required MagnaDDS libs packaged
runbook ready
```

不能标记：

```text
m57 board runtime PASS
real vehicle Topic contract PASS
parking-out algorithm PASS
formal IDL contract frozen
```
