#include "stdint.h"
#include "C:/Keil/Labware/inc/tm4c123gh6pm.h" 
void SystemInit(){}
void init()
{
	SYSCTL_RCGCGPIO_R |= 0x20;
	while((SYSCTL_PRGPIO_R & 0x20) == 0){}
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	GPIO_PORTF_CR_R = 0xFF;
	GPIO_PORTF_DIR_R |= 0x0E;
	GPIO_PORTF_AFSEL_R = 0;
	GPIO_PORTF_DEN_R |= 0x0E;
	GPIO_PORTF_AMSEL_R = 0;
}
int main() 
{
  unsigned int uart0_var = 0x11484;
	int pass[4] = {1,2,5,4};//correct password from guest
  init();//using the same init function of the compilation code
  while (1) 
  {
    if ((uart0_var & 0x03) == 0x0) //Occupied state
    {
      if (((uart0_var & 0x3C) >> 2) == pass[0] && ((uart0_var & 0x3C0) >> 6) == pass[1] &&
        ((uart0_var & 0x3C00) >> 10) == pass[2] && ((uart0_var & 0x3C000) >> 14) == pass[3])
        GPIO_PORTF_DATA_R |= 0x0C; //unlock door and led is green
      else //wrong password
        GPIO_PORTF_DATA_R |= 0x02; //Led is Red and door locked
    }
    else if ((uart0_var & 0x03) == 0x01) //Room_Cleaning  state
      GPIO_PORTF_DATA_R |= 0x0C;
    else if ((uart0_var & 0x03) == 0x02) //Free state
      GPIO_PORTF_DATA_R |= 0x02; //Led is Red and door locked
  }
}
