/**
  ******************** (C) COPYRIGHT STMicroelectronics ***********************
  * @file    readme.txt
  * @author  Central LAB
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief   Description of the SENSING1 application firmware
  ******************************************************************************
  * Attention
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

Application Description 

 This firmware package includes Components Device Drivers, Board Support Package
 and example application for the following STMicroelectronics elements:
 - STEVAL-MKSBOX1V1 (SensorTile.box) evaluation board that contains the following components:
	. MEMS sensor devices: HTS221, LPS22HH, LIS2MDL, LSM6DSOX
	. Analog microphone
	. Battery Charger
 - SD Data Log capabilities
 - Middleware libraries generated thanks to STM32CubeMx extension called X-CUBE-AI featuring example implementation of neural networks for real-time:
	- human activity recognition (HAR)
	- acoustic scene classification (ASC)
 - FatFs generic FAT file system module provides access the storage devices 
   such as memory card and hard disk.
 - FreeRTOS Real Time Kernel/Scheduler that allows applications to be organized as a collection of independent threads of execution
   (under MIT open source license)

 The Example application initializes all the Components and Library creating some Custom Bluetooth services:
 - The first service exposes all the HW and SW characteristics:
   . HW characteristics:
		. related to MEMS sensor devices: Temperature, Humidity, Pressure, Magnetometer, Gyroscope and Accelerometer
		  and Microphones Signal Noise dB level.
		. battery Gas Gauge and SD Data Log (for only SensorTile)
 - The second Service exposes the console services where we have stdin/stdout and stderr capabilities
 - The last Service is used for configuration purpose

 This example must be used with the related BlueMS Android/iOS application available on Play/itune store (Version 4.1.0 or higher),
 in order to read the sent information by Bluetooth Low Energy protocol

                              ------------------
                              | VERY IMPORTANT |
                              ------------------
 1) This example support the Firmware-Over-The-Air (FOTA) update using the BlueMS Android/iOS application (Version 4.1.0 or higher)
 2) This example must run starting at address 0x08004000 in memory and works ONLY if the BootLoader 
 is saved at the beginning of the FLASH (address 0x08000000)
 
 For each IDE (IAR/µVision/System Workbench) there are some scripts *.bat/*.sh that makes the following operations:
 - Full Flash Erase
 - Load the BootLoader on the rigth flash region
 - Load the Program (after the compilation) on the rigth flash region (This could be used for a FOTA)
 - Dump back one single binary that contain BootLoader+Program that could be 
   flashed at the flash beginning (address 0x08000000) (This COULD BE NOT used for FOTA)
 - Reset the board

 
Inside the Binary Directory there are the following binaries:
Binary/
    +-- SENSING1.bin         (Program without BootLoader. COULD BE USED     for FOTA)
    +-- SENSING1_BL.bin      (Program with    BootLoader. COULD NOT BE USED for FOTA)


@par Hardware and Software environment

  - This example runs on STEVAL-STLKT01V1 (SensorTile) evaluation board and it
    can be easily tailored to any other supported device and development board.

  - This example must be used with the related BlueMS Android/iOS application (Version 4.1.0 or higher) available on
    the Google Play or Apple App Store, in order to read the sent information by Bluetooth Low Energy protocol

@par STM32Cube packages:
  - STM32L4xx drivers from STM32CubeL4 V1.14.0
@par X-CUBE packages:
  - X-CUBE-BLE1 V3.3.0
@par STEVAL-MKSBOX1V1:
  - STEVAL-MKSBOX1V1 V1.0.0

@par How to use it ?

This package contains projects for 3 IDEs viz. IAR, µVision and CubeIDE.
In order to make the  program work, you must do the following:
 - WARNING: before opening the project with any toolchain be sure your folder
   installation path is not too in-depth since the toolchain may report errors
   after building.

For IAR:
 - Open IAR toolchain (this firmware has been successfully tested with Embedded Workbench V8.32.3).
 - Open the IAR project file EWARM\SENSING1.eww
 - Rebuild all files and run the script that you find on the same directory: CleanSENSING1.bat

For µVision:
 - Open µVision toolchain (this firmware has been successfully tested with MDK-ARM Professional Version: 5.27.0)
 - Open the µVision project file MDK-ARM\Project.uvprojx
 - Rebuild all files and run the script that you find on the same directory: CleanSENSING1.bat
		
For STM32CubeIDE:
 - Open STM32CubeIDE (this firmware has been successfully tested with STM32CubeIDE Version 1.0.2)
 - Set the default workspace proposed by the IDE (please be sure that there are not spaces in the workspace path).
 - Press "File" -> "Open Project form Filesystem" -> "choose the path where the Cube Ide project is located 
   (it should be STM32CubeIDE\). 
 - Rebuild all files and and run one of these scripts that you find on the same directory:
   - if you are on windows and you had installed the STM32 ST-Link utility: CleanSENSING1.bat
   - Otherwise (Linux/iOS or Windows without the STM32 ST-Link Utility): CleanSENSING1.sh
		
 /******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
