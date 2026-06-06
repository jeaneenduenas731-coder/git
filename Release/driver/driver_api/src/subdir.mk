################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../driver/driver_api/src/adc_interface.c \
../driver/driver_api/src/can_interface.c \
../driver/driver_api/src/dac_interface.c \
../driver/driver_api/src/fft_api.c \
../driver/driver_api/src/flash_interface.c \
../driver/driver_api/src/i2c_host.c \
../driver/driver_api/src/i2c_interface.c \
../driver/driver_api/src/i2s_interface.c \
../driver/driver_api/src/pmu_interface.c \
../driver/driver_api/src/sadc_interface.c \
../driver/driver_api/src/sd_card.c \
../driver/driver_api/src/spim_interface.c \
../driver/driver_api/src/sw_uart.c \
../driver/driver_api/src/uarts_interface.c 

OBJS += \
./driver/driver_api/src/adc_interface.o \
./driver/driver_api/src/can_interface.o \
./driver/driver_api/src/dac_interface.o \
./driver/driver_api/src/fft_api.o \
./driver/driver_api/src/flash_interface.o \
./driver/driver_api/src/i2c_host.o \
./driver/driver_api/src/i2c_interface.o \
./driver/driver_api/src/i2s_interface.o \
./driver/driver_api/src/pmu_interface.o \
./driver/driver_api/src/sadc_interface.o \
./driver/driver_api/src/sd_card.o \
./driver/driver_api/src/spim_interface.o \
./driver/driver_api/src/sw_uart.o \
./driver/driver_api/src/uarts_interface.o 

C_DEPS += \
./driver/driver_api/src/adc_interface.d \
./driver/driver_api/src/can_interface.d \
./driver/driver_api/src/dac_interface.d \
./driver/driver_api/src/fft_api.d \
./driver/driver_api/src/flash_interface.d \
./driver/driver_api/src/i2c_host.d \
./driver/driver_api/src/i2c_interface.d \
./driver/driver_api/src/i2s_interface.d \
./driver/driver_api/src/pmu_interface.d \
./driver/driver_api/src/sadc_interface.d \
./driver/driver_api/src/sd_card.d \
./driver/driver_api/src/spim_interface.d \
./driver/driver_api/src/sw_uart.d \
./driver/driver_api/src/uarts_interface.d 


# Each subdirectory must supply rules for building sources it contributes
driver/driver_api/src/%.o: ../driver/driver_api/src/%.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -DFUNC_OS_EN=1 -DCFG_APP_CONFIG -DHAVE_CONFIG_H -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/otg_host/inc" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/new" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/hmi/uart" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/components/otg/host/inc" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_framework/wireless_engine" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_framework/wireless_engine/sbc" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_framework/wireless_engine/sbc/encoder/include" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/app_mode_wirelessin" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/components/audio/effect_control" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/components/bt_source" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/hmi/can" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/components/otg/device/inc" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/components/audio/music_parameter" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/components/audio/music_parameter/mic" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/components/audio/music_parameter/uac" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/components/audio/music_parameter/music" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/components/audio/music_parameter/karaoke" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/roboeffect/roboeffect_lib" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/roboeffect/roboeffect_lib/wraps" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/roboeffect/third_party_effect" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/roboeffect/third_party_effect/simple_gain" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/cec/inc" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/app_mode_hdmi" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/components/audio" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/components/soft_watchdog" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/app_mode_spdif" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_framework/slow_device_engine" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/app_mode__common" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/app_mode_bt" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/app_mode_i2s" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/app_mode_idle" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/app_mode_linein" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/app_mode_media" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/app_mode_radio" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/app_mode_usb_audio" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/components/bluetooth" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/components/ble" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/components/flash_manage" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/components/rtc" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/components/upgrade" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/hmi/detect" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/hmi/display" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/hmi/fm" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/hmi/key" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/system_config" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/flashboot" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_framework/mode_engine" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_framework/bluetooth_engine" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_framework/audio_engine" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/driver/driver_api/inc" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/driver/driver/inc" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/app_src/power" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/rtc/inc" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/fatfs/inc" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/bluetooth/inc" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/audio/inc" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/mv_utils/inc" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/lrc/inc" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/rtos/rtos_api" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/rtos/freertos/inc" -I"/cygdrive/E/svn/GuangzhouXuanYin/DB96/Software/MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0/BT_Audio_APP/middleware/roboeffect/third_party_effect/ai_denoise" -Os1 -mcmodel=medium -fwrapv -g3 -Wall -mcpu=d1088-spu -ffunction-sections -fdata-sections -c -fmessage-length=0 -ldsp -mext-dsp -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '


