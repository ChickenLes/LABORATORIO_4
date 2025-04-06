//-----------------------------------------------
// UNIVERSIDAD DEL VALLE DE GUATEMALA
// IE2023: PROGRAMACION DE MICROCONTROLADORES
// PRELAB4.C
// AUTOR: ANTHONY ALEJANDRO BOTEO LÓPEZ
// PROYECTO: PRELABORATORIO 4
// HARDWARE: ATMEGA328P
// CREADO: 02/04/2025
// ULTIMA MODIFICACION: 04/04/2025
// DESCRIPCIÓN:
//-----------------------------------------------


#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/////////////////////////////////////VARIABLES/////////////////////////////
volatile uint8_t CONTADOR1 = 0x00;
volatile uint8_t PC0ANTI = 0;  //ANTIRREBOTE PC0
volatile uint8_t PC1ANTI = 0;  //ANTIRREBOTE PC1

/////////////////////////////////////SETUP/////////////////////////////////
void setup() {
	//CONFIGURANDO OUTPUTS
	DDRD = 0xFF;
	PORTD = 0x00;
	
	//CONFIGURANDO INPUTS
	DDRC = 0x00;
	PORTC = 0xFF;

	//CONFIGURACION PINCHANGE
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT8) | (1 << PCINT9);
	
	//CONFIGURACION TIMER0
	TCCR0A |= (1 << WGM01);				  //CTC
	TCCR0B |= (1 << CS01) | (1 << CS00);  // PRESCALER 64
	OCR0A = 249;                          //1ms
	TIMSK0 |= (1 << OCIE0A);              //INTERRUPCION POR COMPARACION
	
	
	sei(); 
}

//PIN CHANGE INTERRUPCION
ISR(PCINT1_vect) {
	if (!(PINC & (1 << PINC0))) {
		if (PC0ANTI == 0) {
			CONTADOR1++;
			PC0ANTI = 200; //200ms 
		}
	}

	if (!(PINC & (1 << PINC1))) {
		if (PC1ANTI == 0) {
			CONTADOR1--;
			PC1ANTI = 200; //200ms 
		}
	}
}

//INTERRUPCION POR COMPARACION TIMER0
ISR(TIMER0_COMPA_vect) {
	if (PC0ANTI > 0) PC0ANTI--;
	if (PC1ANTI > 0) PC1ANTI--;
}

int main(void) {
	setup();
	while (1) {
		PORTD = CONTADOR1; 
	}
}
