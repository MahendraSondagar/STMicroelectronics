/**
  ******************************************************************************
  * @file    x_nucleo_cca02m1_audio.h
  * @version V4.0.0
  * @author  Central Labs
  * @brief   This file contains the common defines and functions prototypes for
  *          the x_nucleo_cca02m1_audio.c driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef X_NUCLEO_CCA02M1_AUDIO_H
#define X_NUCLEO_CCA02M1_AUDIO_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "x_nucleo_cca02m1_conf.h"
#include "x_nucleo_cca02m1_errno.h"
#include <stdlib.h>

/* Include PDM to PCM lib header file */
#include "pdm2pcm_glo.h"

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup X_NUCLEO_CCA02M1
  * @{
  */
    
/** @addtogroup X_NUCLEO_CCA02M1_AUDIO 
  * @{
  */
   
/** @defgroup X-NUCLEO-CCA02M1_AUDIO_Exported_Variables
 * @{
 */
#ifdef USE_STM32L4XX_NUCLEO
extern DMA_HandleTypeDef               hDmaDfsdm[];
#else
extern I2S_HandleTypeDef                 hAudioInI2s;

#define DMA_MAX(_X_)                (((_X_) <= DMA_MAX_SZE)? (_X_):DMA_MAX_SZE)
#define HTONS(A)  ((((uint16_t)(A) & 0xff00) >> 8) | \
                   (((uint16_t)(A) & 0x00ff) << 8))
#endif

/**
 * @}
 */   

/** @defgroup X_NUCLEO_CCA02M1_AUDIO_Exported_Types X_NUCLEO_CCA02M1_AUDIO Exported Types
  * @{
  */

typedef struct {
	int32_t Z; 
	int32_t oldOut; 
	int32_t oldIn; 
}HP_FilterState_TypeDef;  
  
typedef struct
{                                   
  uint32_t                    Device;                                           
  uint32_t                    SampleRate;                                         
  uint32_t                    BitsPerSample;                                          
  uint32_t                    ChannelsNbr;                                         
  uint32_t                    Volume;
}BSP_AUDIO_Init_t;

typedef struct
{
  uint32_t                    Instance;            /* Audio IN instance              */  
  uint32_t                    Device;              /* Audio IN device to be used     */ 
  uint32_t                    SampleRate;          /* Audio IN Sample rate           */
  uint32_t                    BitsPerSample;       /* Audio IN Sample resolution     */
  uint32_t                    ChannelsNbr;         /* Audio IN number of channel     */
  uint16_t                    *pBuff;              /* Audio IN record buffer         */
  uint8_t                     **pMultiBuff;        /* Audio IN multi-buffer          */
  uint32_t                    Size;                /* Audio IN record buffer size    */
  uint32_t                    Volume;              /* Audio IN volume                */
  uint32_t                    State;               /* Audio IN State                 */
  uint32_t                    IsMultiBuff;         /* Audio IN multi-buffer usage    */
  uint32_t                    IsMspCallbacksValid; /* Is Msp Callbacks registred     */
  HP_FilterState_TypeDef 	  HP_Filters[4];       /*!< HP filter state for each channel*/
  uint32_t DecimationFactor;
}AUDIO_IN_Ctx_t;

typedef struct
{
  uint32_t Mode;            
  uint32_t Standard;       
  uint32_t DataFormat;     
  uint32_t MCLKOutput;      
  uint32_t AudioFreq;       
  uint32_t CPOL;            
  uint32_t ClockSource;     
  uint32_t FullDuplexMode; 
}MX_I2S_Config;

typedef struct
{
  uint32_t Mode;
  uint32_t Direction;  
  uint32_t DataSize;    
  uint32_t CLKPolarity; 
  uint32_t CLKPhase;   
  uint32_t NSS;        
  uint32_t BaudRatePrescaler;
  uint32_t FirstBit;       
  uint32_t TIMode;         
  uint32_t CRCCalculation; 
  uint32_t CRCPolynomial;	
}MX_SPI_Config;

#ifdef USE_STM32L4XX_NUCLEO

