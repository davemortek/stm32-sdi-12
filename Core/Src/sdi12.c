/**
  ******************************************************************************
  * @file           : sdi12.c
  * @version        : v1.0
  * @brief          : SD disk file operations
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

/* Includes ------------------------------------------------------------------*/
#include "sdi12.h"

/* Private variables ---------------------------------------------------------*/
SDI12_TypeDef sdi12;

uint8_t SDI12_rxBufffer[rxBUFFERSIZE];

static char *sdi12_calloc_char(size_t num);
static SDI12_error_t getData(char address, char *resp);
static uint32_t measure(char address);
static SDI12_error_t sendCommandWithResponse(char *command, char *responseDest);

void SDI12_Init(UART_HandleTypeDef huart)
{
	sdi12.huart = huart;        		/*!< Specifies the SDI-12 uart handle */
}

static char *sdi12_calloc_char(size_t num)
{
    return (char *)calloc(num, sizeof(char));
}

SDI12_error_t SDI12_GetInfo(char address, char *resp)
{
    SDI12_error_t err = 0;
    char *command;
    char *response;

    command = sdi12_calloc_char(strlen(SDI12_Command_GetInfo) + 1);
    if (command == NULL)
        return -1;
    sprintf(command, "%c%s", address, SDI12_Command_GetInfo);

    response = sdi12_calloc_char(100); // Changed from 24 to 100 because memory overloads in sendCommandWithResponse

    if (response == NULL)
    {
        free(command);
        return -1;
    }

    err = sendCommandWithResponse(command, response);
    strcpy(resp,"\0"); /* Clear char pointer */
    strcat(resp,response);

    if (err != SDI12_ERROR_SUCCESS)
    {
        free(command);
        free(response);
        return -1;
    }

    free(command);
    free(response);

    return err;
}


SDI12_error_t SDI12_QueryAddress(char address, char *resp)
{
    SDI12_error_t err = 0;
    char *command;
    char *response;

    command = sdi12_calloc_char(strlen(SDI12_Command_QueryAddress) + 1);
    if (command == NULL)
        return -1;
    sprintf(command, "%c%s", address, SDI12_Command_QueryAddress);

    response = sdi12_calloc_char(100); // Changed from 24 to 100 because memory overloads in sendCommandWithResponse

    if (response == NULL)
    {
        free(command);
        return -1;
    }

    err = sendCommandWithResponse(command, response);
    strcpy(resp,"\0"); /* Clear char pointer */
    strcat(resp,response);

    if (err != SDI12_ERROR_SUCCESS)
    {
        free(command);
        free(response);
        return -1;
    }

    free(command);
    free(response);

    return err;
}



SDI12_error_t SDI12_GetData(char address, int32_t *vwc, int32_t *ec, int32_t *temp)
{
    SDI12_error_t err = 0;
    char *response;
    float a,b,c;

    HAL_Delay(measure(address)); // Take sensor measurement and wait for data to be ready

    response = sdi12_calloc_char(100); // Changed from 24 to 100 because memory overloads in sendCommandWithResponse

    getData(address, response);

    sscanf(response, "1%f%f%f",a,b,c);

    vwc = (int32_t)(a*1000);
    ec = (int32_t)(b*1000);
    temp = (int32_t)(c*1000);
    return err;
}


static SDI12_error_t getData(char address, char *resp)
{
    SDI12_error_t err = 0;
    char *command;
    char *response;

    command = sdi12_calloc_char(strlen(SDI12_Command_GetData) + 1);
    if (command == NULL)
        return -1;
    sprintf(command, "%c%s", address, SDI12_Command_GetData);

    response = sdi12_calloc_char(100); // Changed from 24 to 100 because memory overloads in sendCommandWithResponse

    if (response == NULL)
    {
        free(command);
        return -1;
    }

    err = sendCommandWithResponse(command, response);
    strcpy(resp,"\0"); /* Clear char pointer */
    strcat(resp,response); /* Append data with CR LF */

    command = sdi12_calloc_char(strlen(SDI12_Command_GetData1) + 1);
    if (command == NULL)
        return -1;
    sprintf(command, "%c%s", address, SDI12_Command_GetData1);

    response = sdi12_calloc_char(100); // Changed from 24 to 100 because memory overloads in sendCommandWithResponse

    if (response == NULL)
    {
        free(command);
        return -1;
    }

//  err = sendCommandWithResponse(command, response);
//  strcat(resp,response); /* Append data with CR LF */

    if (err != SDI12_ERROR_SUCCESS)
    {
        free(command);
        free(response);
        return -1;
    }

    free(command);
    free(response);

    return err;
}


