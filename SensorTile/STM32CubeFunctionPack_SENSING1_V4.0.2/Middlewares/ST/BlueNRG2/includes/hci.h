/**
  ******************************************************************************
  * @file    hci.h
  * @author  AMS - VMA RF Application team
  * @version V1.0.0
  * @date    21-Sept-2016
  * @brief   Header file for BlueNRG-1's HCI APIs
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  ******************************************************************************
  */
#ifndef __HCI_H_
#define __HCI_H_

#include "bluenrg1_types.h"

/** 
 * @addtogroup HIGH_LEVEL_INTERFACE HIGH_LEVEL_INTERFACE
 * @{
 */

/**
 * @defgroup HCI_TL HCI_TL
 * @{
 */

/** 
 * @defgroup HCI_TL_functions HCI_TL functions
 * @{
 */

/**
 * @brief Initialize the Host Controller Interface. 
 *        This function must be called before any data can be received 
 *        from BLE controller.
 *
 * @param  UserEvtRx: ACI events callback function pointer
 *         This callback is triggered when an user event is received from 
 *         the BLE core device. 
 * @param  pConf: Configuration structure pointer
 * @retval None
 */
void hci_init(void(* UserEvtRx)(void* pData), void* pConf);

/**
 * @brief  Processing function that must be called after an event is received from
 *         HCI interface. 
 *         It must be called outside ISR. It will call user_notify() if necessary.
 *
 * @param  None
 * @retval None
 */ 
void hci_user_evt_proc(void);

/**
 * @}
 */

/**
 * @}
 */ 

/**
 * @}
 */  
#endif /* __HCI_H_ */