typedef struct
{
  /* Filter parameters */
  DFSDM_Filter_TypeDef   *FilterInstance;
  uint32_t               RegularTrigger;
  uint32_t               SincOrder;   
  uint32_t               Oversampling;
  /* Channel parameters */
  DFSDM_Channel_TypeDef *ChannelInstance;
  uint32_t              DigitalMicPins;
  uint32_t              DigitalMicType;
  uint32_t              Channel4Filter;
  uint32_t              ClockDivider;
  uint32_t              RightBitShift; 
}MX_DFSDM_Config;  

#if ((USE_HAL_DFSDM_REGISTER_CALLBACKS == 1) || (USE_HAL_SAI_REGISTER_CALLBACKS == 1))
typedef struct
{
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)	
  pSAI_CallbackTypeDef            pMspSaiInitCb;
  pSAI_CallbackTypeDef            pMspSaiDeInitCb;
#endif
#if (USE_HAL_DFSDM_REGISTER_CALLBACKS == 1)  
  pDFSDM_Filter_CallbackTypeDef   pMspFltrInitCb; 
  pDFSDM_Filter_CallbackTypeDef   pMspFltrDeInitCb;
  pDFSDM_Channel_CallbackTypeDef  pMspChInitCb; 
  pDFSDM_Channel_CallbackTypeDef  pMspChDeInitCb;
#endif /* (USE_HAL_DFSDM_REGISTER_CALLBACKS == 1) */  
}BSP_AUDIO_IN_Cb_t;
#endif /* ((USE_HAL_DFSDM_REGISTER_CALLBACKS == 1) || (USE_HAL_SAI_REGISTER_CALLBACKS == 1)) */

#endif

/**
  * @}
  */ 

/** @defgroup X_NUCLEO_CCA02M1_AUDIO_Exported_Constants X_NUCLEO_CCA02M1_AUDIO Exported Constants
  * @{
  */

#define AUDIO_VOLUME_INPUT      64U
    
#define BSP_AUDIO_IN_IT_PRIORITY 6

/* AUDIO FREQUENCY */
#define AUDIO_FREQUENCY_192K     192000U
#define AUDIO_FREQUENCY_176K     176400U
#define AUDIO_FREQUENCY_96K       96000U
#define AUDIO_FREQUENCY_88K       88200U
#define AUDIO_FREQUENCY_48K       48000U
#define AUDIO_FREQUENCY_44K       44100U
#define AUDIO_FREQUENCY_32K       32000U
#define AUDIO_FREQUENCY_22K       22050U
#define AUDIO_FREQUENCY_16K       16000U
#define AUDIO_FREQUENCY_11K       11025U
#define AUDIO_FREQUENCY_8K         8000U 

/* AUDIO RESOLUTION */
#define AUDIO_RESOLUTION_16b                16U
#define AUDIO_RESOLUTION_32b                32U


/*------------------------------------------------------------------------------
                          USER I2S / SPI defines parameters
 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
                        AUDIO IN defines parameters
------------------------------------------------------------------------------*/ 
  /* I2S Configuration defines */
#define AUDIO_IN_I2S_INSTANCE                                    SPI2
#define AUDIO_IN_I2S_CLK_ENABLE()                                __SPI2_CLK_ENABLE()
#define AUDIO_IN_I2S_SCK_PIN                                     GPIO_PIN_13
#define AUDIO_IN_I2S_SCK_GPIO_PORT                               GPIOB
#define AUDIO_IN_I2S_SCK_GPIO_CLK_ENABLE()                       __GPIOB_CLK_ENABLE()
#define AUDIO_IN_I2S_SCK_AF                                      GPIO_AF5_SPI2  
#define AUDIO_IN_I2S_MOSI_PIN                                    GPIO_PIN_15
#define AUDIO_IN_I2S_MOSI_GPIO_PORT                              GPIOB
#define AUDIO_IN_I2S_MOSI_GPIO_CLK_ENABLE()                      __GPIOB_CLK_ENABLE();
#define AUDIO_IN_I2S_MOSI_AF                                     GPIO_AF5_SPI2
  
  /* I2S DMA definitions */
