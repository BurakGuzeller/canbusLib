/*
 * canbus.h
 *
 *  Created on: Nov 15, 2024
 *      Author: burakguzeller
 */

#ifndef INC_CANBUS_H_
#define INC_CANBUS_H_

#include "main.h"
#include <stdio.h>
#include <string.h>

#define CAN_IDE_32  0b00000100
#define UARTx   &huart2



extern CAN_HandleTypeDef hcan1;
extern UART_HandleTypeDef huart2;

extern uint32_t TxMailbox;

void canbusRxInit(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *filtre);
void canbusFilter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *filtre);
void canbusTxInit(CAN_HandleTypeDef *hcan);
void canTxExtMessage(CAN_HandleTypeDef *hcan ,CAN_TxHeaderTypeDef *txHeader, uint32_t canID, uint8_t data[]);
void canTxStdMessage(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *txHeader, uint16_t canID, uint8_t data[]);
void canbusIDListFilter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *filtre, uint32_t ID1ToBeCaught, uint32_t ID2ToBeCaught, uint32_t ID3ToBeCaught,uint32_t ID4ToBeCaught);
void canbusStdMaskFilter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *filtre, uint32_t filtreHigh, uint32_t maskHigh, uint32_t filtreLow, uint32_t maskLow);
void canbusExtIDMaskFilter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *filtre, uint32_t filtreHigh, uint32_t maskHigh, uint32_t filtreLow, uint32_t maskLow);
void canbusNoFilter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *filtre);

#endif /* INC_CANBUS_H_ */
