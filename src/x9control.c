#include <msp430.h>
#include <stdbool.h>

#define CS		BIT3
#define INC		BIT4
#define UD		BIT5
#define TX		BIT2
#define RX	  BIT1


bool UP = true;
bool DOWN = false;
bool PAR = true;
unsigned int Nsteps = 100;
unsigned char RXDATA = 0;


void __init_wheelchair(void);
void __init_UART(void);
void set_pot(unsigned int pos, bool save);
void step_pot(unsigned int steps, bool dir);
void desel_save(void);
void desel_notsave(void);

int main(void) {
	unsigned int xPos = 0;
	unsigned int yPos = 0;
	WDTCTL = WDTPW + WDTHOLD;			//deshabilita el timer whatchdog
	__init_wheelchair();
	__init_UART();
	P1SEL |= RX + TX;
	P1SEL2 |= RX + TX;			// habilita los pines 1, 2, 6 y 7 del puerto 1 con sus funciones secundarias (RX, TX, scl y sda)
	UCA0CTL1 &= ~UCSWRST;                     // Liberar el USCI para entrar en operacion
	IE2 |= UCA0RXIE;			//habilita interrupcion de transmision del USCI sincrono y de recepcion del usci asincrono
	__bis_SR_register(CPUOFF + GIE);          // apaga el cpu y habilita las interrupciones generales
	while (1)
	{

			if (PAR)
			{
				yPos = RXDATA;
				yPos = (yPos-47)*10;
				if (xPos == 10 && yPos ==100 )
				{
					P1OUT ^= BIT0 + BIT6;			// P1.0 and P1.6 set high
					set_pot(xPos,true);

				}


			}

			else
			{
				xPos = RXDATA;
				xPos = (xPos-47)*10;


			}


			__bis_SR_register(LPM0_bits);	// Enter LPM0, interrupts enabled

	}

}




//  Echo back RXed character, confirm TX buffer is ready first
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  RXDATA = UCA0RXBUF;
	PAR = !PAR;
	__bic_SR_register_on_exit(LPM0_bits);	// Wake-up CPU
}


void __init_UART(void){
	if (CALBC1_1MHZ==0xFF)					// If calibration constant erased
	{
		while(1);                               // do not load, trap CPU!!
	}
	DCOCTL = 0; // Select lowest DCOx and MODx settings<
	BCSCTL1 = CALBC1_1MHZ; // Set DCO
	DCOCTL = CALDCO_1MHZ;
	UCA0CTL1 |= UCSWRST;
	UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 104;                              // 1MHz/ 9615
  UCA0BR1 = 0;                              // 1MHz /9615
  UCA0MCTL = UCBRS0;               // Modulation UCBRSx = 1
}



void __init_wheelchair(void){
	P1DIR = BIT0 + CS + INC + UD + BIT6;     //Configurar pines del puerto 1 como salida
	P2DIR = BIT0 + BIT1;		//Configurar pines del puerto 2 como salida para ajustar en neutral el control (VS/2)
	P1OUT = 0;
	P2OUT = 0;
	P1OUT |= INC;			      // Activa pin inc del pot digital
	P2OUT |= BIT0 + BIT1;   // Activa las salidas del cto de control de silla a vs/2 a traves de la compuerta analogica
	__delay_cycles(1000000);
	P2OUT &= ~BIT0;
	P2OUT &= ~BIT1;

}

void set_pot(unsigned int pos, bool save)
{
	step_pot (Nsteps, DOWN); 			// Ajusta el wipper al minimo de resistencia dando 100 pasos hacia abajo
	step_pot (pos, UP);
	if (save){
		desel_save();
	}
	else {
		desel_notsave();
	}

}

void step_pot(unsigned int steps, bool dir)
{ P1OUT &= ~CS;
	if (dir==UP)
	{
		P1OUT |= UD;
	}
	else
	{
		P1OUT &= ~UD;
	}
	unsigned int i;
	for (i=0; i<steps; i++)
	{
		P1OUT &= ~INC;
		__delay_cycles(1);
		P1OUT |= INC;
		__delay_cycles(1);
	}
	__delay_cycles(100);
}

void desel_save(void)  				// desactiva el chip select guardando el wipper
{
	P1OUT |= CS;

}

void desel_notsave(void)
{
	P1OUT &= ~INC;
	P1OUT |= CS;
	P1OUT |= INC;

}
