//-----------------------------------------------
// UNIVERSIDAD DEL VALLE DE GUATEMALA
// IE2023: PROGRAMACION DE MICROCONTROLADORES
// LABORATORIO4.C
// AUTOR: ANTHONY ALEJANDRO BOTEO LÓPEZ
// PROYECTO: PRELABORATORIO 4
// HARDWARE: ATMEGA328P
// CREADO: 04/04/2025
// ULTIMA MODIFICACION: 04/04/2025
// DESCRIPCIÓN:
//-----------------------------------------------

////////////////////////LIBRERIAS ETC//////////////////////////////
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/////////////////////////////////////VARIABLES/////////////////////////////
volatile uint8_t CONTADOR1 = 0x00; //VALOR DEL CONTADOR
volatile uint8_t DISPLAY_VALUE = 0; //VALOR DEL DISPLAY
volatile uint8_t PC0ANTI = 0;  //ANTIRREBOTE PC0
volatile uint8_t PC1ANTI = 0;  //ANTIRREBOTE PC1
volatile uint8_t ESTADO_DISPLAY = 0;  // 0: UNIDADES, 1: DECENAS, 2: CONTADOR



const	uint8_t	TABLA[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

/////////////////////////////////////SETUP/////////////////////////////////
void setup() {
	//CONFIGURANDO OUTPUTS
	DDRD  = 0xFF;
	PORTD = 0x00;
	DDRB  = 0xFF;
	PORTB = 0x00;
	//CONFIGURANDO INPUTS
	DDRC  = 0x00;
	PORTC = 0xFF;
	PORTC &= ~(1 << PINC2);
}
void setup_pinchange(){
	//CONFIGURACION PINCHANGE
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT8) | (1 << PCINT9);
}

void  setup_timer0(){
	//CONFIGURACION TIMER0
	TCCR0A |= (1 << WGM01);				  //CTC
	TCCR0B |= (1 << CS01) | (1 << CS00);  // PRESCALER 64
	OCR0A = 249;                          //1ms
	TIMSK0 |= (1 << OCIE0A);              //INTERRUPCION POR COMPARACION
	
}

void  setup_ADC(){
	ADMUX   =	0;
	ADMUX	=	(1 << REFS0) | (1 << ADLAR) | (1 << MUX1);
	
	ADCSRA	=	0;
	ADCSRA	|=  (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADIE) | (1 << ADEN);
	ADCSRA	|= (1 << ADSC);
	
}

////////////////////////////////////////////MAIN////////////////////////////////////
int main(void) {
	setup_pinchange();
	setup_timer0();
	setup();
	setup_ADC();
	sei();

	while (1) {
	}
}

/////////////////////////////////////INTERRUPCION//////////////////////////////
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
	// MULTIPLEXADO DEL DISPLAY
	
	//SWITCH CASE CAMBIAR ENTRE ESTADOS DEL BOTON
	switch(ESTADO_DISPLAY){
		
		case 0: // UNIDADES
		PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2));  //Apagar todo
		PORTD = TABLA[DISPLAY_VALUE & 0x0F];
		PORTB |= (1 << PB0);  //ENCENDER UNIDADES
		break;
		
		case 1: // DECENAS
		PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2));
		PORTD = TABLA[(DISPLAY_VALUE >> 4) & 0x0F];
		PORTB |= (1 << PB1);  //ENCENDER DECENAS
		break;
		
		case 2: // CONTADOR
		PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2));
		PORTD = CONTADOR1;
		PORTB |= (1 << PB2);  //ENCENDER CONTADOR1
		break;
	}
	//AUMENTAR ESTADO MUESTREO DEL DISPLAY
	ESTADO_DISPLAY++;
	if(ESTADO_DISPLAY > 2) ESTADO_DISPLAY = 0;
	
	//COMPARACIÓN ENCENDER ALARMA
	if (DISPLAY_VALUE > CONTADOR1) {
		PORTB |= (1 << PB3) | (1 << PB4);  // ENCENDER PB3 y PB4
		} else {
		PORTB &= ~((1 << PB3) | (1 << PB4));  // APAGAR PB3 y PB4
	}

}

ISR(ADC_vect){
	
	DISPLAY_VALUE		=	ADCH;
	ADCSRA |= (1 << ADSC);
}
