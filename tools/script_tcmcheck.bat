@echo OFF 

setlocal enabledelayedexpansion

rem nds32le-elf-objdump -h  %1 | grep.exe ".text " | tr.exe -s " " | cut.exe -d" " -f6
for /f %%a in ('nds32le-elf-objdump -h  %1 ^| grep.exe ".tcm_section " ^| tr.exe -s " " ^| cut.exe -d" " -f6') do set "tcmAddr=%%a"
for /f %%a in ('nds32le-elf-objdump -h  %1 ^| grep.exe ".driver.isr " ^| tr.exe -s " " ^| cut.exe -d" " -f6') do set "driverAddr=%%a"
for /f %%a in ('nds32le-elf-objdump -h  %1 ^| grep.exe ".text " ^| tr.exe -s " " ^| cut.exe -d" " -f6') do set "textAddr=%%a"

set /a threshold = 21 * 1024

echo TcmSectAddr 0x%tcmAddr%
echo DriverSectAddr 0x%driverAddr%
echo TextSectAddr 0x%textAddr%

set "hTcm=%tcmAddr:0x=%"
set "hTcm=%hTcm:0X=%"
set "hDriver=%driverAddr:0x=%"
set "hDriver=%hDriver:0X=%"
set "hText=%textAddr:0x=%"
set "hText=%hText:0X=%"

set /a decTcm=0x%hTcm%
set /a decDriver=0x%hDriver%
set /a decText=0x%hText%

set /a modTcmEnd = decDriver %% 65536
set /a modDriverEnd = decText %% 65536

if "!modDriverEnd!" gtr "!threshold!" (
    echo [ERROR] DriverEnd = !modDriverEnd!  out  TCM_SIZE = !threshold!
    echo TcmSectionEnd = !modTcmEnd!
    exit /b 1
) else (
    echo [OK] DriverEnd = !modDriverEnd!  in  TCM_SIZE = !threshold!
    echo TcmSectionEnd = !modTcmEnd!
    exit /b 0
)