static uint32_t measure(char address)
{
    uint32_t dataAvailable = 0; // Time until measurement data available (ms)
    char *command;
    char *response;

    command = sdi12_calloc_char(strlen(SDI12_Command_Measure) + 1);
    if (command == NULL)
        return -1;
    sprintf(command, "%c%s", address, SDI12_Command_Measure);

    response = sdi12_calloc_char(100); // Changed from 24 to 100 because memory overloads in sendCommandWithResponse

    if (response == NULL)
    {
        free(command);
        return -1;
    }

    sendCommandWithResponse(command, response);
    dataAvailable = (response[1] - 48) * 100000 +
    		        (response[2] - 48) * 10000 +
					(response[3] - 48) * 1000;

    printf("TIME - %lu\r\n",dataAvailable);

//    if (err != SDI12_ERROR_SUCCESS)
//    {
//        free(command);
//        free(response);
//        return -1;
//    }

    free(command);
    free(response);

    return dataAvailable;
}

/**
  * @brief  Sends SDI-12 command
  * @param  command
  * @retval SDI12_OK
  */
static SDI12_error_t sendCommandWithResponse(char *command, char *responseDest)
{
    uint8_t command_len = strlen(command);

    if(command != NULL)
    {
        printf("Command: %s\r\n",command);
    }
    else
    {
    	printf("Command: NULL\r\n");
    }

    // Set TX Pin to Output
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Send break
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
    HAL_Delay(12);

    // Set marking
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_Delay(7);

    // Transmit Data
    uint8_t err = HAL_UART_Transmit(&sdi12.huart, (uint8_t *)command, command_len,1000);
    if(err != HAL_OK)
    {
   	 printf("TX ERROR - %d\r\n",err);
   	 return HAL_ERROR;
    }

    sdi12.huart.AdvancedInit.Swap = UART_ADVFEATURE_SWAP_ENABLE;
    err = HAL_UART_Init(&sdi12.huart);
    if(err != HAL_OK)
    {
   	printf("INIT ERROR - %d\r\n",err);
   	return HAL_ERROR;
    }


    //Set uart to recieve mode
    err = HAL_UART_Receive(&sdi12.huart, (uint8_t *)responseDest, 40,1000);
    if(err != HAL_TIMEOUT)printf("RX ERROR - %d\r\n",err);

    printf("<< DATA - %s\r\n",responseDest);

    sdi12.huart.AdvancedInit.Swap = UART_ADVFEATURE_SWAP_DISABLE;
    err = HAL_UART_Init(&sdi12.huart);
    if(err != HAL_OK)
    {
   	printf("INIT 2 ERROR - %d\r\n",err);
   	return HAL_ERROR;
    }

    return SDI12_ERROR_SUCCESS;
}


// SDI-12 Pin Swap Method

int SDI12_Send_Command(uint8_t *data,uint8_t *rxbuffer, uint32_t timeout)
{

 for(uint8_t i=0;i<40;i++)
 {
	 SDI12_rxBufffer[i] = 'A';
 }

 uint8_t len = strlen((char *)data);


 GPIO_InitTypeDef GPIO_InitStruct = {0};

 GPIO_InitStruct.Pin = GPIO_PIN_9;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

 // Send break
 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
 HAL_Delay(12);

 // Set marking
 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
 GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
 GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
 HAL_Delay(7);

 // Transmit Data
 uint8_t err = HAL_UART_Transmit(&sdi12.huart, data, len,1000);
 if(err != HAL_OK)
 {
	 printf("TX ERROR - %d\r\n",err);
	 return HAL_ERROR;
 }

 sdi12.huart.AdvancedInit.Swap = UART_ADVFEATURE_SWAP_ENABLE;
 err = HAL_UART_Init(&sdi12.huart);
 if(err != HAL_OK)
 {
	printf("INIT ERROR - %d\r\n",err);
	return HAL_ERROR;
 }


 //Set uart to recieve mode
 err = HAL_UART_Receive(&sdi12.huart, (uint8_t *)SDI12_rxBufffer, 50,1000);
 printf("RX ERROR - %d\r\n",err);

// if(err != HAL_OK)
// {
//	 return HAL_ERROR;
// }


// << SENSOR DATA: 1+0-.0+20.9024

 printf("<< DATA - \r\n");

 //Read data buffer
 uint8_t i = 0;
 while(SDI12_rxBufffer[i] != '\0')
 {
	 printf("<< SDI-12: %c , %c\r\n",(char)SDI12_rxBufffer[i],(char)(SDI12_rxBufffer[i]&0x7F)); // &0x7F to remove bit error in bitmask 10000000

//	 if(i==len-1){
//		 if(memcmp(data,SDI12_rxBufffer,len))
//		 {
//			 printf("<< SDI-12: Commands Match\r\n");
//		 }
//		 else
//		 {
//			 return HAL_ERROR;
//		 }
//	 }
//	 if(i>len-1)
//	 {
//		 rxbuffer[i-len] = SDI12_rxBufffer[i]&0x7F; // &0x7F to remove bit error in bitmask 10000000
//	 }
	 rxbuffer[i] = SDI12_rxBufffer[i]&0x7F; // &0x7F to remove bit error in bitmask 10000000
	 i++;
	 if(i>40)break;
 }
// err = HAL_UART_DeInit(&sdi12.huart);
// if(err != HAL_OK)
// {
//	printf("DEINIT ERROR - %d\r\n",err);
//	return HAL_ERROR;
// }

 sdi12.huart.AdvancedInit.Swap = UART_ADVFEATURE_SWAP_DISABLE;
 err = HAL_UART_Init(&sdi12.huart);
 if(err != HAL_OK)
 {
	printf("INIT 2 ERROR - %d\r\n",err);
	return HAL_ERROR;
 }
 //end command function
 return HAL_OK;
}

