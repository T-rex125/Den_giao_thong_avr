/*
 * 
 *
 */ 
#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "I2C_Master_H_file.h"
#include "LCD16x2_4Bit.h"

#define Device_Write_address	0xD0				/* Define RTC DS1307 slave address for write operation */
#define Device_Read_address		0xD1				/* Make LSB bit high of slave address for read operation */
#define TimeFormat12			0x40				/* Define 12 hour format */
#define AMPM					0x20

//--------------------------------------------------------------------------------
//Khai bao mang hien thi ky tu
uint8_t number[10]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};

/* ------------------------------------------------------------------
--------------------------------------------------------------------*/
// dinh nghia cac chan giao tiep SPI cua AVR
#define SPI_PORT PORTB
#define SPI_DDR  DDRB
#define SCK_PIN 7
#define MISO_PIN 6
#define MOSI_PIN 5
#define SS 4

#define sbi(port,bit)	(port) |= (1<<bit)
#define cbi(port,bit)	(port) &= ~(1<<bit)

//Init SPI Master
void SPI_Master_Init()
{
	SPI_DDR |= (1<<SCK_PIN)| (1<<MOSI_PIN)|(1<<SS);
//	SPI_PORT |= (1<<MISO_PIN);
	//Cau hinh master
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	
	sbi(SPI_PORT, SS);
}
void SPI_Transmit(uint8_t data)
{
	cbi(SPI_PORT,SS);
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
	sbi(SPI_PORT,SS);	
}
//-----------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

int second,minute,hour,day,date,month,year;

bool IsItPM(char hour_)
{
	if(hour_ & (AMPM))
	return 1;
	else
	return 0;
}

void RTC_Read_Clock(char read_clock_address)
{
	I2C_Start(Device_Write_address);				/* Start I2C communication with RTC */
	I2C_Write(read_clock_address);					/* Write address to read */
	I2C_Repeated_Start(Device_Read_address);		/* Repeated start with device read address */

	second = I2C_Read_Ack();						/* Read second */
	minute = I2C_Read_Ack();						/* Read minute */
	hour = I2C_Read_Nack();							/* Read hour with Nack */
	I2C_Stop();										/* Stop i2C communication */
}

void RTC_Read_Calendar(char read_calendar_address)
{
	I2C_Start(Device_Write_address);
	I2C_Write(read_calendar_address);
	I2C_Repeated_Start(Device_Read_address);

	day = I2C_Read_Ack();							/* Read day */ 
	date = I2C_Read_Ack();							/* Read date */
	month = I2C_Read_Ack();							/* Read month */
	year = I2C_Read_Nack();							/* Read the year with Nack */
	I2C_Stop();										/* Stop i2C communication */
}

int main(void)
{
	char buffer[20];
	char* days[7]= {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

	I2C_Init();										
	lcdinit();										
	
	SPI_Master_Init();
	_delay_ms(10);
    while(1)
    {
		RTC_Read_Clock(0);							/* Read the clock with second address i.e location is 0 */
		if (hour & TimeFormat12)
		{
			sprintf(buffer, "%02x:%02x:%02x  ", (hour & 0b00011111), minute, second);
			if(IsItPM(hour))
			strcat(buffer, "PM");
			else
			strcat(buffer, "AM");
			lcd_print_xy(0,0,buffer);
		}
		else
		{
			sprintf(buffer, "%02x:%02x:%02x  ", (hour & 0b00011111), minute, second);
			lcd_print_xy(0,0,buffer);
		}
		//--------------------------------------------------------
		RTC_Read_Calendar(3);						/* Read the calender with day address i.e location is 3 */
		sprintf(buffer, "%02x/%02x/%02x %3s ", date, month, year,days[day-1]);
		lcd_print_xy(1,0,buffer);		
		//--------------------------------------------------------
		int temp = hour;
		DDRA = 0xFF;
		if(temp==17)
		{
			sbi(PORTA,PINA0);
			SPI_Transmit(1);
			_delay_ms(10);
			PORTB |= (1<<PINB1);
		}
		else if (temp == 0 && temp==5)
		{
			sbi(PORTA,PINA1);
			SPI_Transmit(2);
			_delay_ms(10);
		}
		else
		{
			sbi(PORTA,PINA2);
			SPI_Transmit(0);
			_delay_ms(10);
		}
    }
}