#define AUDIO_IN_I2S_DMAx_CLK_ENABLE()                          __DMA1_CLK_ENABLE()
#define AUDIO_IN_I2S_DMAx_STREAM                                DMA1_Stream3
#define AUDIO_IN_I2S_DMAx_CHANNEL                               DMA_CHANNEL_0
#define AUDIO_IN_I2S_DMAx_IRQ                                   DMA1_Stream3_IRQn
#define AUDIO_IN_I2S_DMAx_PERIPH_DATA_SIZE                      DMA_PDATAALIGN_HALFWORD
#define AUDIO_IN_I2S_DMAx_MEM_DATA_SIZE                         DMA_MDATAALIGN_HALFWORD  
#define AUDIO_IN_I2S_IRQHandler                                 DMA1_Stream3_IRQHandler

/* DFSDM Configuration defines */
#define AUDIO_DFSDMx_MIC1_CHANNEL                    DFSDM_Channel2  
#define AUDIO_DFSDMx_MIC2_CHANNEL                    DFSDM_Channel1
#define AUDIO_DFSDMx_MIC3_CHANNEL                    DFSDM_Channel7  
#define AUDIO_DFSDMx_MIC4_CHANNEL                    DFSDM_Channel6

#define AUDIO_DFSDMx_MIC1_CHANNEL_FOR_FILTER         DFSDM_CHANNEL_2  
#define AUDIO_DFSDMx_MIC2_CHANNEL_FOR_FILTER         DFSDM_CHANNEL_1
#define AUDIO_DFSDMx_MIC3_CHANNEL_FOR_FILTER         DFSDM_CHANNEL_7  
#define AUDIO_DFSDMx_MIC4_CHANNEL_FOR_FILTER         DFSDM_CHANNEL_6

#define AUDIO_DFSDMx_MIC1_FILTER                     DFSDM_Filter0
#define AUDIO_DFSDMx_MIC2_FILTER                     DFSDM_Filter1
#define AUDIO_DFSDMx_MIC3_FILTER                     DFSDM_Filter2
#define AUDIO_DFSDMx_MIC4_FILTER                     DFSDM_Filter3
#define AUDIO_DFSDMx_CLK_ENABLE()                    __HAL_RCC_DFSDM_CLK_ENABLE()

/* DATIN for MIC1 */
#define AUDIO_DFSDMx_DATIN_MIC1_PIN                  GPIO_PIN_14
#define AUDIO_DFSDMx_DATIN_MIC1_AF                   GPIO_AF6_DFSDM
#define AUDIO_DFSDMx_DATIN_MIC1_GPIO_PORT            GPIOB
#define AUDIO_DFSDMx_DATIN_MIC1_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE() 

/* DATIN for MIC2 */
#define AUDIO_DFSDMx_DATIN_MIC2_PIN                  GPIO_PIN_14
#define AUDIO_DFSDMx_DATIN_MIC2_AF                   GPIO_AF6_DFSDM
#define AUDIO_DFSDMx_DATIN_MIC2_GPIO_PORT            GPIOB
#define AUDIO_DFSDMx_DATIN_MIC2_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE() 

/* DATIN for MIC3 */
#define AUDIO_DFSDMx_DATIN_MIC3_PIN                  GPIO_PIN_10
#define AUDIO_DFSDMx_DATIN_MIC3_AF                   GPIO_AF6_DFSDM
#define AUDIO_DFSDMx_DATIN_MIC3_GPIO_PORT            GPIOB
#define AUDIO_DFSDMx_DATIN_MIC3_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE() 

/* DATIN for MIC4 */
#define AUDIO_DFSDMx_DATIN_MIC4_PIN                  GPIO_PIN_10
#define AUDIO_DFSDMx_DATIN_MIC4_AF                   GPIO_AF6_DFSDM
#define AUDIO_DFSDMx_DATIN_MIC4_GPIO_PORT            GPIOB
#define AUDIO_DFSDMx_DATIN_MIC4_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()


