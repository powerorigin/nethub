/**
  ******************************************************************************
  * @file    cc1100.c 
  * @author  chyq
  * @version V1.1.0
  * @date    18-9-2013
  * @brief   cc1100º¯Êý.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "cc1100.h"

#define  CC1100_SET_CS()   GPIO_SetBits(CCxxx0_CS_PORT, CCxxx0_CS_PIN)
#define  CC1100_CLR_CS()   GPIO_ResetBits(CCxxx0_CS_PORT, CCxxx0_CS_PIN)
#define  CC1100_SET_SCK()  GPIO_SetBits(CCxxx0_SCK_PORT, CCxxx0_SCK_PIN)
#define  CC1100_CLR_SCK()  GPIO_ResetBits(CCxxx0_SCK_PORT, CCxxx0_SCK_PIN)
#define  CC1100_SET_SI()   GPIO_SetBits(CCxxx0_SI_PORT, CCxxx0_SI_PIN)
#define  CC1100_CLR_SI()   GPIO_ResetBits(CCxxx0_SI_PORT, CCxxx0_SI_PIN)
#define  CC1100_GET_SO()   GPIO_ReadInputDataBit(CCxxx0_SO_PORT, CCxxx0_SO_PIN)
#define  CC1100_GET_GDO0() GPIO_ReadInputDataBit(CCxxx0_GDO0_PORT, CCxxx0_GDO0_PIN)
#define  CC1100_GET_GDO2() GPIO_ReadInputDataBit(CCxxx0_GDO2_PORT, CCxxx0_GDO2_PIN)

/*********************************************************************
 * @fn      cc1100_wait
 *
 * @brief
 *
 *   wait for a while.
 *
 * @param   void
 *
 * @return  void
 */
void cc1100_wait(void)
{
    __nop();__nop();__nop();__nop(); 
	__nop();__nop();__nop();__nop();
    __nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop(); 
	__nop();__nop();__nop();__nop();
    __nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop(); 
	__nop();__nop();__nop();__nop();
    __nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();
}

/*********************************************************************
 * @fn      cc1100_Delay
 *
 * @brief
 *
 *   delay n us.
 *
 * @param   void
 *
 * @return  void
 */
void cc1100_Delay(uint8_t timeout)
{
    do
    {
        __nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();
		__nop();__nop();	
		__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();
		__nop();__nop();
    } while(-- timeout);
}

/*********************************************************************
 * @fn      cc1100_Init
 *
 * @brief
 *
 *   init.
 *
 * @param   void
 *
 * @return  void
 */
void cc1100_Init(void)
{

}

/*********************************************************************
 * @fn      cc1100_writebyte
 *
 * @brief
 *
 *   write a byte to cc1100.
 *
 * @param   value -
 *
 * @return  status
 */
uint8_t cc1100_writebyte(uint8_t value)
{
    uint8_t i, status = 0;
    
    for(i = 0; i < 8; i ++) 
    { 
        CC1100_CLR_SCK();
        if(value & 0x80) 
            CC1100_SET_SI();
        else 
            CC1100_CLR_SI();
        
        cc1100_wait();
        
        CC1100_SET_SCK();
        
        value <<= 1;
        status <<= 1;	
        
        if(CC1100_GET_SO())
           status = status | 0x01;
        else
           status = status & 0xfe;
    }
    
    CC1100_CLR_SCK();
    
    return(status);
}

/*********************************************************************
 * @fn      cc1100_readbyte
 *
 * @brief
 *
 *   read a byte from cc1100.
 *
 * @param   none
 *
 * @return  value
 */
uint8_t cc1100_readbyte(void)
{
    uint8_t i, value;
    
    for(i = 0; i < 8;i ++) 
    { 
        value <<= 1;
        
        CC1100_SET_SCK();
                       
        if(CC1100_GET_SO()) 
            value |= 0x01;
        else 
            value &= 0xFE;
    
        CC1100_CLR_SCK();
    }    
    
    return value;
}

//reset the CCxxx0 and wait for it to be ready
void cc1100_Reset(void) 
{ 
    uint8_t  delayCnt = 0;
    
    CC1100_CLR_CS();
    while (CC1100_GET_SO())
    {
        if(delayCnt ++ >= CCxxx0_DELAY_CNT)
            break;
    }
    cc1100_writebyte(CCxxx0_SRES); 
    cc1100_wait(); 
    CC1100_SET_CS();
} 
 
// reset the CCxxx0 after power_on and wait for it to be ready
//
//                 min 40 us
//             <----------------------->
// CSn      |--|  |--------------------|          |-----------
//          |  |  |                    |          |
//              --                      ----------
//
// MISO                                       |---------------
//          - - - - - - - - - - - - - - - -|  |         
//                                          --          
//               Unknown / don't care
//
// MOSI     - - - - - - - - - - - - - - - ---------- - - - - - 
//                                         | SRES |
//          - - - - - - - - - - - - - - - ---------- - - - - -                    
//  
void cc1100_PowerUp_Reset(void) 
{ 
    CC1100_SET_CS();
    cc1100_Delay(1); 
    CC1100_CLR_CS();
    cc1100_Delay(1); 
    CC1100_SET_CS();
    cc1100_Delay(41); 
    cc1100_Reset(); 
}

