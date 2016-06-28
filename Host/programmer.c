#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

struct termios Serial_settings;

unsigned char buffer[2048];
unsigned char byte[2];
unsigned char end = 'q';
unsigned char rec;

int main(int argc, char *argv[])
{
	int Serial_desc, Firmware_desc, i;
	ssize_t count = 0;
	if (argc != 3) {
		perror("Wrong input specified...\nPlease provide serial port followed by firmware file\n");
		exit(1);
	}
	Serial_desc = open(argv[1], O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (Serial_desc < 0) {
		perror("Unable to open serial port");
		exit(1);
	}
	Firmware_desc = open(argv[2], O_RDWR);
	if (Firmware_desc < 0) {
		perror("Unable to open Firmware file\n");
	}

        if (tcgetattr(Serial_desc, &Serial_settings) != 0) {
                perror("Unable to get Serial port settings\n");
                exit(1);
        }
        memset(&Serial_settings,0,sizeof(Serial_settings));
	Serial_settings.c_cflag &= ~CSIZE;
        Serial_settings.c_cflag = CS8 | CREAD | CLOCAL;     /* 8 data bits and enable receiver */
        Serial_settings.c_cflag &= ~PARENB;                 /* No parity */
        Serial_settings.c_cflag &= ~CSTOPB;                 /* Stop bit=1 */
        Serial_settings.c_cflag &= ~CRTSCTS;                /* No hardware flow control */
        Serial_settings.c_cflag &= ~(IXON + IXOFF + IXANY); /* No software flow control */
        cfsetispeed(&Serial_settings, B9600);
        cfsetospeed(&Serial_settings, B9600);
        if (tcsetattr(Serial_desc, TCSANOW, &Serial_settings) != 0) {
                perror("Unable to set Serial port settings\n");
                exit(1);
        }
        printf("Serial Port settings set successfully\n");
        printf("\t1. Baud rate : 9600\n");
        printf("\t2. Data bits : 8\n");
        printf("\t3. Stop bit  : 1\n");
        printf("\t4. Parity bit : None\n");
	
	count = read(Firmware_desc, buffer, sizeof(buffer));
	i = 7;
        do{
                byte[0] = buffer[i];
                byte[1] = buffer[i+1];
                printf("%s ", byte);
		count = write(Serial_desc, &byte[0], sizeof(byte[0]));
		count = write(Serial_desc, &byte[1], sizeof(byte[1]));
		count = read(Serial_desc, &rec, sizeof(rec));
		while(rec != byte[0]);
                i += 3;
                if (buffer[i] == '\r') {
                        i = i+2;
                }
        }while(buffer[i] != '@');

	count = write(Serial_desc, &end, sizeof(end));
	close(Firmware_desc);
	close(Serial_desc);
	return 0;
}