/* CKOUT for all mics (GPIOC_PIN_2)*/                                                           
#define AUDIO_DFSDMx_CKOUT_PIN                       GPIO_PIN_2
#define AUDIO_DFSDMx_CKOUT_AF                        GPIO_AF6_DFSDM
#define AUDIO_DFSDMx_CKOUT_GPIO_PORT                 GPIOC
#define AUDIO_DFSDMx_CKOUT_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()


/* DFSDM DMA MIC1 and MIC2 channels definitions */
#define AUDIO_DFSDMx_DMAx_MIC1_STREAM                DMA1_Channel4
#define AUDIO_DFSDMx_DMAx_MIC1_IRQ                   DMA1_Channel4_IRQn
#define AUDIO_DFSDM_DMAx_MIC1_IRQHandler             DMA1_Channel4_IRQHandler

#define AUDIO_DFSDMx_DMAx_MIC2_STREAM                DMA1_Channel5
#define AUDIO_DFSDMx_DMAx_MIC2_IRQ                   DMA1_Channel5_IRQn
#define AUDIO_DFSDM_DMAx_MIC2_IRQHandler             DMA1_Channel5_IRQHandler

#define AUDIO_DFSDMx_DMAx_MIC3_STREAM                DMA1_Channel6
#define AUDIO_DFSDMx_DMAx_MIC3_IRQ                   DMA1_Channel6_IRQn
#define AUDIO_DFSDM_DMAx_MIC3_IRQHandler             DMA1_Channel6_IRQHandler

#define AUDIO_DFSDMx_DMAx_MIC4_STREAM                DMA1_Channel7
#define AUDIO_DFSDMx_DMAx_MIC4_IRQ                   DMA1_Channel7_IRQn
#define AUDIO_DFSDM_DMAx_MIC4_IRQHandler             DMA1_Channel7_IRQHandler

#define AUDIO_DFSDMx_DMAx_PERIPH_DATA_SIZE           DMA_PDATAALIGN_WORD
#define AUDIO_DFSDMx_DMAx_MEM_DATA_SIZE              DMA_MDATAALIGN_WORD
#define AUDIO_DFSDMx_DMAx_CLK_ENABLE()               __HAL_RCC_DMA1_CLK_ENABLE()                                                     

#ifdef USE_SPI3
    /* SPI Configuration defines */  
  
#define AUDIO_IN_SPI_INSTANCE                                            SPI3
#define AUDIO_IN_SPI_CLK_ENABLE()                               __SPI3_CLK_ENABLE()
#define AUDIO_IN_SPI_SCK_GPIO_CLK_ENABLE()                      __GPIOC_CLK_ENABLE()
#define AUDIO_IN_SPI_MISO_GPIO_CLK_ENABLE()                     __GPIOC_CLK_ENABLE() 
#define AUDIO_IN_SPI_MOSI_GPIO_CLK_ENABLE()                     __GPIOC_CLK_ENABLE()   
#define AUDIO_IN_SPI_FORCE_RESET()                              __SPI3_FORCE_RESET()
#define AUDIO_IN_SPI_RELEASE_RESET()                            __SPI3_RELEASE_RESET()
#define AUDIO_IN_SPI_SCK_PIN                                    GPIO_PIN_10
#define AUDIO_IN_SPI_SCK_GPIO_PORT                              GPIOC
#define AUDIO_IN_SPI_SCK_AF                                     GPIO_AF6_SPI3
#define AUDIO_IN_SPI_MOSI_PIN                                   GPIO_PIN_12
#define AUDIO_IN_SPI_MOSI_GPIO_PORT                             GPIOC
#define AUDIO_IN_SPI_MOSI_AF                                    GPIO_AF6_SPI3
  
  /* SPI DMA definitions */
#define AUDIO_IN_SPI_DMAx_CLK_ENABLE()                          __DMA1_CLK_ENABLE()
#define AUDIO_IN_SPI_RX_DMA_CHANNEL                             DMA_CHANNEL_0
#define AUDIO_IN_SPI_RX_DMA_STREAM                              DMA1_Stream2 
#define AUDIO_IN_SPI_DMA_RX_IRQn                                DMA2_Stream2_IRQn
#define AUDIO_IN_SPI_DMA_RX_IRQHandler                          DMA2_Stream2_IRQHandler
  
