
#include <msp430g2553.h>

unsigned char Rdata;

void uart_config(void)
{
  P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
  P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD
  UCA0CTL1 |= UCSSEL_2 + UCSWRST;                     // SMCLK
  UCA0BR0 = 104;                            // 1MHz 9600
  UCA0BR1 = 0;                              // 1MHz 9600
  UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  //IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
 // IE2 |= UCA0TXIE;                          // Enable USCI_A0 RX interrupt
  _BIS_SR(GIE);      						// Interrupts enabled
}


void send_byte(unsigned char byte)
{
	while (!(IFG2&UCA0TXIFG));             // USCI_A0 TX buffer ready?
	UCA0TXBUF = byte;                       // Transmit the data
	while(UCA0STAT&UCBUSY);
}

unsigned char rec_byte(void)
{
  while (!(IFG2&UCA0RXIFG));                // USCI_A0 TX buffer ready?
  Rdata = UCA0RXBUF;
  return Rdata;
}
