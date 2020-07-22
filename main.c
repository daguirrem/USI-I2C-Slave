/*
 * File:   main.c
 * Autor:  David A. Aguirre Morales - david.aguirre1598@outlook.com
 *
 * Fecha de creación:   23 de junio de 2020, 08:24 PM
 * Última modificación: 22 de julio de 2020
 *                      documentación
 * 
 * Objetivo:
 * PRUEBAS de implementación del protocolo I²C en modo esclavo para comunicarlo 
 * con otros MCU's
 * 
 * ESTADO:
 *  SOLO LEE O ESCRIBE UN REGISTRO A LA VEZ (IMPORTANTE)
 * 
 * FUTURAS ACTUALIZACIONES:
 *  IMPLEMENTACIÓN ESCRITURA Y LECTURA DE VARIOS REGISTROS CONSECUTIVOS SEGÚN EL 
 *  ESTANDAR.
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
 *  comprobar la lectura por parte de un maestro.
 *
 *  3.Comprobación de un registro específico con un valor dado para comprobar
 *  la escritura por parte del esclavo, en caso de que el registro correspinda
 *  a el valor, el PIN3 del puerto B conmuntará cada segundo y medio (500ms).
 */

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "i2c.h"

uint8_t x = 0;

int main(void) {
    
    usi_i2c_slave( 0x1F );                      /*1*/
    i2c_slave.registers[3] = 0xCA;              /*2*/
    
    DDRB |= ( 1<<PINB3 );                       /*Pin 3 como salida*/
        
    while (1) {
        
        _delay_ms(500);
        if(i2c_slave.registers[2] == 0x81){     /*3*/
            PORTB ^= (1<<PINB3);                /*Conmutación pin 3*/
        }
    }
}