#else
  /* SPI Configuration defines */  
#define AUDIO_IN_SPI_INSTANCE                                            SPI1
#define AUDIO_IN_SPI_CLK_ENABLE()                               __SPI1_CLK_ENABLE()
#define AUDIO_IN_SPI_SCK_GPIO_CLK_ENABLE()                      __GPIOA_CLK_ENABLE()
#define AUDIO_IN_SPI_MISO_GPIO_CLK_ENABLE()                     __GPIOA_CLK_ENABLE() 
#define AUDIO_IN_SPI_MOSI_GPIO_CLK_ENABLE()                     __GPIOA_CLK_ENABLE()   
#define AUDIO_IN_SPI_FORCE_RESET()                              __SPI1_FORCE_RESET()
#define AUDIO_IN_SPI_RELEASE_RESET()                            __SPI1_RELEASE_RESET()
#define AUDIO_IN_SPI_SCK_PIN                                    GPIO_PIN_5
#define AUDIO_IN_SPI_SCK_GPIO_PORT                              GPIOA
#define AUDIO_IN_SPI_SCK_AF                                     GPIO_AF5_SPI1
#define AUDIO_IN_SPI_MOSI_PIN                                   GPIO_PIN_7
#define AUDIO_IN_SPI_MOSI_GPIO_PORT                             GPIOA
#define AUDIO_IN_SPI_MOSI_AF                                    GPIO_AF5_SPI1
  
  /* SPI DMA definitions */
#define AUDIO_IN_SPI_DMAx_CLK_ENABLE()                          __DMA2_CLK_ENABLE()
#define AUDIO_IN_SPI_RX_DMA_CHANNEL                             DMA_CHANNEL_3
#define AUDIO_IN_SPI_RX_DMA_STREAM                              DMA2_Stream2 
#define AUDIO_IN_SPI_DMA_RX_IRQn                                DMA2_Stream2_IRQn
#define AUDIO_IN_SPI_DMA_RX_IRQHandler                          DMA2_Stream2_IRQHandler
  
#endif

/* AUDIO TIMER definitions */
#define AUDIO_IN_TIMER                                     TIM3
#define AUDIO_IN_TIMER_CLK_ENABLE()                        __TIM3_CLK_ENABLE()  
#define AUDIO_IN_TIMER_CHOUT_AF                            GPIO_AF2_TIM3
#define AUDIO_IN_TIMER_CHOUT_PIN                           GPIO_PIN_5
#define AUDIO_IN_TIMER_CHOUT_GPIO_PORT                     GPIOB
#define AUDIO_IN_TIMER_CHOUT_GPIO_PORT_CLK_ENABLE()        __GPIOB_CLK_ENABLE()  
#define AUDIO_IN_TIMER_CHIN_AF                             GPIO_AF2_TIM3
#define AUDIO_IN_TIMER_CHIN_PIN                            GPIO_PIN_4
#define AUDIO_IN_TIMER_CHIN_GPIO_PORT                      GPIOB
#define AUDIO_IN_TIMER_CHIN_GPIO_PORT_CLK_ENABLE()         __GPIOB_CLK_ENABLE() 

/* Audio In devices */ 

/* Analog microphone input from 3.5 audio jack connector */
#define AUDIO_IN_ANALOG_MIC        0x00U 
/* MP34DT01TR digital microphone on PCB top side */
#define AUDIO_IN_DIGITAL_MIC1      0x10U
#define AUDIO_IN_DIGITAL_MIC2      0x20U
#define AUDIO_IN_DIGITAL_MIC3      0x40U
#define AUDIO_IN_DIGITAL_MIC4      0x80U
#define AUDIO_IN_DIGITAL_MIC_LAST  AUDIO_IN_DIGITAL_MIC4
#define AUDIO_IN_DIGITAL_MIC       (AUDIO_IN_DIGITAL_MIC1 | AUDIO_IN_DIGITAL_MIC2 | AUDIO_IN_DIGITAL_MIC3 | AUDIO_IN_DIGITAL_MIC4)
#define DFSDM_MIC_NUMBER           AUDIO_CHANNELS

