################################################################################
# 自动生成的文件。不要编辑！
################################################################################

ifndef ANDESIGHT_ROOT
ANDESIGHT_ROOT=/cygdrive/C/Andestech/AndeSight_RDS_v323
endif

ifndef CROSS_COMPILE
CROSS_COMPILE=nds32le-elf-
endif

ifndef SECONDARY_OUTPUT_PATH
SECONDARY_OUTPUT_PATH=output
endif

$(shell mkdir -p $(SECONDARY_OUTPUT_PATH))

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include app_framework/audio_engine/subdir.mk
-include app_framework/bluetooth_engine/subdir.mk
-include app_framework/mode_engine/subdir.mk
-include app_framework/slow_device_engine/subdir.mk
-include app_framework/wireless_engine/subdir.mk
-include app_framework/wireless_engine/sbc/encoder/srce/subdir.mk
-include app_framework/wireless_engine/sbc/subdir.mk
-include app_src/app_mode__common/subdir.mk
-include app_src/app_mode_bt/subdir.mk
-include app_src/app_mode_dual_chip_idle/subdir.mk
-include app_src/app_mode_hdmi/subdir.mk
-include app_src/app_mode_i2s/subdir.mk
-include app_src/app_mode_idle/subdir.mk
-include app_src/app_mode_linein/subdir.mk
-include app_src/app_mode_media/subdir.mk
-include app_src/app_mode_radio/subdir.mk
-include app_src/app_mode_spdif/subdir.mk
-include app_src/app_mode_usb_audio/subdir.mk
-include app_src/app_mode_wirelessin/subdir.mk
-include app_src/components/algorithm/subdir.mk
-include app_src/components/audio/subdir.mk
-include app_src/components/audio/effect_control/subdir.mk
-include app_src/components/audio/music_parameter/bis/subdir.mk
-include app_src/components/audio/music_parameter/bypass/subdir.mk
-include app_src/components/audio/music_parameter/subdir.mk
-include app_src/components/audio/music_parameter/hfp/subdir.mk
-include app_src/components/audio/music_parameter/karaoke/subdir.mk
-include app_src/components/audio/music_parameter/mic/subdir.mk
-include app_src/components/audio/music_parameter/micusbAI/subdir.mk
-include app_src/components/audio/music_parameter/music/subdir.mk
-include app_src/components/audio/music_parameter/tdm/subdir.mk
-include app_src/components/audio/music_parameter/two_audio/subdir.mk
-include app_src/components/audio/music_parameter/uac/subdir.mk
-include app_src/components/ble/subdir.mk
-include app_src/components/bluetooth/subdir.mk
-include app_src/components/bt_source/subdir.mk
-include app_src/components/dump/subdir.mk
-include app_src/components/flash_manage/subdir.mk
-include app_src/components/otg/device/src/subdir.mk
-include app_src/components/otg/host/src/subdir.mk
-include app_src/components/otg/subdir.mk
-include app_src/components/rtc/subdir.mk
-include app_src/components/soft_watchdog/subdir.mk
-include app_src/components/upgrade/subdir.mk
-include app_src/hmi/can/subdir.mk
-include app_src/hmi/detect/subdir.mk
-include app_src/hmi/display/subdir.mk
-include app_src/hmi/fm/subdir.mk
-include app_src/hmi/key/subdir.mk
-include app_src/hmi/uart/subdir.mk
-include app_src/hmi/uartupgrade/subdir.mk
-include app_src/subdir.mk
-include app_src/new/subdir.mk
-include app_src/power/subdir.mk
-include app_src/startup/subdir.mk
-include driver/driver_api/src/subdir.mk
-include middleware/audio/src/subdir.mk
-include middleware/bluetooth/src/subdir.mk
-include middleware/cec/src/subdir.mk
-include middleware/fatfs/src/subdir.mk
-include middleware/flashboot/subdir.mk
-include middleware/mv_utils/src/subdir.mk
-include middleware/roboeffect/roboeffect_lib/subdir.mk
-include middleware/roboeffect/roboeffect_lib/wraps/subdir.mk
-include middleware/roboeffect/third_party_effect/ai_denoise/subdir.mk
-include middleware/roboeffect/third_party_effect/simple_gain/subdir.mk
-include middleware/rtos/freertos/src/subdir.mk
-include middleware/rtos/rtos_api/subdir.mk
-include middleware/spimflash/subdir.mk
-include test/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
BUILD_ARTIFACT_FILE_BASE_NAME = BT_Audio_APP

LINKER_OUTPUTS = BT_Audio_APP.adx

GPROF_OUT += \
../gmon.sum ../gmon.out \

PWR_OUT += \
$(SECONDARY_OUTPUT_PATH)/pwr \

