# CANBUS YAPILANDIRMA

## CUBEIDE yapılandırması

* Clock yapılandırmasını yapmalısınız. Max frekans hızında seçtim siz proje gereksinimine göre değişiklik yapabilirisniz.

![image1](/images/Ekran%20Görüntüsü%20-%202024-11-17%2000-21-22.png)

* Debug mesajları ve transmit ve receive mesajlarını dinlemek için uart aktif hale getirlip 115200 baudrate kullandım. Siz Uartı DMA modunda da kullanabilirisnz bu proje temelinde canbus kütüphanesi olduğu için DMA kullanma gereksiniminde bulunmadım. Büyük projelerde CPU yükünü hafifletmek kesinlikle önemlidir. Pin bağlantıları bu şekildedir.

![images2](/images/Ekran%20Görüntüsü%20-%202024-11-17%2000-21-42.png)

* CANBUS baudrate hızını 250000 baudrate seçilmiştir. timestamp ayarlamalrı ve prescaler bu şekilde yapmanız istenilen baudrate için yeterli olacaktır.

> Unutmayınız CANBUS iletim de TX ve RX için aynı baudrate olmalı bu yüzden aynı baudrate seçilmelidir.

![images3](/images/Ekran%20Görüntüsü%20-%202024-11-17%2000-22-00.png)

## Fonksiyon Açıklamaları ve Örnek KUllanımları

* CANBUS Rx modunda başlatmak için ;

```C
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

    canbusExtIDMaskFilter(hcan, filtre, canID_EXT, 0xFFFF, 0, 0);

    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
    sprintf((char*)debugMessage, "CANBUS IS START SUCCESSFULLY\r\n");
    HAL_UART_Transmit(&huart1, debugMessage, strlen((char*)debugMessage), HAL_MAX_DELAY);
}

```

> Örnek kullanım bu şekildedir.

```C

CAN_FilterTypeDef filtre;  // global degisken olarak kullanmaniz tavsiye edilir.
canbusRxInit(&hcan1, &filtre);
```

* CANBUS Tx modunda başlatmak için ;

```C
/**
 * @brief  CAN bus gönderimini başlatır.
 * @param  hcan: CAN kontrolörü için işaretçi.
 * @retval None
 */
void canbusTxInit(CAN_HandleTypeDef *hcan) {
    uint8_t debugMessage[60] = { 0 };

    HAL_CAN_Start(hcan);

    sprintf((char*)debugMessage, "CANBUS IS START SUCCESSFULLY\r\n");
    HAL_UART_Transmit(&huart1, debugMessage, strlen((char*)debugMessage), 1000);
}
```

> Örnek kullanım bu şekildedir.  

```C
canbusTxInit(&hcan1);
```

* CANBUS standart modda mesaj göndermek için

```C
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
        HAL_UART_Transmit(&huart1, (uint8_t *)tempData, length,1000);  // DMA kullanımı CPU yükünü azaltır.
    }
}

```

> Örnek kullanım bu şekildedir.

```C
CAN_TxHeaderTypeDef txHeader;  // global degisken olarak kullanım tavsiye edilir.
uint8_t data[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
canTxStdMessage(&hcan1, &txHeader, 0x7FF, data);
```

* Extended ID şeklinde mesaj gönderimi ;

```C
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
        HAL_UART_Transmit(&huart1, (uint8_t *)tempData, length,10000);  // DMA kullanımı CPU yükünü azaltır.
    }
}

```

> Örnek kullanımı bu şekildeidr.

```C
CAN_TxHeaderTypeDef txHeader;
uint8_t data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
canTxExtMessage(&hcan1, &txHeader, 0x1FBCAF01, data);
```

* Filtreleme icin STANDART ve EXTERNDED mmodda filtrelemek ya da ID list ya da ID MASK yöntemiyle filtrelemek icin toplu şekilde bir fonksiyonda yazdım temiz görünmüm açısından. __UNUTMAYINIZ : IDLİST yönteminde istenilen ID yakalanır sadece IDMASK yönteminde ise filtre aralıgından maskeleneck alana kadar ID ler secilir.__

