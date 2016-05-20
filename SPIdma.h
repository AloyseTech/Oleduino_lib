//
//  SPIdma.h
//  
//
//  Created by Théo Meyer on 16/11/2015.
//
//

#ifndef _SPIdma_h
#define _SPIdma_h

#include <SPI.h>
//#define SPISPEED 12000000

#define PRREG(x) Serial.print(#x" 0x"); Serial.println(x,HEX)


class DMA{
public:
    // DMA   12 channels
    typedef struct {
        uint16_t btctrl;
        uint16_t btcnt;
        uint32_t srcaddr;
        uint32_t dstaddr;
        uint32_t descaddr;
    } dmacdescriptor ;

    volatile dmacdescriptor wrb[12] __attribute__ ((aligned (16)));
    dmacdescriptor descriptor_section[12] __attribute__ ((aligned (16)));
    dmacdescriptor descriptor __attribute__ ((aligned (16)));

    static const uint32_t chnltx=0, chnlrx=1; // DMA channels
    enum XfrType { DoTX, DoRX, DoTXRX};
    XfrType xtype;
    uint8_t rxsink[1], txsrc[1];
    Sercom *sercom = (Sercom   *)SERCOM4;  // SPI SERCOM

    //DMA();
    void DMAC_Handler();
    void init();
    void end();
    void spi_xfr(void *txdata, void *rxdata,  size_t n);
    void spi_write(void *data,  size_t n);
    void spi_read(void *data,  size_t n);
    void spi_transfer(void *txdata, void *rxdata,  size_t n);

};

#endif
