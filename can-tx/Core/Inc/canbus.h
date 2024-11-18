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



extern CAN_HandleTypeDef hcan1;
extern UART_HandleTypeDef huart1;

extern uint32_t TxMailbox;
#define  canID_EXT  0x0408ABCF

void canbusRxInit(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *filtre);
void canbusFilter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *filtre);
void canbusTxInit(CAN_HandleTypeDef *hcan);
void canTxExtMessage(CAN_HandleTypeDef *hcan ,CAN_TxHeaderTypeDef *txHeader, uint32_t canID, uint8_t data[]);
void canTxStdMessage(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *txHeader, uint16_t canID, uint8_t data[]);
void canbusConfigFilter(CAN_HandleTypeDef *hcan,CAN_FilterTypeDef *filtre,uint32_t idType,uint32_t filterMode,uint32_t filterScale,uint32_t filterHigh, uint32_t filterLow,uint32_t maskHigh,uint32_t maskLow
);
#endif /* INC_CANBUS_H_ */