```C
/**
 * @brief  CAN filtre yapılandırmasını dinamik olarak yapar.
 * @param  hcan: CAN kontrolörü için işaretçi.
 * @param  filtre: CAN filtre yapılandırma yapısı için işaretçi.
 * @param  idType: Filtrelenecek ID tipi (CAN_ID_STD veya CAN_ID_EXT).
 * @param  filterMode: Filtre modu (CAN_FILTERMODE_IDMASK veya CAN_FILTERMODE_IDLIST).
 * @param  filterScale: Filtre ölçeği (CAN_FILTERSCALE_16BIT veya CAN_FILTERSCALE_32BIT).
 * @param  filterHigh: Filtrelenecek ID'nin High kısmı (16-bit veya 32-bit'e göre hesaplanır).
 * @param  filterLow: Filtrelenecek ID'nin Low kısmı (sadece 32-bit modda kullanılır).
 * @param  maskHigh: Maske değerinin High kısmı.
 * @param  maskLow: Maske değerinin Low kısmı.
 * @retval None
 */
void canbusConfigFilter(
    CAN_HandleTypeDef *hcan,
    CAN_FilterTypeDef *filtre,
    uint32_t idType,
    uint32_t filterMode,
    uint32_t filterScale,
    uint32_t filterHigh,
    uint32_t filterLow,
    uint32_t maskHigh,
    uint32_t maskLow
) {
    filtre->FilterActivation = CAN_FILTER_ENABLE;
    filtre->FilterBank = 14;
    filtre->FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filtre->FilterMode = filterMode;
    filtre->FilterScale = filterScale;

    if (filterScale == CAN_FILTERSCALE_16BIT) {
        // 16-bit ID/Maske ayarları
        filtre->FilterIdHigh = (filterHigh << 5) & 0xFFFF;
        filtre->FilterIdLow = (filterLow << 5) & 0xFFFF;
        filtre->FilterMaskIdHigh = (maskHigh << 5) & 0xFFFF;
        filtre->FilterMaskIdLow = (maskLow << 5) & 0xFFFF;
    } else if (filterScale == CAN_FILTERSCALE_32BIT) {
        // 32-bit ID/Maske ayarları
        filtre->FilterIdHigh = (filterHigh >> 13) & 0xFFFF;
        filtre->FilterIdLow = ((filterLow << 3) & 0xFFFF) | (idType == CAN_ID_EXT ? CAN_IDE_32 : 0);
        filtre->FilterMaskIdHigh = (maskHigh >> 13) & 0xFFFF;
        filtre->FilterMaskIdLow = ((maskLow << 3) & 0xFFFF) | (idType == CAN_ID_EXT ? CAN_IDE_32 : 0);
    }

    // Filtreyi CAN kontrolörüne uygula
    if (HAL_CAN_ConfigFilter(hcan, filtre) != HAL_OK) {
        Error_Handler();
    }
}
```

> 1. Örnek kullanım bir ID aralığını filtrelemek için kullanılır. Örneğin, 0x123 ID'sini ve ona yakın değerleri filtrelemek isteyebilirsiniz.

```C
CAN_FilterTypeDef canFilter;
canbusConfigFilter(&hcan, &canFilter, CAN_ID_STD, CAN_FILTERMODE_IDMASK, CAN_FILTERSCALE_16BIT,0x123, 0x000,0x7FF, 0x000);

```
0x123 ID'sine sahip mesajlar kabul edilecektir.

Maske 0x7FF olduğundan, yalnızca ilk 11 bit önemlidir (standart ID).

> 2. Örnek kullanım 32-bit ID'leri filtrelemek için kullanılır. Örneğin, 0x1ABCDE12 ID'sini kabul etmek için.

```C
CAN_FilterTypeDef canFilter;
canbusConfigFilter(&hcan, &canFilter, CAN_ID_EXT, CAN_FILTERMODE_IDMASK,CAN_FILTERSCALE_32BIT, 0x1ABCDE12,0x00000000, 0x1FFFFFFF, 0x00000000);
```
Sadece tam olarak 0x1ABCDE12 ID'sine sahip mesajlar kabul edilir.

Maske 0x1FFFFFFF olduğundan, 29-bit tam ID kontrol edilir.

> 3. Örnek kullanım Belirli birkaç ID'yi filtrelemek istiyorsanız (örneğin 0x123 ve 0x456), liste modunu kullanabilirsiniz.

```C
CAN_FilterTypeDef canFilter;
canbusConfigFilter(
    &hcan,            
    &canFilter,        
    CAN_ID_STD,     
    CAN_FILTERMODE_IDLIST, 
    CAN_FILTERSCALE_16BIT,
    0x123,             // İlk ID
    0x456,             // İkinci ID
    0x000,             // Maske gerekmez (ID List modunda kullanılmaz)
    0x000              // Maske gerekmez (ID List modunda kullanılmaz)
);

```

Yalnızca 0x123 ve 0x456 ID'leri kabul edilir.

Maske kullanılmaz, çünkü ID listesi zaten tam eşleşme gerektirir.

> 4. Örnek kullanım Birden fazla 32-bit ID'yi liste modunda filtrelemek 

