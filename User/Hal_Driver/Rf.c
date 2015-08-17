/**
  ******************************************************************************
  * @file    Rf.c 
  * @author  chyq
  * @version V1.1.0
  * @date    18-9-2013
  * @brief   Rfº¯Êý.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>
#include "Rf.h"

uint8_t  Rf_Buf[RF_BUF_LEN];

/*********************************************************************
 * @fn      Rf_Init
 *
 * @brief
 *
 *   init.
 *
 * @param   void
 *
 * @return  void
 */
void Rf_Init(void)
{
    cc1100_Init();
}

/*********************************************************************
 * @fn      Rf_SetChannel
 *
 * @brief
 *
 *   Set Channel.
 *
 * @param   channr -
 *
 * @return  void
 */
void Rf_SetChannel(uint8_t channr)
{
    cc1100_WriteReg(CCxxx0_CHANNR, channr);
}

/*********************************************************************
 * @fn      Rf_SetWORmode
 *
 * @brief
 *
 *   Set WOR mode.
 *
 * @param   void
 *
 * @return  void
 */
void Rf_SetWORmode(void)
{
    //Ê¹ÄÜRC
    cc1100_WriteReg(CCxxx0_WORCTRL, 0x78);
    
#ifdef ELE_ADJUST_DEBUG 
    //EVENT0 1S
    cc1100_WriteReg(CCxxx0_WOREVT1, 0x87);
    cc1100_WriteReg(CCxxx0_WOREVT0, 0x6A);
#else
    //EVENT0 0.28S
    cc1100_WriteReg(CCxxx0_WOREVT1, 0x25);
    cc1100_WriteReg(CCxxx0_WOREVT0, 0xEA);
#endif
    
    cc1100_WriteReg(CCxxx0_MCSM2, 0x00);
    cc1100_WriteReg(CCxxx0_MCSM1, 0x3F);
    cc1100_WriteReg(CCxxx0_MCSM0, 0x18);
    
    cc1100_Strobe(CCxxx0_SWORRST);
}

/*********************************************************************
 * @fn      Rf_EnterWORmode
 *
 * @brief
 *
 *   enter WOR mode.
 *
 * @param   void
 *
 * @return  void
 */
void Rf_EnterWORmode(void)
{
    cc1100_Strobe(CCxxx0_SWOR);
}

/*********************************************************************
 * @fn      Rf_EnterIdleMode
 *
 * @brief
 *
 *   enter Idle Mode.
 *
 * @param   void
 *
 * @return  void
 */
void Rf_EnterIdleMode(void)
{
    cc1100_Strobe(CCxxx0_SIDLE);
}

/*********************************************************************
 * @fn      Rf_FlushTxBuf
 *
 * @brief
 *
 *   Flush Tx Buffer.
 *
 * @param   void
 *
 * @return  void
 */
void Rf_FlushTxBuf(void)
{
    cc1100_Strobe(CCxxx0_SFTX);
}

/*********************************************************************
 * @fn      Rf_FlushRxBuf
 *
 * @brief
 *
 *   Flush Rx Buffer.
 *
 * @param   void
 *
 * @return  void
 */
void Rf_FlushRxBuf(void)
{
    cc1100_Strobe(CCxxx0_SFRX);
}

/*********************************************************************
 * @fn      Rf_SendPacket
 *
 * @brief
 *
 *   send a packet.
 *
 * @param   txBuffer -
 *          size -
 *
 * @return  void
 */
void Rf_SendPacket(uint8_t *txBuffer, uint8_t size)
{
    cc1100_SendPacket(txBuffer, size);
}

/*********************************************************************
 * @fn      Rf_ReceivePacket
 *
 * @brief
 *
 *   receive a packet.
 *
 * @param   rxBuffer -
 *          lenth -
 *
 * @return  LQI|CRC, FLASE
 */
uint8_t Rf_ReceivePacket(uint8_t *rxBuffer, uint8_t *length)
{
    return cc1100_ReceivePacket(rxBuffer, length);
}

/*********************************************************************
 * @fn      Rf_SendStart
 *
 * @brief
 *
 *   start to send a packet.
 *
 * @param   rxBuffer - 
 *          lenth - 
 * @return  void
 */
void Rf_SendStart(uint8_t *txBuffer, uint8_t size)
{
    cc1100_WriteBurstReg(CCxxx0_TXFIFO, txBuffer, size);

    cc1100_Strobe(CCxxx0_STX);
}

/*********************************************************************
 * @fn      Rf_SendEnd
 *
 * @brief
 *
 *   End to send a packet.
 *
 * @param   void
 *
 * @return  void
 */
void Rf_SendEnd(void)
{
}

/*********************************************************************
 * @fn      Rf_ReceiveStart
 *
 * @brief
 *
 *   start to Receive a packet.
 *
 * @param   void
 *
 * @return  void
 */
void Rf_ReceiveStart(void)
{
    cc1100_Strobe(CCxxx0_SRX);
}

/*********************************************************************
 * @fn      Rf_ReceiveEnd
 *
 * @brief
 *
 *   End to Receive a packet.
 *
 * @param   rxBuffer - 
 *          lenth - 
 *
 * @return  LQI|CRC, FLASE
 */
uint8_t Rf_ReceiveEnd(uint8_t *rxBuffer, uint8_t *length)
{
    uint8_t status[2];
    uint8_t packetLength;
  
    // This status register is safe to read since it will not be updated after
    // the packet has been received (See the CC1100 and 2500 Errata Note)
    if ((cc1100_ReadStatus(CCxxx0_RXBYTES) & CCxxx0_BYTES_IN_RXFIFO)) 
    {

        // Read length byte
        packetLength = cc1100_ReadReg(CCxxx0_RXFIFO);
    
        // Read data from RX FIFO and store in rxBuffer
        if (packetLength <= *length) 
        {
            cc1100_ReadBurstReg(CCxxx0_RXFIFO, rxBuffer, packetLength); 
            *length = packetLength;
        
            // Read the 2 appended status bytes (status[0] = RSSI, status[1] = LQI)
            cc1100_ReadBurstReg(CCxxx0_RXFIFO, status, 2); 
        
            // MSB of LQI is the CRC_OK bit
            return (status[CCxxx0_LQI_IDX] & CCxxx0_CRC_OK);
        } 
        else 
        {
            *length = packetLength;

            // Make sure that the radio is in IDLE state before flushing the FIFO
            // (Unless RXOFF_MODE has been changed, the radio should be in IDLE state at this point) 
            cc1100_Strobe(CCxxx0_SIDLE);

            // Flush RX FIFO
            cc1100_Strobe(CCxxx0_SFRX);
            return FALSE;
        }
    } 
    else
        return FALSE;
}