GCOV_OUT += \
$(foreach subdir,$(SUBDIRS),$(wildcard $(subdir)/*.gcda) $(wildcard $(subdir)/*.gcno)) \

SYMBOL_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/symbol.txt \

READELF_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/readelf.txt \

OBJDUMP_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/objdump.txt \

OBJCOPY_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/$(BUILD_ARTIFACT_FILE_BASE_NAME).bin \

SIZE_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/.PHONY.size \


# 所有目标
all:
	$(MAKE) --no-print-directory pre-build
	$(MAKE) --no-print-directory main-build

# 主构建目标
main-build: BT_Audio_APP.adx secondary-outputs

# 工具调用
BT_Audio_APP.adx: $(OBJS) $(USER_OBJS)
	@echo '正在构建目标： $@'
	@echo '正在调用： Andes C Linker'
	$(CROSS_COMPILE)gcc -Os1 -nostartfiles -static -L"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/roboeffect/roboeffect_lib" -L"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_framework/wireless_engine" -L"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_framework/wireless_engine/sbc" -L"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/otg_host" -L"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/components/algorithm" -L"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/bluetooth" -L"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/cec" -L"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/driver/driver" -L"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/audio" -L"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/rtc" -L"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/lrc" -L"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/fatfs" -L"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/roboeffect/third_party_effect/ai_denoise" -mcmodel=medium -T"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/BP15x.ld" -Wl,--gc-sections -mvh -ldsp -mext-dsp -o "BT_Audio_APP.adx" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo '已结束构建目标： $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

$(foreach subdir,$(SUBDIRS),$(wildcard $(subdir)/*.gcda) $(wildcard $(subdir)/*.gcno)): BT_Audio_APP.adx $(OBJS) $(USER_OBJS)
../gmon.sum ../gmon.out: BT_Audio_APP.adx $(OBJS) $(USER_OBJS)
$(SECONDARY_OUTPUT_PATH)/pwr: BT_Audio_APP.adx $(OBJS) $(USER_OBJS)

$(SECONDARY_OUTPUT_PATH)/symbol.txt: $(LINKER_OUTPUTS)
	@echo '正在调用： NM (symbol listing)'
	$(CROSS_COMPILE)nm -n -l -C "BT_Audio_APP.adx" > $(SECONDARY_OUTPUT_PATH)/symbol.txt
	@echo '已结束构建： $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/readelf.txt: $(LINKER_OUTPUTS)
	@echo '正在调用： Readelf (ELF info listing)'
	$(CROSS_COMPILE)readelf -a "BT_Audio_APP.adx" > $(SECONDARY_OUTPUT_PATH)/readelf.txt
	@echo '已结束构建： $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/objdump.txt: $(LINKER_OUTPUTS)
	@echo '正在调用： Objdump (disassembly)'
	$(CROSS_COMPILE)objdump -x -d -C "BT_Audio_APP.adx" > $(SECONDARY_OUTPUT_PATH)/objdump.txt
	@echo '已结束构建： $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/$(BUILD_ARTIFACT_FILE_BASE_NAME).bin: $(LINKER_OUTPUTS)
	@echo '正在调用： Objcopy (object content copy)'
	$(CROSS_COMPILE)objcopy -S -O binary "BT_Audio_APP.adx" $(SECONDARY_OUTPUT_PATH)/$(BUILD_ARTIFACT_FILE_BASE_NAME).bin
	@echo '已结束构建： $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/.PHONY.size: $(LINKER_OUTPUTS)
	@echo '正在调用： Size (section size listing)'
	$(CROSS_COMPILE)size  "BT_Audio_APP.adx" | tee $(SECONDARY_OUTPUT_PATH)/.PHONY.size
	@echo '已结束构建： $@'
	@echo ' '

# 其他目标
clean:
	-$(RM) $(GPROF_OUT)$(PWR_OUT)$(SIZE_OUTPUTS)$(READELF_OUTPUTS)$(EXECUTABLES)$(OBJDUMP_OUTPUTS)$(OBJS)$(SYMBOL_OUTPUTS)$(S_UPPER_DEPS)$(GCOV_OUT)$(C_DEPS)$(OBJCOPY_OUTPUTS) BT_Audio_APP.adx
	-@echo ' '

pre-build:
	-../tools/script.bat
	-@echo ' '

post-build:
	-$(CROSS_COMPILE)objcopy -O binary "BT_Audio_APP.adx" output/BT_Audio_APP.bin ; $(CROSS_COMPILE)objcopy -O binary app_src/components/flash_manage/flash_param.o ../tools/merge_script/flash_param.bin ;  ../tools/script_post.bat; ../tools/script_tcmcheck.bat "BT_Audio_APP.adx"
	-@echo ' '

secondary-outputs: $(SYMBOL_OUTPUTS) $(READELF_OUTPUTS) $(OBJDUMP_OUTPUTS) $(OBJCOPY_OUTPUTS) $(SIZE_OUTPUTS)

.PHONY: all clean dependents
.SECONDARY: main-build pre-build post-build

-include ../makefile.targets