```C
CAN_FilterTypeDef canFilter;
canbusConfigFilter(
    &hcan,             
    &canFilter,       
    CAN_ID_EXT,     
    CAN_FILTERMODE_IDLIST,
    CAN_FILTERSCALE_32BIT, 
    0x1ABCDE12,        // İlk ID
    0x1FEDCBA0,        // İkinci ID
    0x00000000,        // Maske gerekmez 
    0x00000000         // Maske gerekmez
);

```

Yalnızca 0x1ABCDE12 ve 0x1FEDCBA0 ID'leri kabul edilir.

Maske kullanılmaz, çünkü liste modu tam eşleşme gerektirir.


* 5. Örnek ,belirli bir ID grubuna (örneğin, 0x100 ile 0x107 arası) ait mesajları kabul etmek istiyorsanız, maske değerini kullanabilirsiniz.

```C
CAN_FilterTypeDef canFilter;
canbusConfigFilter(
    &hcan,        
    &canFilter,      
    CAN_ID_STD,          
    CAN_FILTERMODE_IDMASK, 
    CAN_FILTERSCALE_16BIT,  
    0x100,                  // Filtrelenecek temel ID (High kısmı)
    0x000,                  // Low kısmı (16-bit modda kullanılmaz)
    0x7F8,                  // Maske high (ilk 7 bit önemli)
    0x000                   // Maske low (16-bit modda kullanılmaz)
);

```

> 1. Filtre ID (filterHigh):
 >
 >* 0x100: Temel ID'dir. Filtreleme, bu ID'yi temel alarak yapılır.
> 2. Maske (maskHigh):
>
>* 0x7F8: Bu maske, ilk 7 biti önemli yapar ve son 3 biti "don't care" >olarak bırakır.

Bu durumda, aşağıdaki ID'ler kabul edilir:
0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107.
> 3. Maske Mantığı:
> 
>Maske bitleri 1 olan kısımlar, filtreleme için kontrol edilir.
>
>Maske bitleri 0 olan kısımlar "don't care" olarak kabul edilir ve >herhangi bir değerle eşleşebilir.

* 6. Örnek , Extended ID'ler (29-bit) için benzer bir yapı kullanabilirsiniz. Örneğin, 0x1ABCDE00 ile 0x1ABCDEFF arasındaki ID'leri kabul etmek için ;

```C
CAN_FilterTypeDef canFilter;
canbusConfigFilter(
    &hcan,            
    &canFilter,            
    CAN_ID_EXT,            
    CAN_FILTERMODE_IDMASK, 
    CAN_FILTERSCALE_32BIT,  
    0x1ABCDE00,             // Filtrelenecek temel ID (High kısmı)
    0x00000000,             // Low kısmı
    0x1FFFFF00,             // Maske high (ilk 21 bit önemli)
    0x00000000              // Maske low
);

```

> * Filtre ID (filterHigh ve filterLow):
>
> 0x1ABCDE00: Temel ID'dir. Filtreleme bu ID'ye yakın ID'leri kapsar.

> Maske (maskHigh ve maskLow):
> 0x1FFFFF00: İlk 21 bit kontrol edilir, son 8 bit "don't care" olarak Zbırakılır.

Kabul edilen ID'ler:

>0x1ABCDE00 - 0x1ABCDEFF.


__ÖNEMLİ NOT__

> 1. Maske ve ID Kullanımı:

> Maske değeri ne kadar "dar" ise (daha az 0 varsa), o kadar spesifik filtreleme yapılır.
> Daha fazla 0 içeren bir maske, daha geniş bir ID aralığını kabul eder.


2. Hatalı Değerlerden Kaçınma:
> Maske ve ID değerlerinizin uyumlu olduğundan emin olun.
Örneğin, filterHigh değeri maske tarafından kapsanmayan bir bit içerirse bu bit göz ardı edilir.


* INTERRUPT RX kodu 

```C
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan1)
{
    uint8_t buffer[128];
    uint32_t id;  // Mesaj ID'si
    int length;

    HAL_CAN_GetRxMessage(hcan1, CAN_RX_FIFO0, &rxHeader, rxData);

    if (rxHeader.IDE == CAN_ID_EXT) {
        id = rxHeader.ExtId;
        length = snprintf((char *)buffer, sizeof(buffer),
                           "Ext ID : 0x%08X, Data: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                           id, rxData[0], rxData[1], rxData[2], rxData[3],
                           rxData[4], rxData[5], rxData[6], rxData[7]);
    }
    else
    {
        id = rxHeader.StdId;
        length = snprintf((char *)buffer, sizeof(buffer),
                           "Std ID : 0x%03X, Data: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                           id, rxData[0], rxData[1], rxData[2], rxData[3],
                           rxData[4], rxData[5], rxData[6], rxData[7]);
    }

    if (length > 0) {
        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, length, 10000);
    }

    memset(buffer, 0, sizeof(buffer));
}

```# canbusLib
