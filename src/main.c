/*
 * File:   main.c
 * Autor:  David A. Aguirre Morales - david.aguirre1598@outlook.com
 *
 * Fecha de creación:   23 de junio de 2020, 08:24 PM
 * Última modificación: 22 de julio de 2020
 *                      Modificación para pruebas de lectura o escritura de varios
 *			registros
 *
 * Objetivo:
 *  PRUEBAS de implementación del protocolo I²C en modo esclavo para comunicarlo
 *  con otros MCU's
 *
 * ESTADO:
 *  Funcionalidad probada.
 *
 * FUTURAS ACTUALIZACIONES:
 *  Elaboración de ejemplos de implementación
 *
 * MCU:
 *  ATtiny 45.
 * IDE:
 *  MPLAB 5.40, AVR TOOLCHAIN 3.62
 */

/* DESCRIPCIÓN DE PRUEBAS:
 *  1.Inicialización del periferico I²C con una direcicción específicada por el
 *  usuario.
 *
 *  2.Inicialización del un registro cualquiera con un dato cualquiera para
 *  comprobar la lectura por parte de un maestro, (2 para comprobar varios reg)
 *
 *  3.Comprobación de un registro específico con un valor dado para comprobar
 *  la escritura por parte del esclavo, en caso de que el registro correspinda
 *  a el valor, el PIN3 del puerto B conmuntará cada medio segundo (500ms).
 *
 *  ATtiny45 a 8MHz con una comunicación estable a 125KHz
 */

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "i2c_slave.h"

uint8_t x = 0;

int main(void) {
    
    /*1*/
    usi_i2c_slave( 0x1F );

    /*2*/
    /*Usa un byte de los registros disponibles*/
    usi_i2c_save_registers_s8(-12,0x1);
    /*Usa dos bytes de los registros disponibles (0x2-0x3)*/
    usi_i2c_save_registers_u16(0xCAB3,0x2);
    /*Usa cuatro bytes de los registros disponibles (0x4-0x7)*/
    usi_i2c_save_registers_u32(0xFFC90132,0x4);

    /*Pin 3 como salida*/
    DDRB |= ( 1<<PINB3 );
    uint16_t *i2c_16_in;
    while (1) {
	/*3*/
	i2c_16_in = (uint16_t*) (i2c_slave.registers+1);
        if(*i2c_16_in == 0x81F9) {
	    /*Conmutación pin 3*/
            PORTB ^= (1<<PINB3);
        }
	#ifndef DEBUG
	_delay_ms(500);
	#endif
    }
}