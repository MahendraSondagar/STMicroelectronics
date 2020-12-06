/**
  @page Sigfox Readme file
 
  @verbatim
  ******************** (C) COPYRIGHT 2017 STMicroelectronics *******************
  * @file    Sigfox/ModemAT/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    30-April-2018
  * @brief   This application is a simple demo of a Sigfox Modem connecting to 
  *          a Sigfox Network. 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
   @endverbatim

@par Description

 This directory contains a set of source files that implements a simple modem  of 
 a Sigfox Object connecting to a Sigfox Network. The Sigfox modem run on
 B-L072Z-LRWAN1.
  ******************************************************************************

 ModemAT application shows how to eprform some basic operations like setting the
 RC zone, sending a frame, generate a continues wave, etc., by controlling the
 device from PC via an hyper terminal.
 Once the hyperterminal is connected, comand can be send, as for example:
   AT$ID?  gets the device ID
   AT$RCZ?  gets the RC zone configuration
   AT$RCZ=<value> sets the RC zone (1 for Europe, 2 for North America, 4 for South America)
   AT$SB=<value> sen a bit where value is 0 or 1
   AT$SF=<value> send a frame where value is a sequence of ASCII (e.g. 48454C4C4F to write HELLO)
   AT$CW=<value> generate a continues sinusoid at frequency <value>. <value>=0 to stop the generation.
   ATS300 send an out of band message
   ATS302=<value> set radio output power in dBm (range 5 to 20)
   ATS410=<value> set the crypto key 0: for private key, 1 for public key
   ATS400 this comand shall not be used on real Sigfox network, but just for certification purpose
  More details can be found by reading the user manual UM2245.pdf


@par Directory contents 

  - ModemAT/Inc/hw_conf.h                file to manage Cube SW family used and debug switch
  - ModemAT/Inc/stm32lXxx_hal_conf.h     Library Configuration file
  - ModemAT/Inc/stm32lXxx_it.h           Header for stm32lXxx_it.c
  - ModemAT/Inc/stm32lXxx_hw_conf.h      Header for stm32lXxx_hw_conf.c
  - ModemAT/Inc/hw_spi.h                 Header for hw_spi.c
  - ModemAT/Inc/hw_rtc.h                 Header for hw_rtc.c
  - ModemAT/Inc/hw_gpio.h                Header for hw_gpio.c
  - ModemAT/Inc/hw_tim2.h                Header for hw_tim2.c
  - ModemAT/Src/command.h                at command parser header
  - ModemAT/Src/at.h                     at command implementation header
  - ModemAT/Inc/hw.h                     group all hw interface
  - ModemAT/Inc/vcom.h                   interface to vcom.c 
  - ModemAT/Inc/tiny_sscanf.h            interface to tiny_sscanf.c 
  - ModemAT/Inc/tiny_printf.h            interface to tiny_printf.c 
  - ModemAT/Inc/debug.h                  interface to debug functionally
  - ModemAT/Inc/version .h               version file
  
  - ModemAT/Src/main.c                   Main program file
  - ModemAT/Src/stm32lXxx_it.c           STM32lXxx Interrupt handlers
  - ModemAT/Src/stm32lXxx_hal_msp.c      stm32lXxx specific hardware HAL code
  - ModemAT/Src/stm32lXxx_hw.c           stm32lXxx specific hardware driver code
  - ModemAT/Src/hw_spi.c                 spi driver
  - ModemAT/Src/hw_rtc.c                 rtc driver
  - ModemAT/Src/hw_gpio.c                gpio driver
  - ModemAT/Src/command.c                at command parser
  - ModemAT/Src/at.c                     at command implementation
  - ModemAT/Inc/hw_tim2.c                timer 2 driver
  - ModemAT/Src/vcom.c                   virtual com port interface on Terminal
  - ModemAT/Inc/tiny_sscanf.c            low foot print sscanf
  - ModemAT/Inc/tiny_printf.c            low foot print printf
  - ModemAT/Src/debug.c                  debug driver
 
@par Hardware and Software environment 


  - This example runs on STM32L072/82 embedded in the module
    
  - This application has been tested with STMicroelectronics:
    B-L072Z-LRWAN1 RevC boards 

  - Set Up:

             --------------------------  V    V  --------------------------
             |     B-L072Z-LRWAN1     |  |    |  |      Sigfox Netork     |
             |         with           |  |    |  |                        |
   ComPort<--|      Sigfox Modem      |--|    |--|                        |-->Web Server
             |                        |          |                        |
             --------------------------          --------------------------

  - Defining DEBUG in hw_conf.h allows keeping the debugger attached
    If DEBUG flag is not defined (hw_conf.h) the debugger disconnects in order to reduce power consumption

  - Depending on tool chain in order to connect the debugger again (when in low power) can be necessary 
    to keep the reset button pressed when starting the next download or
    to erase the chip memory. This to avoid the device to enter low power before debugger connection.
    It can also be necessary to reset the board after the download.

  - Defining TRACE in hw_conf.h allows to see the activity by connecting with an hyperterminal

  - In order to access Sigfox Network each device needs to be "Personalised" and "Activate".
    See ../SignatureGenerator/readme.txt
    Otherwise (for test purposes) a Sigfox Network Emulator can be used instead of real Sigfox Network 
    by using the AT command: ATS410=1

@par How to use it ? 
In order to make the program work, you must do the following :
  - Open your preferred toolchain 
  - Rebuild all files, erase the chip memory  and load your image into target memory
  - Run the example
  - Open 1 Terminal connected to the sigfox modem 
  - Terminal Config = 9600, 8b, 1 stopbit, no parity, no flow control ( in src/vcom.c)
  - JP9 must have pin 1 and 2 connected (TCXO_VCC managed by MCU) 
  - The solder bridge SB26 must be closed.(DIO4 from radio connected to PA5)
  - The solder bridge SB13 must be closed. (TCXO_OUT connected to PH0-OSC-IN)
  

   
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
