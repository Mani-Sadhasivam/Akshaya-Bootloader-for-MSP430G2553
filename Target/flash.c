
#include "msp430g2553.h"

#define APP_START_ADDR  0XF800
#define APP_END_ADDR  0xFC00

unsigned char Sector_Erase(unsigned int addr)
{
    FCTL1 = FWKEY + ERASE;                      // Enable flash erase
    FCTL2 = FWKEY + FSSEL_2 + 2;              // Flash timing setup (SMCLK/3) = 333.333Khz
    FCTL3 = FWKEY;                              // Disable lock
    *(unsigned int *)addr = 0;                  // Dummy write to erase flash
    FCTL1 = FWKEY;
    FCTL3 = FWKEY+LOCK;                         // Diasble flash write

    return 1;
}


void Flash_Erase(void)
{
    unsigned int addr;

    for (addr = APP_START_ADDR; addr < APP_END_ADDR; addr += 512)
    {
    	Sector_Erase(addr);
    }
}


unsigned char Flash_Write(unsigned int addr, unsigned char data)
{
    FCTL3 = FWKEY;
    FCTL1 = FWKEY+ WRT;                         // Enable flash write
    *(unsigned char*)addr = data;               // Write data to flash
    FCTL1 = FWKEY;                              // Disable flash write
    FCTL3 = FWKEY + LOCK;

    return 1;
}
