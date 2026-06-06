# 任务进度

## 当前任务

- 按用户要求，把灯效模式一的灯珠颜色全部改成红色。

## 已确认假设

- 模式一对应 `LED_MODE_CHANGE1`，入口在 `CheckRgbLedEffect()` 中调用 `LedEffect1()`。
- “灯珠的颜色全部改成红色”按最小范围理解为：模式一中被音量点亮的 LED_DATA_3（音乐音量）和 LED_DATA_2（麦克风音量）灯珠都显示红色；音量数量、亮度比例、定时和音乐加速逻辑保持不变。

## 计划

- 定位 `LedEffect1()` 的颜色写入逻辑。
- 仅替换模式一内 LED_DATA_3/LED_DATA_2 的绿/黄/红分段颜色逻辑。
- 做静态范围检查，确认模式一不再写绿色/黄色分段。

## 已完成

- 已定位 `app_src/new/pwm_led.c` 中 `LedEffect1()`。
- 已将 `LedEffect1()` 中 LED_DATA_3 循环改为统一 `r_val = 255 * userVar.brightness / 100`，`g_val/b_val` 保持 0。
- 已将 `LedEffect1()` 中 LED_DATA_2 循环改为统一 `r_val = 255 * userVar.brightness / 100`，`g_val/b_val` 保持 0。
- 未改动模式一的音量采样、点亮数量、定时和音乐加速逻辑。

## 下一步

- 在原厂/项目完整工具链环境下编译固件。
- 实机切换到第 1 个灯效，确认音乐音量与麦克风音量灯珠均为红色。

## 验证

- 静态回读确认：`LedEffect1()` 中 LED_DATA_3 和 LED_DATA_2 两个循环均只给 `r_val` 赋亮度值。
- 静态范围检查确认：`LedEffect1()` 范围内没有 `g_val = 255`、`b_val = 255`、`if(i <= 34)`、`else if(i <= 44)` 分色逻辑。
- 静态确认：`LED_MODE_CHANGE1` 入口仍调用 `LedEffect1()`。
- 用户反馈：项目当前已经能跑，可作为 Git 存档状态。
- 当前目录不是 Git 仓库，无法使用 `git diff/status` 查看版本差异。
- 当前 PATH 未发现 `make`、`nds32le-elf-gcc`，未能完整编译固件。
