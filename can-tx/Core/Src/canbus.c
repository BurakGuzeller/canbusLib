/*
 * canbus.c
 *
 *  Created on: Nov 15, 2024
 *      Author: burakguzeller
 */

/*
 *  CANBUS ID MASK filreleme :
 *
 *   FilterIdHigh 			= 0x0446;  ---> 0000  0100  0100  0110
 * 	 FilterMaskIdHigh		= 0xFFFF;  ---> 1111  1111  1111  1111
 * 	 -------------------------------------------------------------------
 * 	 filtreden gececek id =                 0000  0100  0100  0110
 * 	 @NOTE : Bunun sebebi high filtre ile high maske arasında sadece '1' lerin baskın olduğu id ler gecmektedir. /
 * 	 		 bir baska ornekte ise maskeleme FFFF disinda olunca.
 *
 * 	 		 maske 1 olunca gececek filtre de aynısı olmalı.
 * 	 		 0 olunca önemi yok 1 de gecebilir 0 da gecebilir.
 *
 * 	 \/\/\/\/\/\/\/\/\/\/\/\/\\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//\/\/\/\/\/\/\/\/\/\/\/\/\/\//\/\/\/\/\/\\/\/
 *
 * 	 FilterIdHigh        = 0x0407;     ---> 0000  0100  0000  0111
 *   FilterMaskIdHigh    = 0x0446;     ---> 0000  0100  0100  0110
 *   ---------------------------------------------------------------------
 *   filtreden gececek id =                 0000  0100  0000  0110
 *   @NOTE: Bu durumda, yalnızca maskede '1' olan bitlerin değerleri dikkate alınır.
 *
 * 	 \/\/\/\/\/\/\/\/\/\/\/\/\\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//\/\/\/\/\/\/\/\/\/\/\/\/\/\//\/\/\/\/\/\\/\/
 *
 * 	 FilterIdHigh        = 0x0450;     ---> 0000  0100  0101  0000
 *   FilterMaskIdHigh    = 0x0FFF;     ---> 0000  1111  1111  1111
 *   ---------------------------------------------------------------------
 *   filtreden gececek id =                 0000  0100  0101  0000
 *  										1111 (gibi seylerde gelebilirdi.)
 *   @NOTE: Maskede '0' olan bitler görmezden gelinir, filtre sadece maskede '1' olan bitlerle eşleşen ID'lere izin verir.
 *
 * 	 \/\/\/\/\/\/\/\/\/\/\/\/\\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//\/\/\/\/\/\/\/\/\/\/\/\/\/\//\/\/\/\/\/\\/\/
 *
 * 	 FilterIdHigh        = 0x07E0;     ---> 0000  0111  1110  0000
 *   FilterMaskIdHigh    = 0x0FF0;     ---> 0000  1111  1111  0000
 *   ---------------------------------------------------------------------
 *   filtreden gececek id =                 0000  0111  1110  0000
 *   @NOTE: Bu örnekte, maskenin 0 olduğu bitlerde herhangi bir değer olabilir.
 */


#include "canbus.h"


/**
 * @brief  CAN bus alımını başlatır.
 * @param  hcan: CAN kontrolörü için işaretçi.
 * @param  filtre: CAN filtreleme yapılandırma yapısı için işaretçi.
 * @retval None
 */
void canbusRxInit(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *filtre) {
    uint8_t debugMessage[60] = { 0 };
    HAL_CAN_Start(hcan);
    canbusNoFilter(hcan, filtre);
    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
    sprintf((char*)debugMessage, "CANBUS IS START SUCCESSFULLY\r\n");
    HAL_UART_Transmit(UARTx, debugMessage, strlen((char*)debugMessage), HAL_MAX_DELAY);
}

/**
 * @brief  CAN filtre yapılandırmasını yapar.
 * @param  hcan: CAN kontrolörü için işaretçi.
 * @param  filtre: CAN filtre yapılandırma yapısı için işaretçi.
 * @retval None
 */
