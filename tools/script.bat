cd ..
if exist mp2 (rd /s /q mp2)
md mp2
cd remind_res
for %%i in (16K//*.mp3) do (..\tools\remind_script\ffmpeg.exe -i 16K//%%i -ab 24000 -ac 1 -ar 16000 ..\mp2//%%i.mp2 >> info.txt 2>&1)
del info.txt
cd ..
for %%i in (remind_res//*.mp3) do (tools\remind_script\ffmpeg.exe -i remind_res//%%i -ab 48000 -ac 1 mp2//%%i.mp2 >> info.txt 2>&1)
del info.txt
cd mp2
ren *.mp2 *.
cd ..
tools\remind_script\MergeAudio2BinNew.exe -a 0x0 -i mp2 -o tools\remind_script\
rd /s /q mp2
fc tools\remind_script\sound_remind_item.h app_framework\audio_engine\remind_sound_item.h
if %errorlevel%==1 (copy tools\remind_script\sound_remind_item.h app_framework\audio_engine\remind_sound_item.h)
@REM tools\libs_copy_script\copyLibs.exe -s ..\..\tools\script_copy.ini
tools\libs_copy_script\copyLibs.exe -s tools\script_copy.ini
tools\libs_copy_script\copyLibs.exe -s tools\script_copy_funlist.ini
grep '^#define CFG_DOUBLE_KEY_EN' app_src\system_config\app_config.h
if %errorlevel%==0 (goto found) else (goto not_found)
:found
grep '^#define CFG_DOUBLE_KEY_EN' app_src\system_config\app_config.h | grep '//'
if %errorlevel%==1 (nds_ldsag.exe -t tools\nds32_template.txt tools\BP15x_Dkey.sag -o BP15x.ld) else (nds_ldsag.exe -t tools\nds32_template.txt tools\BP15x.sag -o BP15x.ld)
exit
:not_found
nds_ldsag.exe -t tools\nds32_template.txt tools\BP15x.sag -o BP15x.ld
exit