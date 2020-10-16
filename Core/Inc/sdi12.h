/**
  ******************************************************************************
  * @file           : sdi12.h
  * @version        : v1.0
  * @brief          : Header for sdi12.c file.
  * @author			: Dave Morton
  * @created		: Jul 4, 2019
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 Mortek.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SDI12_H_
#define SDI12_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32l4xx_hal.h"
/* Private defines -----------------------------------------------------------*/
#define rxBUFFERSIZE 40

#define SDI12_Command_Measure "M0!" // 1) Volumetric Water Content 2) Electrical Conductivity 3) Temperature
#define SDI12_Command_Measure3 "M3!" // 1) Volumetric Water Content 2) Electrical Conductivity 3) Temperature 4) Permittivity 5) Period 6) Voltage Ratio
//#define SDI12_Command_QueryAddress "?!"
#define SDI12_Command_QueryAddress "!"
#define SDI12_Command_GetData "D0!"
#define SDI12_Command_GetData1 "D1!"
#define SDI12_Command_Get_ID "?!" 	   // 1) Device ID
#define SDI12_Command_GetInfo "I!"  // Device Information
/**
 * @brief  Sensor structure definition
 */
typedef struct
{
	UART_HandleTypeDef huart;        		/*!< Specifies the SDI-12 uart handle */

	GPIO_TypeDef* DIR_GPIO_Port;     /*!< Specifies the SDI-12 direction pin port */

	uint32_t DIR_Pin;          	/*!< Specifies the SDI-12 direction pin */


} SDI12_TypeDef;

typedef enum
{
    SDI12_ERROR_INVALID = -1,        // -1
	SDI12_ERROR_SUCCESS = 0,         // 0
	SDI12_ERROR_TIMEOUT,             // 1
	SDI12_ERROR_UNEXPECTED_PARAM,    // 2
	SDI12_ERROR_UNEXPECTED_RESPONSE, // 3
	SDI12_ERROR_NO_RESPONSE         // 4
} SDI12_error_t;

/* Exported functions -------------------------------------------------------*/
int SDI12_Send_Command(uint8_t *data, uint8_t *rxbuffer, uint32_t timeout);
void SDI12_Init(UART_HandleTypeDef huart);

SDI12_error_t SDI12_GetInfo(char address, char *resp);
SDI12_error_t SDI12_QueryAddress(char address, char *resp);
SDI12_error_t SDI12_GetData(char address, int32_t *vwc, int32_t *ec, int32_t *temp);
#endif /* SDI12_H _ */
/************************ (C) COPYRIGHT Mortek *****END OF FILE****/
