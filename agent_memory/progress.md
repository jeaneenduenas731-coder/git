# 任务进度

## 当前任务

- 按用户要求，把灯效模式一的灯珠颜色全部改成白色。

## 已确认假设

- 模式一对应 `LED_MODE_CHANGE1`，入口在 `CheckRgbLedEffect()` 中调用 `LedEffect1()`。
- “灯效模式一的灯珠全部改成白色”按最小范围理解为：模式一中被音量点亮的 LED_DATA_3（音乐音量）和 LED_DATA_2（麦克风音量）灯珠都显示白色；音量数量、亮度比例、定时和音乐加速逻辑保持不变。

## 计划

- 定位 `LedEffect1()` 的颜色写入逻辑。
- 仅替换模式一内 LED_DATA_3/LED_DATA_2 的固定蓝色赋值为固定白色赋值。
- 做静态范围检查，确认模式一固定亮灯同时写 R/G/B 三个亮度通道。

## 已完成

- 已定位 `app_src/new/pwm_led.c` 中 `LedEffect1()`。
- 已将 `LedEffect1()` 中 LED_DATA_3 循环改为统一 `r_val/g_val/b_val = 255 * userVar.brightness / 100`。
- 已将 `LedEffect1()` 中 LED_DATA_2 循环改为统一 `r_val/g_val/b_val = 255 * userVar.brightness / 100`。
- 未改动模式一的音量采样、点亮数量、定时和音乐加速逻辑。

## 下一步

- 在原厂/项目完整工具链环境下编译固件。
- 实机切换到第 1 个灯效，确认音乐音量与麦克风音量灯珠均为白色。

## 验证

- 静态回读确认：`LedEffect1()` 中 LED_DATA_3 和 LED_DATA_2 两个循环均给 `r_val/g_val/b_val` 赋同一亮度值。
- 静态范围检查确认：`LedEffect1()` 范围内 `r_val/g_val/b_val = 255 * userVar.brightness / 100` 各为 2 处。
- 静态确认：`LED_MODE_CHANGE1` 入口仍调用 `LedEffect1()`。
- `git diff -- app_src/new/pwm_led.c` 确认只新增四行颜色通道赋值，从蓝色变白色。
- 当前 PATH 未发现 `make`、`nds32le-elf-gcc`，未能完整编译固件。
