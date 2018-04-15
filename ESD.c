#include <LPC17xx.h>
#include <stdio.h>

#define Refference_Voltage 3.300
#define Full_Scale 0xfff
unsigned long int temp1, temp2;
#define RS_CTRL (1<<8)
#define EN_CTRL (1<<9)
#define DT_CTRL (0xf<<4)
#define alpha 0.5

void lcd_init(void);
void wr_cn(void);
void clr_disp(void);
void delay_lcd(unsigned int);
void lcd_com(void);
void wr_dn(void);
void lcd_data(void);
void clear_ports(void);
void lcd_puts(unsigned char *);

int main(void)
{
	SystemInit();
	SystemCoreClockUpdate();
	unsigned long adc_temp, adc_temp1;
	unsigned int i;
	float input_voltage, distance;
	unsigned char volt[7];
	unsigned char Msg[11] = {"Distance="};
	LPC_SC->PCONP |= (1<<12); // Enableing the poer supply.
	lcd_init(); // Iniializing the LCD
	LPC_PINCON->PINSEL1 = (01 << 14); // using the 14th pin
	temp1 = 0x80;
	lcd_com();
	delay_lcd(800);
	lcd_puts(&Msg[0]);
	while(1)
	{
		LPC_ADC->ADCR = (1<<0) | (1<<21) | (001<<24);
		while((adc_temp = LPC_ADC->ADGDR) == 0x80000000);
		//Infinite loop on completion of an instance sample
		// else put the lower block in
		//while((adc_temp = LPC_ADC->ADGDR) =! 0x80000000);
		adc_temp = (LPC_ADC->ADGDR >> 4) & 0xfff; // output result is
		//stored in bits 15:4 , extracting the output.
		input_voltage = ((float)adc_temp * Refference_Voltage) / Full_Scale;
		// adc_temp * resolution
		distance = 27/input_voltage; // Distance calculated
		sprintf(volt, "%3.2f", distance); // stores distance as a string in volt
		for(i=0; i < 2000; i++);
		temp1 = 0x8c;
		lcd_com();
		delay_lcd(800);
		lcd_puts(&volt[0]);
		for(i = 0; i < 20000; i++);
		for(i = 0; i < 7; i++)
		{
			volt[i] = 0x00;
			adc_temp = 0;
			input_voltage = 0;
		}
	}
}

void lcd_init()
{
	LPC_PINCON->PINSEL0 &= 0xfff000ff;
	LPC_GPIO0->FIODIR|=DT_CTRL;
	LPC_GPIO0->FIODIR|=RS_CTRL;
	LPC_GPIO0->FIODIR|=EN_CTRL;
	clear_ports();
	delay_lcd(3200);
	int k = 0 ;
	for(;k<3;k++)
	{
		temp2=(0x30<<0);
		wr_cn();
		delay_lcd(30000);
	}
	temp2=(0x20<<0);
	wr_cn();
	delay_lcd(30000);
	temp1=0x28;
	lcd_com();
	delay_lcd(30000);
	temp1=0x0c;
	lcd_com();
	delay_lcd(30000);
	temp1=0x06;
	lcd_com();
	delay_lcd(800);
	temp1=0x01;
	lcd_com();
	delay_lcd(800);
	temp1=0x80;
	lcd_com();
	delay_lcd(10000);
	return;
}

void lcd_com(void)
{
	temp2= temp1 & 0xf0;
	temp2= temp2 << 0;
	wr_cn();
	temp2= temp1 & 0x0f;
	temp2= temp2 << 4;
	wr_cn();
	delay_lcd(1000);
	return;
}

void wr_cn(void)
{
	clear_ports();
	LPC_GPIO0->FIOPIN=temp2;
	LPC_GPIO0->FIOCLR=RS_CTRL;
	LPC_GPIO0->FIOSET=EN_CTRL;
	delay_lcd(25);
	LPC_GPIO0->FIOCLR=EN_CTRL;
	return;
}

void lcd_data(void)
{
	temp2= temp1 & 0xf0;
	temp2= temp2 << 0;
	wr_dn();
	temp2= temp1 & 0x0f;
	temp2= temp2 << 4;
	wr_dn();
	delay_lcd(1000);
	return;
}

void wr_dn(void)
{
	clear_ports();
	LPC_GPIO0->FIOPIN=temp2;
	LPC_GPIO0->FIOSET=RS_CTRL;
	LPC_GPIO0->FIOSET=EN_CTRL;
	delay_lcd(25);
	LPC_GPIO0->FIOCLR=EN_CTRL;
	return;
}

void delay_lcd(unsigned int r1)
{
	unsigned int r;
	for(r=0;r<r1;r++);
	return;
}

void clr_disp(void)
{
	temp1=0x01;
	lcd_com();
	delay_lcd(10000);
	return;
}

void clear_ports(void)
{
	LPC_GPIO0->FIOCLR=DT_CTRL;
	LPC_GPIO0->FIOCLR=RS_CTRL;
	LPC_GPIO0->FIOCLR=EN_CTRL;
	return;
}

void lcd_puts(unsigned char *buf1)
{
	unsigned int i=0;
	while(buf1[i]!='\0')
	{
		temp1=buf1[i];
		lcd_data();
		i++;
		if(i==16)
		{
			temp1=0xc0;
			lcd_com();
		}
	}
}