/*********************************************************************
 * @fn      cc1100_ReadBurstReg
 *
 * @brief
 *
 *   read burst register.
 *
 * @param   addr -
 *          buffer - 
 *          count - 
 *
 * @return  void
 */
void cc1100_ReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count) 
{
    uint8_t  i, delayCnt = 0;
    
    CC1100_CLR_CS();
    while (CC1100_GET_SO())
    {
        if(delayCnt ++ >= CCxxx0_DELAY_CNT)
            break;
    }
    cc1100_writebyte(addr | CCxxx0_READ_BURST);
    cc1100_wait();  
    for (i = 0; i < count; i ++) 
    {
        buffer[i] = cc1100_readbyte();
        cc1100_wait();
    }
    CC1100_SET_CS();
}

/*********************************************************************
 * @fn      cc1100_ReadReg
 *
 * @brief
 *
 *   read register.
 *
 * @param   addr -
 *
 * @return  value
 */
uint8_t cc1100_ReadReg(uint8_t addr) 
{
    uint8_t x, delayCnt = 0;
    
    CC1100_CLR_CS();
    while (CC1100_GET_SO())
    {
        if(delayCnt ++ >= CCxxx0_DELAY_CNT)
            break;
    }
    cc1100_writebyte(addr | CCxxx0_READ_SINGLE);
    cc1100_wait();
    x = cc1100_readbyte();
    CC1100_SET_CS();
    return x;
}

/*********************************************************************
 * @fn      cc1100_ReadStatus
 *
 * @brief
 *
 *   read status.
 *
 * @param   addr -
 *
 * @return  status
 */
uint8_t cc1100_ReadStatus(uint8_t addr) 
{
    uint8_t x, delayCnt = 0;
    
    CC1100_CLR_CS();
    while (CC1100_GET_SO())
    {
        if(delayCnt ++ >= CCxxx0_DELAY_CNT)
            break;
    }
    cc1100_writebyte(addr | CCxxx0_READ_BURST);
    cc1100_wait();
    x = cc1100_readbyte();
    CC1100_SET_CS();
    return x;
}

/*********************************************************************
 * @fn      cc1100_Strobe
 *
 * @brief
 *
 *   strobe.
 *
 * @param   strobe -
 *
 * @return  void
 */
void cc1100_Strobe(uint8_t strobe) 
{
    uint8_t  delayCnt = 0;
    
    CC1100_CLR_CS();
    while (CC1100_GET_SO())
    {
        if(delayCnt ++ >= CCxxx0_DELAY_CNT)
            break;
    }
    cc1100_writebyte(strobe);
    cc1100_wait();
    CC1100_SET_CS();
}

/*********************************************************************
 * @fn      cc1100_WriteReg
 *
 * @brief
 *
 *   write register.
 *
 * @param   addr -
 *          value -
 *
 * @return  void
 */
void cc1100_WriteReg(uint8_t addr, uint8_t value) 
{
    uint8_t  delayCnt = 0;
    
    CC1100_CLR_CS();
    while (CC1100_GET_SO())
    {
        if(delayCnt ++ >= CCxxx0_DELAY_CNT)
            break;
    }
    cc1100_writebyte(addr);
    cc1100_wait();
    cc1100_writebyte(value);
    cc1100_wait();
    CC1100_SET_CS();
}

/*********************************************************************
 * @fn      cc1100_WriteBurstReg
 *
 * @brief
 *
 *   write burst register.
 *
 * @param   addr -
 *          buffer -
 *          count -
 *
 * @return  void
 */
void cc1100_WriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count) 
{
    uint8_t i, delayCnt = 0;
    
    CC1100_CLR_CS();
    while (CC1100_GET_SO())
    {
        if(delayCnt ++ >= CCxxx0_DELAY_CNT)
            break;
    }
    cc1100_writebyte(addr | CCxxx0_WRITE_BURST);
    cc1100_wait();
    for (i = 0; i < count; i ++) 
    {
        cc1100_writebyte(buffer[i]);
        cc1100_wait();
    }
    CC1100_SET_CS();
}

/*********************************************************************
 * @fn      cc1100_WriteSettings
 *
 * @brief
 *
 *   write setting value.
 *
 * @param   pCCxxx0Settings -
 *
 * @return  void
 */
