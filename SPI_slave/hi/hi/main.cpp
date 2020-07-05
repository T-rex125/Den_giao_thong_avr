/*
 * hi.cpp
 *
 * Created: 11/06/2020 19:57:42
 * Author : Joker's
 */ 
#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
// dinh nghia cac chan giao tiep SPI cua AVR
#define SPI_PORT PORTB
#define SPI_DDR  DDRB
#define SCK_PIN 7
#define MISO_PIN 6
#define MOSI_PIN 5
#define SS 4

//Khai bao chan hien thi den
#define den_xanh PINC3
#define den_vang PINC4
#define den_do PINC5
#define DK1 PINC6
#define DK2 PINC7
#define Led7thanh PORTD
//-------------------------------------
#define thuong 0
#define caodiem 1
#define dem 2
//Khai bao mang hien thi ky tu
uint8_t number[10]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
	
//-------------------------------------
volatile unsigned char rData;
//-------------------------------------
void hien_thi_led_7thanh(uint16_t x);
void den_giao_thong(int8_t xanh, int8_t vang, int8_t red);
void Mode(uint8_t y);
void Slave_Init();
//-------------------------------------

int main()
{
	DDRA = 0xFF;
	DDRC = 0xFF;
	DDRD = 0xFF;
	Slave_Init();
	sei();
	while(1)
	{
		
	}
	return 0;
}
ISR(SPI_STC_vect)
{
	if(SPDR == 0)
	{
		Mode(thuong);
	}
	else if(SPDR == 1)
	{
		Mode(caodiem);
	}
	else if(SPDR == 2)
	{
		Mode(dem);
	}
}
void Slave_Init()
{
	SPI_DDR |= (1<<MISO_PIN);
	SPI_PORT |= (1<<MOSI_PIN)|(1<<SS);
	// che do slave;
	
	SPCR |= (1<<SPE)|(1<<SPIE);
	//SPE = 1 : Enable; = 0 : Disable
	//MSTR = 1 : Master;
}
//-----------------------------------------------------------------------------------------
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