void canbusNoFilter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *filtre) {
	filtre->FilterActivation = CAN_FILTER_ENABLE;
	filtre->FilterBank = 14;
	filtre->FilterFIFOAssignment = CAN_FILTER_FIFO0;
	filtre->FilterIdHigh = 0;
	filtre->FilterIdLow = 0;
	filtre->FilterMaskIdHigh = 0;
	filtre->FilterMaskIdLow = 0;
	filtre->FilterMode = CAN_FILTERMODE_IDMASK;
	filtre->FilterScale = CAN_FILTERSCALE_32BIT;

	if(HAL_CAN_ConfigFilter(hcan, filtre) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief  CAN filtre standart id yapılandırmasını yapar.
 * @param  hcan: CAN kontrolörü için işaretçi.
 * @param  filtre: CAN filtre yapılandırma yapısı için işaretçi.
 * @param  filtreHigh : filtrelenecek High ID
 * @param  filtreLow  : filtrelenecek Low ID
 * @param  maskHigh   : maskelecek High ID' ye gore maske
 * @param  maskLow    : maskelecek Low  ID' ye gore maske
 * @retval None
 */
void canbusStdIDMaskFilter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *filtre, uint32_t filtreHigh, uint32_t maskHigh, uint32_t filtreLow, uint32_t maskLow) {
    filtre->FilterActivation = CAN_FILTER_ENABLE;
    filtre->FilterBank = 14;
    filtre->FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filtre->FilterIdHigh = filtreHigh << 5;
    filtre->FilterIdLow = filtreLow << 5;
    filtre->FilterMaskIdHigh = maskHigh << 5;
    filtre->FilterMaskIdLow = maskLow << 5;
    filtre->FilterMode = CAN_FILTERMODE_IDMASK;
    filtre->FilterScale = CAN_FILTERSCALE_16BIT;   // STD filtreleme olduğu için CAN_FILTERSCALE_16BIT kullanabilirsiniz.

    if (HAL_CAN_ConfigFilter(hcan, filtre) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief  CAN filtre extended id yapılandırmasını yapar.
 * @param  hcan: CAN kontrolörü için işaretçi.
 * @param  filtre: CAN filtre yapılandırma yapısı için işaretçi.
 * @param  filtreHigh : filtrelenecek High ID
 * @param  filtreLow  : filtrelenecek Low ID
 * @param  maskHigh   : maskelecek High ID' ye gore maske
 * @param  maskLow    : maskelecek Low  ID' ye gore maske
 * @retval None
 */
void canbusExtIDMaskFilter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *filtre, uint32_t filtreHigh, uint32_t maskHigh, uint32_t filtreLow, uint32_t maskLow) {
    filtre->FilterActivation = CAN_FILTER_ENABLE;
    filtre->FilterBank = 14;
    filtre->FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filtre->FilterIdHigh = filtreHigh >> 13;
    filtre->FilterIdLow = filtreLow << 3 | CAN_IDE_32;
    filtre->FilterMaskIdHigh = maskHigh >> 13;
    filtre->FilterMaskIdLow = maskLow << 3 | CAN_IDE_32;
    filtre->FilterMode = CAN_FILTERMODE_IDMASK;
    filtre->FilterScale = CAN_FILTERSCALE_32BIT;

    if (HAL_CAN_ConfigFilter(hcan, filtre) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief  CAN filtre ID LİST filtre yapilandirmasidir. En fazla 4 istenilen ID den mesaj yakalanabilir.
 * @param  hcan: CAN kontrolörü için işaretçi.
 * @param  filtre: CAN filtre yapılandırma yapısı için işaretçi.
 * @param  ID1ToBeCaught : ID list yontemi ile yakalanmasini istediginiz ID 1
 * @param  ID1ToBeCaught : ID list yontemi ile yakalanmasini istediginiz ID 2
 * @param  ID1ToBeCaught : ID list yontemi ile yakalanmasini istediginiz ID 3
 * @param  ID1ToBeCaught : ID list yontemi ile yakalanmasini istediginiz ID 4
 *
 * @retval None
 */
void canbusIDListFilter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *filtre, uint32_t ID1ToBeCaught, uint32_t ID2ToBeCaught, uint32_t ID3ToBeCaught,uint32_t ID4ToBeCaught) {
    filtre->FilterActivation 		= CAN_FILTER_ENABLE;
    filtre->FilterBank 				= 14;
    filtre->FilterFIFOAssignment 	= CAN_FILTER_FIFO0;
    filtre->FilterIdHigh 			= ID1ToBeCaught >> 13;
    filtre->FilterIdLow 			= ID2ToBeCaught >> 3 | CAN_IDE_32;
    filtre->FilterMaskIdHigh 		= ID3ToBeCaught >>13;
    filtre->FilterMaskIdLow 		= ID4ToBeCaught >> 3 | CAN_IDE_32;
    filtre->FilterMode 				= CAN_FILTERMODE_IDMASK;
    filtre->FilterScale	 			= CAN_FILTERSCALE_32BIT;

    if (HAL_CAN_ConfigFilter(hcan, filtre) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief  CAN bus gönderimini başlatır.
 * @param  hcan: CAN kontrolörü için işaretçi.
 * @retval None
 */
void canbusTxInit(CAN_HandleTypeDef *hcan) {
    uint8_t debugMessage[60] = { 0 };

    HAL_CAN_Start(hcan);

    sprintf((char*)debugMessage, "CANBUS IS START SUCCESSFULLY\r\n");
    HAL_UART_Transmit(UARTx, debugMessage, strlen((char*)debugMessage), 1000);
}

/**
 * @brief  Uzun ID'li (29 bit) CAN mesajı gönderir.
 * @param  hcan: CAN kontrolörü için işaretçi.
 * @param  txHeader: CAN başlık bilgilerini içeren yapı.
 * @param  canID: CAN mesajının ID'si.
 * @param  data: Gönderilecek CAN veri dizisi.
 * @retval None
 */
void canTxExtMessage(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *txHeader, uint32_t canID, uint8_t data[]) {
    char tempData[128] = { 0 };
    int length;
    uint32_t TxMailbox;

    txHeader->DLC = 8;
    txHeader->ExtId = canID;   // 29 bit uzun ID
    txHeader->IDE = CAN_ID_EXT;
    txHeader->RTR = CAN_RTR_DATA;

    if (HAL_CAN_AddTxMessage(hcan, txHeader, data, &TxMailbox) != HAL_OK) {
        Error_Handler();
    }

    length = snprintf(tempData, sizeof(tempData),
                      "ID: 0x%08lX, Data: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                      canID, data[0], data[1], data[2], data[3],
                      data[4], data[5], data[6], data[7]);

    if (length > 0) {
        HAL_UART_Transmit(UARTx, (uint8_t *)tempData, length,10000);  // DMA kullanımı CPU yükünü azaltır.
    }
}

/**
 * @brief  Standart ID'li (11 bit) CAN mesajı gönderir.
 * @param  hcan: CAN kontrolörü için işaretçi.
 * @param  txHeader: CAN başlık bilgilerini içeren yapı.
 * @param  canID: CAN mesajının ID'si.
 * @param  data: Gönderilecek CAN veri dizisi.
 * @retval None
 */
void canTxStdMessage(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *txHeader, uint16_t canID, uint8_t data[]) {
    char tempData[128] = { 0 };
    int length;
    uint32_t TxMailbox;



    txHeader->DLC = 8;
    txHeader->StdId = canID;
    txHeader->IDE = CAN_ID_STD;        // 11 bit standart ID
    txHeader->RTR = CAN_RTR_DATA;

    if (HAL_CAN_AddTxMessage(hcan, txHeader, data, &TxMailbox) != HAL_OK) {
        Error_Handler();
    }

    length = snprintf(tempData, sizeof(tempData),
                      "ID: 0x%03X, Data: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                      canID, data[0], data[1], data[2], data[3],
                      data[4], data[5], data[6], data[7]);

    if (length > 0) {
        HAL_UART_Transmit(UARTx, (uint8_t *)tempData, length,1000);  // DMA kullanımı CPU yükünü azaltır.
    }
}
