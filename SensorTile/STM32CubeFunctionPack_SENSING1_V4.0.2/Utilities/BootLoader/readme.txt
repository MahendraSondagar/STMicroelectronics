/**
******************************************************************************
* file    readme.txt
* Version V2.0.0
* date    21-May-2019
******************************************************************************
* Attention
*
* COPYRIGHT(c) 2019 STMicroelectronics
*
* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
*
*        http://www.st.com/software_license_agreement_liberty_v2
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

This firmware is one Boot Loader that must run at the Flash beginning (0x08000000 address) and has the purpose to apply the Firmware-Over-The-Air (FOTA) updated received, already checked and stored in Flash, or run the program normally if there is not one update.

For STM32L4R9ZI the 2Mbytes of Flash is Divided on 2 banks of 1Mbytes each one, and each bank is splitted on 256 pages of 4Kbytes.

|Page 0->7  | Page 4->255       | Page 256->511 |
|16K        | 1008K             | 1024K         |
|0x08000000 |0x08004000         |0x08100000     |
|-----------------------------------------------|
|  Reg1     |       Reg2        |    Reg3       |
|BootLoader | Running Program   |    FOTA       |


This Boot Loader must be Loaded on Sector 0 and it checks if there is one FOTA stored (0x08100000 address):
- If YES:
  - it erases the necessary space on Region 2 (Reg2)
  - it copies the FOTA on that sectors
  - it erases the FOTA region (Reg3) after the update.
- if NOT:
  - it works like a trampoline for executing the normal program stored from Region 2 (0x08004000 address)

The FOTA must be less than 1008Kbytes





For STM32L476/75 the `Mbytes of Flash is Divided on 2 banks of 512Kbytes each one, and each bank is splitted on 256 pages of 2Kbytes.

|Page 0->7  | Page 4->255       | Page 256->511 |
|16K        | 504K              | 512K          |
|0x08000000 |0x08004000         |0x08080000     |
|-----------------------------------------------|
|  Reg1     |       Reg2        |    Reg3       |
|BootLoader | Running Program   |    FOTA       |


This Boot Loader must be Loaded on Sector 0 and it checks if there is one FOTA stored (0x08080000 address):
- If YES:
  - it erases the necessary space on Region 2 (Reg2)
  - it copies the FOTA on that sectors
  - it erases the FOTA region (Reg3) after the update.
- if NOT:
  - it works like a trampoline for executing the normal program stored from Region 2 (0x08004000 address)

The FOTA must be less than 504Kbytes

The "Running Program" and the FOTA are compiled for running from 0x08004000 address. If they are placed at the beginning of the FLASH they doesn't work.
And without the BootLoader that programs could not be executed.

 /******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/