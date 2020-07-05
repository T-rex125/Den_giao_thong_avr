/*
 * ATmega 16/32 interfacing with RTC DS1307
 * http://www.electronicwings.com
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
#define thuong 0
#define caodiem 1
#define dem 2
//--------------------------------------------------------------------------------
//Khai bao chan hien thi den
#define den_xanh PINC3
#define den_vang PINC4
#define den_do PINC5
#define DK1 PINC6
#define DK2 PINC7
#define Led7thanh PORTD
//Khai bao mang hien thi ky tu
uint8_t number[10]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};

/* ------------------------------------------------------------------
--------------------------------------------------------------------*/

//Ham hien thi led 7 thanh
void hien_thi_led_7thanh(uint16_t x)
{
	uint16_t temp, chuc, dv;
	//Tach hang chuc, don vi ra
	temp = x;
	dv = temp % 10;
	temp = temp/10;
	chuc = temp %10;
	
	PORTC |= (1<<DK1);
	Led7thanh = number[chuc];
	_delay_ms(1);
	PORTC &= ~(1<<DK1);
	
	PORTC |= (1<<DK2);
	Led7thanh = number[dv];
	_delay_ms(1);
	PORTC &=~(1<<DK2);

}
void den_giao_thong(int8_t xanh, int8_t vang, int8_t red)
{
	while(xanh>=0)
	{
		PORTC |= (1<<den_xanh);
		for(int i=0;i<500; i++)
		{
			hien_thi_led_7thanh(xanh);
		}
		xanh--;
	}
	PORTC &=~(1<<den_xanh);
	
	while(vang>=0)
	{
		PORTC |= (1<<den_vang);
		for(int i=0;i<500; i++)
		{
			hien_thi_led_7thanh(vang);
		}
		vang--;
	}
	PORTC &=~(1<<den_vang);
	
	while(red >=0)
	{
		PORTC |= (1<<den_do);
		for(int i=0;i<500; i++)
		{
			hien_thi_led_7thanh(red);
		}
		red--;
	}
	PORTC &=~(1<<den_do);
}
//Ham chon che do
void Mode(uint8_t y)
{
	if(y == thuong)
		den_giao_thong(15,5,10);
	if(y == caodiem)
		den_giao_thong(10,5,10);
	if(y == dem)
		den_giao_thong(20,5,15);
}
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

	I2C_Init();										/* Initialize I2C */
	lcdinit();										/* Initialize LCD16x2 */

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
		DDRD = 0xFF;
		PORTD = 0x00;
		DDRC |= (1<<den_do)|(1<<den_vang)|(1<<den_xanh)|(1<<DK1)|(1<<DK2);
//		PORTC &= ~(1<<den_do)&(1<<den_vang)&(1<<den_xanh)&(1<<DK1)&(1<<DK2);
		if((8<=hour<=9)&&(17<=hour<<18))
			Mode(1);
		else if (0<=hour<=5)
			Mode(2);
		else
			Mode(0);
		//--------------------------------------------------------
		RTC_Read_Calendar(3);						/* Read the calender with day address i.e location is 3 */
	    sprintf(buffer, "%02x/%02x/%02x %3s ", date, month, year,days[day-1]);
		lcd_print_xy(1,0,buffer);
    }
}