/* Default Audio IN internal buffer size */   
#define DEFAULT_AUDIO_IN_BUFFER_SIZE        (AUDIO_SAMPLING_FREQUENCY/1000)*2

/* Buffer size defines for F4 and F7*/

#define CHANNEL_DEMUX_MASK                    	0x55
    
#define MAX_MIC_FREQ                 	  3072  /*KHz*/
#define MAX_AUDIO_IN_CHANNEL_NBR_PER_IF   2 
#define MAX_AUDIO_IN_CHANNEL_NBR_TOTAL    4 

/*Number of millisecond of audio at each DMA interrupt*/
#define N_MS_PER_INTERRUPT               (1)
    
/*BSP internal buffer size in half words (16 bits)*/  
#define PDM_INTERNAL_BUFFER_SIZE_I2S          ((MAX_MIC_FREQ / 8) * MAX_AUDIO_IN_CHANNEL_NBR_PER_IF * N_MS_PER_INTERRUPT)
#if MAX_AUDIO_IN_CHANNEL_NBR_TOTAL > 2
#define PDM_INTERNAL_BUFFER_SIZE_SPI          ((MAX_MIC_FREQ / 8) * MAX_AUDIO_IN_CHANNEL_NBR_PER_IF * N_MS_PER_INTERRUPT)
#else
#define PDM_INTERNAL_BUFFER_SIZE_SPI          1
#endif

/* Audio In states */
#define AUDIO_IN_STATE_RESET               0U
#define AUDIO_IN_STATE_RECORDING           1U
#define AUDIO_IN_STATE_STOP                2U
#define AUDIO_IN_STATE_PAUSE               3U

/* Audio In instances number:
   Instance 0 is I2S / SPI path
   Instance 1 is DFSDM path
   Instance 2 is PDM path
 */
#define AUDIO_IN_INSTANCES_NBR             3U
/**
  * @}
  */
   
/** @defgroup X_NUCLEO_CCA02M1_AUDIO_Exported_Macros X_NUCLEO_CCA02M1_AUDIO Exported Macros
  * @{
  */
#define POS_VAL(VAL)                  (POSITION_VAL(VAL) - 4)
#define VOLUME_IN_CONVERT(Volume)     (((Volume) >= 100)? 239:((uint8_t)(((Volume) * 239) / 100)))
    
/**
  * @}
  */ 
/** @addtogroup X_NUCLEO_CCA02M1_AUDIO_Exported_Variables
  * @{
  */
/* Recording context */
extern AUDIO_IN_Ctx_t                         AudioInCtx[];
/**
  * @}
  */

/** @defgroup X_NUCLEO_CCA02M1_AUDIO_IN_Exported_Functions X_NUCLEO_CCA02M1_AUDIO_IN Exported Functions
  * @{
  */
int32_t BSP_AUDIO_IN_Init(uint32_t Instance, BSP_AUDIO_Init_t* AudioInit);    
int32_t BSP_AUDIO_IN_DeInit(uint32_t Instance);
#if ((USE_HAL_DFSDM_REGISTER_CALLBACKS == 1U) || (USE_HAL_SAI_REGISTER_CALLBACKS == 1U))
int32_t BSP_AUDIO_IN_RegisterDefaultMspCallbacks (uint32_t Instance);
int32_t BSP_AUDIO_IN_RegisterMspCallbacks (uint32_t Instance, BSP_AUDIO_IN_Cb_t *CallBacks);
#endif /* ((USE_HAL_DFSDM_REGISTER_CALLBACKS == 1U) || (USE_HAL_SAI_REGISTER_CALLBACKS == 1U)) */
int32_t BSP_AUDIO_IN_Record(uint32_t Instance, uint8_t* pBuf, uint32_t NbrOfBytes);
int32_t BSP_AUDIO_IN_Stop(uint32_t Instance);
int32_t BSP_AUDIO_IN_Pause(uint32_t Instance);
int32_t BSP_AUDIO_IN_Resume(uint32_t Instance);

