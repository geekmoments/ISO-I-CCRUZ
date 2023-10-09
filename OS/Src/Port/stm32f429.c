/*
 * stm32f429.c
 *
 *  Created on: Oct 8, 2023
 *      Author: cesarcruz
 */
#ifdef STM32F429

#include "stm32f429.h"
#include "core_cm4.h"

osIRQVector irqVector[IRQ_NUMBER] = { 0 };

void WWDG_IRQHandler(void)                  {osIRQHandler(WWDG_IRQn);}                  /* Window WatchDog                             */
void PVD_IRQHandler(void)                   {osIRQHandler(PVD_IRQn);}                   /* PVD through EXTI Line detection             */
void TAMP_STAMP_IRQHandler(void)            {osIRQHandler(TAMP_STAMP_IRQn);}            /* Tamper and TimeStamps through the EXTI line */
void RTC_WKUP_IRQHandler(void)              {osIRQHandler(RTC_WKUP_IRQn);}              /* RTC Wakeup through the EXTI line            */
void FLASH_IRQHandler(void)                 {osIRQHandler(FLASH_IRQn);}                 /* FLASH                                       */
void RCC_IRQHandler(void)                   {osIRQHandler(RCC_IRQn);}                   /* RCC                                         */
void EXTI0_IRQHandler(void)                 {osIRQHandler(EXTI0_IRQn);}                 /* EXTI Line0                                  */
void EXTI1_IRQHandler(void)                 {osIRQHandler(EXTI1_IRQn);}                 /* EXTI Line1                                  */
void EXTI2_IRQHandler(void)                 {osIRQHandler(EXTI2_IRQn);}                 /* EXTI Line2                                  */
void EXTI3_IRQHandler(void)                 {osIRQHandler(EXTI3_IRQn);}                 /* EXTI Line3                                  */
void EXTI4_IRQHandler(void)                 {osIRQHandler(EXTI4_IRQn);}                 /* EXTI Line4                                  */
void DMA1_Stream0_IRQHandler(void)          {osIRQHandler(DMA1_Stream0_IRQn);}          /* DMA1 Stream 0                               */
void DMA1_Stream1_IRQHandler(void)          {osIRQHandler(DMA1_Stream1_IRQn);}          /* DMA1 Stream 1                               */
void DMA1_Stream2_IRQHandler(void)          {osIRQHandler(DMA1_Stream2_IRQn);}          /* DMA1 Stream 2                               */
void DMA1_Stream3_IRQHandler(void)          {osIRQHandler(DMA1_Stream3_IRQn);}          /* DMA1 Stream 3                               */
void DMA1_Stream4_IRQHandler(void)          {osIRQHandler(DMA1_Stream4_IRQn);}          /* DMA1 Stream 4                               */
void DMA1_Stream5_IRQHandler(void)          {osIRQHandler(DMA1_Stream5_IRQn);}          /* DMA1 Stream 5                               */
void DMA1_Stream6_IRQHandler(void)          {osIRQHandler(DMA1_Stream6_IRQn);}          /* DMA1 Stream 6                               */
void ADC_IRQHandler(void)                   {osIRQHandler(ADC_IRQn);}                   /* ADC1, ADC2 and ADC3s                        */
void CAN1_TX_IRQHandler(void)               {osIRQHandler(CAN1_TX_IRQn);}               /* CAN1 TX                                     */
void CAN1_RX0_IRQHandler(void)              {osIRQHandler(CAN1_RX0_IRQn);}              /* CAN1 RX0                                    */
void CAN1_RX1_IRQHandler(void)              {osIRQHandler(CAN1_RX1_IRQn);}              /* CAN1 RX1                                    */
void CAN1_SCE_IRQHandler(void)              {osIRQHandler(CAN1_SCE_IRQn);}              /* CAN1 SCE                                    */
void EXTI9_5_IRQHandler(void)               {osIRQHandler(EXTI9_5_IRQn);}               /* External Line[9:5]s                         */
void TIM1_BRK_TIM9_IRQHandler(void)         {osIRQHandler(TIM1_BRK_TIM9_IRQn);}         /* TIM1 Break and TIM9                         */
//void TIM1_UP_TIM10_IRQHandler(void)         {osIRQHandler(TIM1_UP_TIM10_IRQn);}         /* TIM1 Update and TIM10                       */
void TIM1_TRG_COM_TIM11_IRQHandler(void)    {osIRQHandler(TIM1_TRG_COM_TIM11_IRQn);}    /* TIM1 Trigger and Commutation and TIM11      */
void TIM1_CC_IRQHandler(void)               {osIRQHandler(TIM1_CC_IRQn);}               /* TIM1 Capture Compare                        */
void TIM2_IRQHandler(void)                  {osIRQHandler(TIM2_IRQn);}                  /* TIM2                                        */
void TIM3_IRQHandler(void)                  {osIRQHandler(TIM3_IRQn);}                  /* TIM3                                        */
void TIM4_IRQHandler(void)                  {osIRQHandler(TIM4_IRQn);}                  /* TIM4                                        */
void I2C1_EV_IRQHandler(void)               {osIRQHandler(I2C1_EV_IRQn);}               /* I2C1 Event                                  */
void I2C1_ER_IRQHandler(void)               {osIRQHandler(I2C1_ER_IRQn);}               /* I2C1 Error                                  */
void I2C2_EV_IRQHandler(void)               {osIRQHandler(I2C2_EV_IRQn);}               /* I2C2 Event                                  */
void I2C2_ER_IRQHandler(void)               {osIRQHandler(I2C2_ER_IRQn);}               /* I2C2 Error                                  */
void SPI1_IRQHandler(void)                  {osIRQHandler(SPI1_IRQn);}                  /* SPI1                                        */
void SPI2_IRQHandler(void)                  {osIRQHandler(SPI2_IRQn);}                  /* SPI2                                        */
void USART1_IRQHandler(void)                {osIRQHandler(USART1_IRQn);}                /* USART1                                      */
void USART2_IRQHandler(void)                {osIRQHandler(USART2_IRQn);}                /* USART2                                      */
void USART3_IRQHandler(void)                {osIRQHandler(USART3_IRQn);}                /* USART3                                      */
void EXTI15_10_IRQHandler(void)             {osIRQHandler(EXTI15_10_IRQn);}             /* External Line[15:10]s                       */
void RTC_Alarm_IRQHandler(void)             {osIRQHandler(RTC_Alarm_IRQn);}             /* RTC Alarm (A and B) through EXTI Line       */
void OTG_FS_WKUP_IRQHandler(void)           {osIRQHandler(OTG_FS_WKUP_IRQn);}           /* USB OTG FS Wakeup through EXTI line         */
void TIM8_BRK_TIM12_IRQHandler(void)        {osIRQHandler(TIM8_BRK_TIM12_IRQn);}        /* TIM8 Break and TIM12                        */
void TIM8_UP_TIM13_IRQHandler(void)         {osIRQHandler(TIM8_UP_TIM13_IRQn);}         /* TIM8 Update and TIM13                       */
void TIM8_TRG_COM_TIM14_IRQHandler(void)    {osIRQHandler(TIM8_TRG_COM_TIM14_IRQn);}    /* TIM8 Trigger and Commutation and TIM14      */
void TIM8_CC_IRQHandler(void)               {osIRQHandler(TIM8_CC_IRQn);}               /* TIM8 Capture Compare                        */
void DMA1_Stream7_IRQHandler(void)          {osIRQHandler(DMA1_Stream7_IRQn);}          /* DMA1 Stream7                                */
void FMC_IRQHandler(void)                   {osIRQHandler(FMC_IRQn);}                   /* FMC                                         */
void SDIO_IRQHandler(void)                  {osIRQHandler(SDIO_IRQn);}                  /* SDIO                                        */
void TIM5_IRQHandler(void)                  {osIRQHandler(TIM5_IRQn);}                  /* TIM5                                        */
void SPI3_IRQHandler(void)                  {osIRQHandler(SPI3_IRQn);}                  /* SPI3                                        */
void UART4_IRQHandler(void)                 {osIRQHandler(UART4_IRQn);}                 /* UART4                                       */
void UART5_IRQHandler(void)                 {osIRQHandler(UART5_IRQn);}                 /* UART5                                       */
void TIM6_DAC_IRQHandler(void)              {osIRQHandler(TIM6_DAC_IRQn);}              /* TIM6 and DAC1&2 underrun errors             */
void TIM7_IRQHandler(void)                  {osIRQHandler(TIM7_IRQn);}                  /* TIM7                                        */
void DMA2_Stream0_IRQHandler(void)          {osIRQHandler(DMA2_Stream0_IRQn);}          /* DMA2 Stream 0                               */
void DMA2_Stream1_IRQHandler(void)          {osIRQHandler(DMA2_Stream1_IRQn);}          /* DMA2 Stream 1                               */
void DMA2_Stream2_IRQHandler(void)          {osIRQHandler(DMA2_Stream2_IRQn);}          /* DMA2 Stream 2                               */
void DMA2_Stream3_IRQHandler(void)          {osIRQHandler(DMA2_Stream3_IRQn);}          /* DMA2 Stream 3                               */
void DMA2_Stream4_IRQHandler(void)          {osIRQHandler(DMA2_Stream4_IRQn);}          /* DMA2 Stream 4                               */
void ETH_IRQHandler(void)                   {osIRQHandler(ETH_IRQn);}                   /* Ethernet                                    */
void ETH_WKUP_IRQHandler(void)              {osIRQHandler(ETH_WKUP_IRQn);}              /* Ethernet Wakeup through EXTI line           */
void CAN2_TX_IRQHandler(void)               {osIRQHandler(CAN2_TX_IRQn);}               /* CAN2 TX                                     */
void CAN2_RX0_IRQHandler(void)              {osIRQHandler(CAN2_RX0_IRQn);}              /* CAN2 RX0                                    */
void CAN2_RX1_IRQHandler(void)              {osIRQHandler(CAN2_RX1_IRQn);}              /* CAN2 RX1                                    */
void CAN2_SCE_IRQHandler(void)              {osIRQHandler(CAN2_SCE_IRQn);}              /* CAN2 SCE                                    */
void OTG_FS_IRQHandler(void)                {osIRQHandler(OTG_FS_IRQn);}                /* USB OTG FS                                  */
void DMA2_Stream5_IRQHandler(void)          {osIRQHandler(DMA2_Stream5_IRQn);}          /* DMA2 Stream 5                               */
void DMA2_Stream6_IRQHandler(void)          {osIRQHandler(DMA2_Stream6_IRQn);}          /* DMA2 Stream 6                               */
void DMA2_Stream7_IRQHandler(void)          {osIRQHandler(DMA2_Stream7_IRQn);}          /* DMA2 Stream 7                               */
void USART6_IRQHandler(void)                {osIRQHandler(USART6_IRQn);}                /* USART6                                      */
void I2C3_EV_IRQHandler(void)               {osIRQHandler(I2C3_EV_IRQn);}               /* I2C3 event                                  */
void I2C3_ER_IRQHandler(void)               {osIRQHandler(I2C3_ER_IRQn);}               /* I2C3 error                                  */
void OTG_HS_EP1_OUT_IRQHandler(void)        {osIRQHandler(OTG_HS_EP1_OUT_IRQn);}        /* USB OTG HS End Point 1 Out                  */
void OTG_HS_EP1_IN_IRQHandler(void)         {osIRQHandler(OTG_HS_EP1_IN_IRQn);}         /* USB OTG HS End Point 1 In                   */
void OTG_HS_WKUP_IRQHandler(void)           {osIRQHandler(OTG_HS_WKUP_IRQn);}           /* USB OTG HS Wakeup through EXTI              */
void OTG_HS_IRQHandler(void)                {osIRQHandler(OTG_HS_IRQn);}                /* USB OTG HS                                  */
void DCMI_IRQHandler(void)                  {osIRQHandler(DCMI_IRQn);}                  /* DCMI                                        */
void HASH_RNG_IRQHandler(void)              {osIRQHandler(HASH_RNG_IRQn);}              /* Hash and Rng                                */
void FPU_IRQHandler(void)                   {osIRQHandler(FPU_IRQn);}                   /* FPU                                         */
void UART7_IRQHandler(void)                 {osIRQHandler(UART7_IRQn);}                 /* UART7                                       */
void UART8_IRQHandler(void)                 {osIRQHandler(UART8_IRQn);}                 /* UART8                                       */
void SPI4_IRQHandler(void)                  {osIRQHandler(SPI4_IRQn);}                  /* SPI4                                        */
void SPI5_IRQHandler(void)                  {osIRQHandler(SPI5_IRQn);}                  /* SPI5 						               */
void SPI6_IRQHandler(void)                  {osIRQHandler(SPI6_IRQn);}                  /* SPI6						                   */
void SAI1_IRQHandler(void)                  {osIRQHandler(SAI1_IRQn);}                  /* SAI1						                   */
void LTDC_IRQHandler(void)                  {osIRQHandler(LTDC_IRQn);}                  /* LTDC_IRQHandler			                   */
void LTDC_ER_IRQHandler(void)               {osIRQHandler(LTDC_ER_IRQn);}               /* LTDC_ER_IRQHandler			               */
void DMA2D_IRQHandler(void)                 {osIRQHandler(DMA2D_IRQn);}                 /* DMA2D                                       */

void osIRQHandler(osIRQnType irqType)
{

    void(*irqH)(void*);

	OsStatus prevStatus;
    prevStatus = osGetStatus();

    osSetStatus(OS_STATUS_IRQ);

    irqH = irqVector[irqType].handler;

    if(irqH != NULL)
    {
    	void *data = irqVector[irqType].data;

    	irqH(data);
    }

    osSetStatus(prevStatus);

    NVIC_ClearPendingIRQ(irqType);

}

#endif // STM32F429