// SDI-12 Single Wire Method

//int SDI12_Send_Command(uint8_t *data,uint8_t *rxbuffer, uint32_t timeout)
//{
//
// for(uint8_t i=0;i<40;i++)
// {
//	 SDI12_rxBufffer[i] = '\0';
// }
//
// uint8_t len = strlen((char *)data);
// uint8_t err= 0;
//
////  sdi12.huart.AdvancedInit.TxPinLevelInvert = UART_ADVFEATURE_TXINV_ENABLE;
////  if (HAL_UART_Init(&sdi12.huart) != HAL_OK)
////  {
//// 	return HAL_ERROR;
////  }
// //Send break
// GPIO_InitTypeDef GPIO_InitStruct = {0};
//
// GPIO_InitStruct.Pin = GPIO_PIN_9;
// GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
// GPIO_InitStruct.Pull = GPIO_NOPULL;
// GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
// HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
//// if(HAL_LIN_SendBreak(&sdi12.huart) != HAL_OK) return HAL_ERROR;
//// HAL_UART_DeInit(&sdi12.huart);
// HAL_Delay(12);
// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
//
//// sdi12.huart.AdvancedInit.TxPinLevelInvert = UART_ADVFEATURE_TXINV_DISABLE;
//// HAL_UART_Init(&sdi12.huart);
//// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
//
//
//// sdi12.huart.AdvancedInit.TxPinLevelInvert = UART_ADVFEATURE_TXINV_ENABLE;
//// HAL_UART_Init(&sdi12.huart);
// //Transmit uart data
// GPIO_InitStruct.Pin = GPIO_PIN_9;
// GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
// GPIO_InitStruct.Pull = GPIO_PULLUP;
// GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
// GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
// HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
// err = HAL_HalfDuplex_EnableTransmitter(&sdi12.huart);
// if(err != HAL_OK)printf("EN TX ERROR - %d\r\n",err);
// //Set marking
// HAL_Delay(7);
// err = HAL_UART_Transmit(&sdi12.huart, data, len,5000);
// if(err != HAL_OK)printf("TX ERROR - %d\r\n",err);
//
// GPIO_InitStruct.Pin = GPIO_PIN_9;
// GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
// GPIO_InitStruct.Pull = GPIO_NOPULL;
// GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
// GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
//
// HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
// sdi12.huart.Init.WordLength = UART_WORDLENGTH_7B;
//
//// err = HAL_HalfDuplex_Init(&sdi12.huart);
//// if(err != HAL_OK)printf("HALF INIT ERROR - %d\r\n",err);
//
// err = HAL_HalfDuplex_EnableReceiver(&sdi12.huart);
// if(err != HAL_OK)printf("EN RX ERROR - %d\r\n",err);
// //Set uart to recieve mode
// err = HAL_UART_Receive(&sdi12.huart, (uint8_t *)SDI12_rxBufffer, 50,1000);
// printf("RX ERROR - %d\r\n",err);
//
//
//// << SENSOR DATA: 1+0-.0+20.9024
//
// printf("<< DATA: - \r\n");
//
//
// //Wait for data to arrive
// //HAL_Delay(1000);
//
// //Read data buffer
// uint8_t i = 0;
// while(SDI12_rxBufffer[i] != '\0')
// {
//	 printf("<< SDI-12: %c , %c\r\n",(char)SDI12_rxBufffer[i],(char)(SDI12_rxBufffer[i]&0x7F)); // &0x7F to remove bit error in bitmask 10000000
//
////	 if(i==len-1){
////		 if(memcmp(data,SDI12_rxBufffer,len))
////		 {
////			 printf("<< SDI-12: Commands Match\r\n");
////		 }
////		 else
////		 {
////			 return HAL_ERROR;
////		 }
////	 }
////	 if(i>len-1)
////	 {
////		 rxbuffer[i-len] = SDI12_rxBufffer[i]&0x7F; // &0x7F to remove bit error in bitmask 10000000
////	 }
//	 rxbuffer[i] = SDI12_rxBufffer[i]&0x7F; // &0x7F to remove bit error in bitmask 10000000
//	 i++;
//	 if(i>40)break;
// }
//// err = HAL_UART_DeInit(&sdi12.huart);
//// if(err != HAL_OK)
//// {
////	printf("DEINIT ERROR - %d\r\n",err);
////	return HAL_ERROR;
//// }
//
//
// //end command function
// return HAL_OK;
//}

/************************ (C) COPYRIGHT Mortek *****END OF FILE****/
