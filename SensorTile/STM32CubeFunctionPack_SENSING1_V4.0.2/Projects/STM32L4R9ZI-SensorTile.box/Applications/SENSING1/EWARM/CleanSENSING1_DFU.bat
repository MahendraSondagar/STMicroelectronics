@echo off
set CUBE_PROG="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
set NAMESENSING1=SENSING1
set PORTNAME=usb1
set BOOTLOADER="..\..\..\..\..\Utilities\BootLoader\STM32L4R9ZI\BootLoaderL4R9.bin"
color 0F
echo                /******************************************/
echo                           Clean FP-AI-SENSING1
echo                /******************************************/
echo                              Full Chip Erase
echo                /******************************************/
%CUBE_PROG% -c port=%PORTNAME% -e all
echo                /******************************************/
echo                              Install BootLoader
echo                /******************************************/
%CUBE_PROG% -c port=%PORTNAME% -d %BOOTLOADER% 0x08000000 -v
echo                /******************************************/
echo                          Install FP-AI-SENSING1
echo                /******************************************/
%CUBE_PROG% -c port=%PORTNAME% -d %NAMESENSING1%.bin 0x08004000 -v
echo                /******************************************/
echo                      Dump FP-AI-SENSING1 + BootLoader
echo                /******************************************/
set offset_size=0x4000
for %%I in (%NAMESENSING1%.bin) do set application_size=%%~zI
echo %NAMESENSING1%.bin size is %application_size% bytes
set /a size=%offset_size%+%application_size%
echo Dumping %offset_size% + %application_size% = %size% bytes ...
echo ..........................
%CUBE_PROG% -c port=%PORTNAME% -u 0x08000000 %size% %NAMESENSING1%_BL.bin
echo ..........................
echo Reset or power cycle the board to start the STM32 application.
if NOT "%1" == "SILENT" pause