#include <msp430g2553.h>

/*
 * main.c
 */

void clock_config(void);
void periph_config(void);

#define boot_condition() (!(P1IN & BIT3))

unsigned char *welcome_str = "/****Akshaya Bootloader****/\r\n";
unsigned char *menu = "1. Program Application Code\r\n2. Erase Application Memory\r\n";
unsigned char *erase_str_1 = "Erasing Application Sector\r\n";
unsigned char *erase_str_2 = "Application Sector Erased\r\n";

int i;
int address = 0xF800;
unsigned char rdata;


void (*jumpToApp)(void);

int main(void) {
	clock_config();
	periph_config();
	uart_config();
	__delay_cycles(1000000);
	if (boot_condition()) {
		P1OUT |= BIT0;
		while(*welcome_str) {
			send_byte(*welcome_str);
			welcome_str++;
		}
		while(*menu) {
			send_byte(*menu);
			menu++;
		}
		rdata = rec_byte();
		if (rdata == '1') {
			while(*erase_str_1) {
				send_byte(*erase_str_1);
				erase_str_1++;
			}
			Flash_Erase();
			while(*erase_str_2) {
				send_byte(*erase_str_2);
				erase_str_2++;
			}
			do {
				byte[0] = rec_byte();
				send_byte(byte[0]);
				byte[0] = atoh(byte[0]);
				byte[1] = rec_byte();
				send_byte(byte[1]);
				Flash_Write(address,byte[0]);
				address++;
				Flash_Write(address,byte[1]);
				address++;
			}while (byte[0] != 'q');
			/*
			for (i=0;i<120;i++) {
				Flash_Write(address,app[i]);
				address++;
			}*/
			jumpToApp = (unsigned int *)0xF800;  // Jump to Application
			jumpToApp();

		}
	}
	while(1);
	return 0;
}

void clock_config(void)
{
	WDTCTL = WDTPW + WDTHOLD;	// Stop watchdog timer
	if (CALBC1_1MHZ==0xFF)					// If calibration constant erased
	{
		P1DIR |= BIT0;
		P1OUT |= BIT0;
		while(1);                               // do not load, trap CPU!!
	}
	DCOCTL = 0;                               // Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
	__disable_interrupt();      // Disable all interrupts
}

void periph_config(void)
{
	P1DIR = BIT0 + BIT1;
	P1DIR &=~BIT3;
	P1REN |=BIT3;
	P1OUT &=~BIT0;
	P1OUT &=~BIT1;
	P2DIR = 0XFF;
	P2OUT = 0X00;
}
