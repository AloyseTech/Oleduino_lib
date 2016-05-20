//
//  SPIdma.cpp
//  
//
//  Created by Théo Meyer on 16/11/2015.
//
//

#include "SPIdma.h"
#include <SPI.h>


// DMA   12 channels
//DMA::DMA(){}
volatile uint32_t dmadone;

void DMAC_Handler() {
    // interrupts DMAC_CHINTENCLR_TERR DMAC_CHINTENCLR_TCMPL DMAC_CHINTENCLR_SUSP
    uint8_t active_channel;
    
    // disable irqs ?
    __disable_irq();
    active_channel =  DMAC->INTPEND.reg & DMAC_INTPEND_ID_Msk; // get channel number
    DMAC->CHID.reg = DMAC_CHID_ID(active_channel);
    dmadone = DMAC->CHINTFLAG.reg;
    DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TCMPL; // clear
    DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TERR;
    DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_SUSP;
    __enable_irq();
}

void DMA::init() {
    txsrc[0]= 0xff;

    // probably on by default
    PM->AHBMASK.reg |= PM_AHBMASK_DMAC ;
    PM->APBBMASK.reg |= PM_APBBMASK_DMAC ;
    NVIC_EnableIRQ( DMAC_IRQn ) ;
    
    DMAC->BASEADDR.reg = (uint32_t)descriptor_section;
    DMAC->WRBADDR.reg = (uint32_t)wrb;
    DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);
}

void DMA::end(){
    DMAC->CTRL.bit.DMAENABLE = 0;                      // disable DMA controller
    DMAC->CTRL.bit.SWRST = 1;                          // reset all DMA registers
    while (DMAC->CTRL.bit.SWRST){};                    // wait for reset to complete
    //DMAC->CTRL.bit.DMAENABLE = 1;                      // enable DMA controller
}


void DMA::spi_xfr(void *txdata, void *rxdata,  size_t n) {
    uint32_t temp_CHCTRLB_reg;
    
    // set up transmit channel
    DMAC->CHID.reg = DMAC_CHID_ID(chnltx);
    DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
    DMAC->CHCTRLA.reg = DMAC_CHCTRLA_SWRST;
    DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 << chnltx));
    temp_CHCTRLB_reg = DMAC_CHCTRLB_LVL(0) |
    DMAC_CHCTRLB_TRIGSRC(SERCOM4_DMAC_ID_TX) | DMAC_CHCTRLB_TRIGACT_BEAT;
    DMAC->CHCTRLB.reg = temp_CHCTRLB_reg;
    DMAC->CHINTENSET.reg = DMAC_CHINTENSET_MASK ; // enable all 3 interrupts
    descriptor.descaddr = 0;
    descriptor.dstaddr = (uint32_t) &sercom->SPI.DATA.reg;
    descriptor.btcnt =  n;
    descriptor.srcaddr = (uint32_t)txdata;
    descriptor.btctrl =  DMAC_BTCTRL_VALID;
    if (xtype != DoRX) {
        descriptor.srcaddr += n;
        descriptor.btctrl |= DMAC_BTCTRL_SRCINC;
    }
    memcpy(&descriptor_section[chnltx],&descriptor, sizeof(dmacdescriptor));
    
    // rx channel    enable interrupts
    DMAC->CHID.reg = DMAC_CHID_ID(chnlrx);
    DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
    DMAC->CHCTRLA.reg = DMAC_CHCTRLA_SWRST;
    DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 << chnlrx));
    temp_CHCTRLB_reg = DMAC_CHCTRLB_LVL(0) |
    DMAC_CHCTRLB_TRIGSRC(SERCOM4_DMAC_ID_RX) | DMAC_CHCTRLB_TRIGACT_BEAT;
    DMAC->CHCTRLB.reg = temp_CHCTRLB_reg;
    DMAC->CHINTENSET.reg = DMAC_CHINTENSET_MASK ; // enable all 3 interrupts
    dmadone = 0;
    descriptor.descaddr = 0;
    descriptor.srcaddr = (uint32_t) &sercom->SPI.DATA.reg;
    descriptor.btcnt =  n;
    descriptor.dstaddr = (uint32_t)rxdata;
    descriptor.btctrl =  DMAC_BTCTRL_VALID;
    if (xtype != DoTX) {
        descriptor.dstaddr += n;
        descriptor.btctrl |= DMAC_BTCTRL_DSTINC;
    }
    memcpy(&descriptor_section[chnlrx],&descriptor, sizeof(dmacdescriptor));
    
    // start both channels  ? order matter ?
    DMAC->CHID.reg = DMAC_CHID_ID(chnltx);
    DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;
    DMAC->CHID.reg = DMAC_CHID_ID(chnlrx);
    DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;
    
    while(!dmadone);  // await DMA done isr
    
    DMAC->CHID.reg = DMAC_CHID_ID(chnltx);   //disable DMA to allow lib SPI
    DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
    DMAC->CHID.reg = DMAC_CHID_ID(chnlrx);
    DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
}

void DMA::spi_write(void *data,  size_t n) {
    xtype = DoTX;
    spi_xfr(data,rxsink,n);
}
void DMA::spi_read(void *data,  size_t n) {
    xtype = DoRX;
    spi_xfr(txsrc,data,n);
}
void DMA::spi_transfer(void *txdata, void *rxdata,  size_t n) {
    xtype = DoTXRX;
    spi_xfr(txdata,rxdata,n);
}