void cc1100_WriteSettings(const CCxxx0_SETTINGS *pCCxxx0Settings) 
{

    // Write register settings
    cc1100_WriteReg(CCxxx0_FSCTRL1,  pCCxxx0Settings->FSCTRL1);
    cc1100_WriteReg(CCxxx0_FSCTRL0,  pCCxxx0Settings->FSCTRL0);
    cc1100_WriteReg(CCxxx0_FREQ2,    pCCxxx0Settings->FREQ2);
    cc1100_WriteReg(CCxxx0_FREQ1,    pCCxxx0Settings->FREQ1);
    cc1100_WriteReg(CCxxx0_FREQ0,    pCCxxx0Settings->FREQ0);
    cc1100_WriteReg(CCxxx0_MDMCFG4,  pCCxxx0Settings->MDMCFG4);
    cc1100_WriteReg(CCxxx0_MDMCFG3,  pCCxxx0Settings->MDMCFG3);
    cc1100_WriteReg(CCxxx0_MDMCFG2,  pCCxxx0Settings->MDMCFG2);
    cc1100_WriteReg(CCxxx0_MDMCFG1,  pCCxxx0Settings->MDMCFG1);
    cc1100_WriteReg(CCxxx0_MDMCFG0,  pCCxxx0Settings->MDMCFG0);
    cc1100_WriteReg(CCxxx0_CHANNR,   pCCxxx0Settings->CHANNR);
    cc1100_WriteReg(CCxxx0_DEVIATN,  pCCxxx0Settings->DEVIATN);
    cc1100_WriteReg(CCxxx0_FREND1,   pCCxxx0Settings->FREND1);
    cc1100_WriteReg(CCxxx0_FREND0,   pCCxxx0Settings->FREND0);
    cc1100_WriteReg(CCxxx0_MCSM0 ,   pCCxxx0Settings->MCSM0 );
    cc1100_WriteReg(CCxxx0_FOCCFG,   pCCxxx0Settings->FOCCFG);
    cc1100_WriteReg(CCxxx0_BSCFG,    pCCxxx0Settings->BSCFG);
    cc1100_WriteReg(CCxxx0_AGCCTRL2, pCCxxx0Settings->AGCCTRL2);
    cc1100_WriteReg(CCxxx0_AGCCTRL1, pCCxxx0Settings->AGCCTRL1);
    cc1100_WriteReg(CCxxx0_AGCCTRL0, pCCxxx0Settings->AGCCTRL0);
    cc1100_WriteReg(CCxxx0_FSCAL3,   pCCxxx0Settings->FSCAL3);
    cc1100_WriteReg(CCxxx0_FSCAL2,   pCCxxx0Settings->FSCAL2);
    cc1100_WriteReg(CCxxx0_FSCAL1,   pCCxxx0Settings->FSCAL1);
    cc1100_WriteReg(CCxxx0_FSCAL0,   pCCxxx0Settings->FSCAL0);
    cc1100_WriteReg(CCxxx0_FSTEST,   pCCxxx0Settings->FSTEST);
    cc1100_WriteReg(CCxxx0_TEST2,    pCCxxx0Settings->TEST2);
    cc1100_WriteReg(CCxxx0_TEST1,    pCCxxx0Settings->TEST1);
    cc1100_WriteReg(CCxxx0_TEST0,    pCCxxx0Settings->TEST0);
    cc1100_WriteReg(CCxxx0_FIFOTHR,  pCCxxx0Settings->FIFOTHR);
    cc1100_WriteReg(CCxxx0_IOCFG2,   pCCxxx0Settings->IOCFG2);
    cc1100_WriteReg(CCxxx0_IOCFG0,   pCCxxx0Settings->IOCFG0);    
    cc1100_WriteReg(CCxxx0_PKTCTRL1, pCCxxx0Settings->PKTCTRL1);
    cc1100_WriteReg(CCxxx0_PKTCTRL0, pCCxxx0Settings->PKTCTRL0);
    cc1100_WriteReg(CCxxx0_ADDR,     pCCxxx0Settings->ADDR);
    cc1100_WriteReg(CCxxx0_PKTLEN,   pCCxxx0Settings->PKTLEN);
}

/*********************************************************************
 * @fn      cc1100_ReceivePacket
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
uint8_t cc1100_ReceivePacket(uint8_t *rxBuffer, uint8_t *length) 
{
    uint8_t status[2];
    uint8_t packetLength;

    cc1100_Strobe(CCxxx0_SRX);

    // Wait for GDO0 to be set -> sync received
    while (!CC1100_GET_GDO0());

    // Wait for GDO0 to be cleared -> end of packet
    while (CC1100_GET_GDO0());

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

/*********************************************************************
 * @fn      cc1100_SendPacket
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
void cc1100_SendPacket(uint8_t *txBuffer, uint8_t size) 
{

    cc1100_WriteBurstReg(CCxxx0_TXFIFO, txBuffer, size);

    cc1100_Strobe(CCxxx0_STX);

    // Wait for GDO0 to be set -> sync transmitted
    while (!CC1100_GET_GDO0());

    // Wait for GDO0 to be cleared -> end of packet
    while (CC1100_GET_GDO0());

}