int32_t BSP_AUDIO_IN_RecordChannels(uint32_t Instance, uint8_t **pBuf, uint32_t NbrOfBytes);
int32_t BSP_AUDIO_IN_StopChannels(uint32_t Instance, uint32_t Device);
int32_t BSP_AUDIO_IN_PauseChannels(uint32_t Instance, uint32_t Device);
int32_t BSP_AUDIO_IN_ResumeChannels(uint32_t Instance, uint32_t Device);

int32_t BSP_AUDIO_IN_SetDevice(uint32_t Instance, uint32_t Device);
int32_t BSP_AUDIO_IN_GetDevice(uint32_t Instance, uint32_t *Device);
int32_t BSP_AUDIO_IN_SetSampleRate(uint32_t Instance, uint32_t SampleRate);
int32_t BSP_AUDIO_IN_GetSampleRate(uint32_t Instance, uint32_t *SampleRate);                 
int32_t BSP_AUDIO_IN_SetBitsPerSample(uint32_t Instance, uint32_t BitsPerSample);
int32_t BSP_AUDIO_IN_GetBitsPerSample(uint32_t Instance, uint32_t *BitsPerSample);                
int32_t BSP_AUDIO_IN_SetChannelsNbr(uint32_t Instance, uint32_t ChannelNbr);
int32_t BSP_AUDIO_IN_GetChannelsNbr(uint32_t Instance, uint32_t *ChannelNbr);
int32_t BSP_AUDIO_IN_SetVolume(uint32_t Instance, uint32_t Volume);
int32_t BSP_AUDIO_IN_GetVolume(uint32_t Instance, uint32_t *Volume);
int32_t BSP_AUDIO_IN_GetState(uint32_t Instance, uint32_t *State);

/* Specific PDM recodr APIs */
int32_t BSP_AUDIO_IN_PDMToPCM_Init(uint32_t Instance, uint32_t AudioFreq, uint32_t ChnlNbrIn, uint32_t ChnlNbrOut);
int32_t BSP_AUDIO_IN_PDMToPCM(uint32_t Instance, uint16_t *PDMBuf, uint16_t *PCMBuf);
int32_t BSP_AUDIO_IN_RecordPDM(uint32_t Instance, uint8_t* pBuf, uint32_t NbrOfBytes);

void BSP_AUDIO_IN_DMA1_Stream4_IRQHandler(void);
void BSP_AUDIO_IN_DMA1_Stream5_IRQHandler(void);
void BSP_AUDIO_IN_DMA1_Stream6_IRQHandler(void);
void BSP_AUDIO_IN_DMA1_Stream7_IRQHandler(void);

/* User Callbacks: user has to implement these functions in his code if they are needed. */
/* This function should be implemented by the user application.
   It is called into this driver when the current buffer is filled to prepare the next
   buffer pointer and its size. */
void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance);
void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance);

/* This function is called when an Interrupt due to transfer error on or peripheral
   error occurs. */
void BSP_AUDIO_IN_Error_CallBack(uint32_t Instance);

#ifdef USE_STM32L4XX_NUCLEO

/* These function can be modified in case the current settings (e.g. DMA stream)
   need to be changed for specific application needs */
HAL_StatusTypeDef MX_DFSDM1_ClockConfig(DFSDM_Channel_HandleTypeDef *hDfsdmChannel, uint32_t SampleRate);
HAL_StatusTypeDef MX_DFSDM1_Init(DFSDM_Filter_HandleTypeDef *hDfsdmFilter, DFSDM_Channel_HandleTypeDef *hDfsdmChannel, MX_DFSDM_Config *MXConfig);

#else

HAL_StatusTypeDef MX_I2S_ClockConfig(I2S_HandleTypeDef *hi2s, uint32_t PDM_rate);
HAL_StatusTypeDef MX_I2S_Init(I2S_HandleTypeDef* hi2s, MX_I2S_Config *MXConfig);
HAL_StatusTypeDef MX_SPI_Init(SPI_HandleTypeDef* hspi, MX_SPI_Config *MXConfig);

#endif
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* X_NUCLEO_CCA02M1_AUDIO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
