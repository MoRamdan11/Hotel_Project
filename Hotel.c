#include "stdint.h"
#include "C:/Keil/Labware/inc/tm4c123gh6pm.h" //file of all registers define 
void SystemInit(){}

void init()
{
	SYSCTL_RCGCGPIO_R |= 0x20;
	while((SYSCTL_PRGPIO_R & 0x20) == 0)
	{
	}
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	GPIO_PORTF_CR_R = 0xFF;
	GPIO_PORTF_DIR_R |= 0x0E;
	GPIO_PORTF_AFSEL_R = 0;
	GPIO_PORTF_DEN_R |= 0x0E;
	GPIO_PORTF_AMSEL_R = 0;
}

void uart0_init()
{
	//UART_PART
	UART0_CTL_R = 0;
	SYSCTL_RCGCUART_R |= 0x01;
	SYSCTL_RCGCGPIO_R |= 0x01;
	while((SYSCTL_PRGPIO_R & 0x01) == 0)
	{
	}
	UART0_LCRH_R = 0x70;
	UART0_CTL_R = 0x200;
	UART0_CTL_R |= 0x01;
	//GPIO PART
	GPIO_PORTA_LOCK_R = 0x4C4F434B;
	GPIO_PORTA_CR_R |= 0x01;
	GPIO_PORTA_DIR_R &= 0xFE;
	GPIO_PORTA_AFSEL_R |= 0x01;
	GPIO_PORTA_PCTL_R |= 0x01;
	GPIO_PORTA_DEN_R |= 0x01;
	GPIO_PORTA_AMSEL_R &= 0xFE;
}

int keypad()
{
	SYSCTL_RCGCGPIO_R |= 0x18;
	while((SYSCTL_PRGPIO_R&0x18) != 0x18)
	{
	}
	//PORT D --> Rows
	GPIO_PORTD_LOCK_R = 0x4C4F434B;
	GPIO_PORTD_CR_R |= 0x0F;
	GPIO_PORTD_DIR_R |= 0x0F;//output
	GPIO_PORTD_AFSEL_R = 0;
	GPIO_PORTD_DEN_R |= 0x0F;
	GPIO_PORTD_AMSEL_R = 0;
	//PORT E --> Coloums
	GPIO_PORTE_LOCK_R = 0x4C4F434B;
	GPIO_PORTE_CR_R |= 0x0F;
	GPIO_PORTE_DIR_R = 0;//inputs
	GPIO_PORTE_AFSEL_R = 0;
	GPIO_PORTE_DEN_R |= 0x0F;
	GPIO_PORTE_AMSEL_R = 0;
	while((GPIO_PORTE_DATA_R & 0x0F) != 0)
	{
		if((GPIO_PORTE_DATA_R &0x0F) == 0x01)//C1
		{
			GPIO_PORTD_DIR_R |= 0;//D-->input
			GPIO_PORTE_DIR_R |= 0x0F;//E-->output
			while((GPIO_PORTD_DATA_R & 0x0F) != 0)
			{
				if((GPIO_PORTD_DATA_R & 0x0F) == 0x01)//R1
					return 1;
				else if((GPIO_PORTD_DATA_R & 0x0F) == 0x02)//R2
					return 4;
				else if((GPIO_PORTD_DATA_R & 0x0F) == 0x04)//R3
					return 7;
			}
		}
		else if((GPIO_PORTE_DATA_R &0x0F) == 0x02)//C2
		{
			GPIO_PORTD_DIR_R |= 0;//D-->input
			GPIO_PORTE_DIR_R |= 0x0F;//E-->output
			while((GPIO_PORTD_DATA_R & 0x0F) != 0)
			{
				if((GPIO_PORTD_DATA_R & 0x0F) == 0x01)//R1
					return 2;
				else if((GPIO_PORTD_DATA_R & 0x0F) == 0x02)//R2
					return 5;
				else if((GPIO_PORTD_DATA_R & 0x0F) == 0x04)//R3
					return 8;
				else if((GPIO_PORTD_DATA_R & 0x0F) ==0x08)//R4
					return 0;
			}
		}
		else if((GPIO_PORTE_DATA_R &0x0F) == 0x02)//C3
		{
			GPIO_PORTD_DIR_R |= 0;//D-->input
			GPIO_PORTE_DIR_R |= 0x0F;//E-->output
			while((GPIO_PORTD_DATA_R & 0x0F) != 0)
			{
				if((GPIO_PORTD_DATA_R & 0x0F) == 0x01)//R1
					return 3;
				else if((GPIO_PORTD_DATA_R & 0x0F) == 0x02)//R2
					return 6;
				else if((GPIO_PORTD_DATA_R & 0x0F) == 0x04)//R3
					return 9;
			}
		}
		else
			return -1;
	}
}

int Read_data()
{
	int res;
	while((UART0_FR_R & 0x10) == 0x10)
	{
	}
	res = UART0_DR_R;
	return (res & 0xFF);
}

int main()
{
	//uint32_t pass;
	uint32_t i;
	int pass;
	int uart_var;
	init();
	uart0_init();
	while(1)
	{
		if((UART0_DR_R & 0x03) == 0x0)//Occupied state
		{    
			for(i = 0; i < 4; i=i+1)
			{
				uart_var = Read_data();
				pass = keypad();
				while(pass != -1)
			  {
				 pass = keypad();
			  }
				if(pass != ((uart_var & 0xF0)>>4) )
					break;
				if(i == 4)
				{
					GPIO_PORTF_DATA_R |= 0x0C;//unlock door and led is green
				}
			}
		}
		else if((UART0_DR_R & 0x03) == 0x01)//Room_Cleaning state
		{
			GPIO_PORTF_DATA_R |= 0x0C;
		}
		else if((UART0_DR_R & 0x03) == 0x02)//Free state
			GPIO_PORTF_DATA_R |= 0x02;//Led is Red and door locked
	}
}
