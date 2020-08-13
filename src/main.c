/*
 * File:   main.c
 * Autor:  David A. Aguirre Morales - david.aguirre1598@outlook.com
 *
 * Fecha de creación:   23 de junio de 2020, 08:24 PM
 * Última modificación: 20 de agosto de 2020
 *                      Fix librerías, y FUSES para Atmel Studio
 *
 * Objetivo:
 *  PRUEBAS de implementación del protocolo I²C en modo esclavo para comunicarlo
 *  con otros MCU's
 *
 * ESTADO:
 *  Funcionalidad probada.
 *
 * FUTURAS ACTUALIZACIONES:
 *  Ninguna.
 *
 * MCU:
 *  ATtiny 45.
 * IDE:
 *  MPLAB 5.40, AVR TOOLCHAIN 3.62
 */

/* DESCRIPCIÓN DE PRUEBAS:
 *  1.Inicialización del periferico I²C con una direcicción específicada por el
 *  usuario. (0x1F en este ejemplo)
 *
 *  2.Inicialización del un registro cualquiera con un dato cualquiera para
 *  comprobar la lectura por parte de un maestr. (Varios registros inicializados,
 *  para que el maestro pueda comprobar sus valores)
 *
 *  3.Comprobación de un registro específico con un valor dado para comprobar
 *  la escritura por parte del esclavo, en caso de que el registro corresponda
 *  a el valor, el PIN3 del puerto B conmuntará cada medio segundo (500ms).
 *  (El maestro debe escribir en el registro 0x8, el valor de -27128 (word) para que el
 *  led parpadee)
 *
 *  ATtiny45 a 8MHz con una comunicación estable a 100KHz
 *             16Mhz - 200KHz
 */

#define F_CPU 8000000UL

#include <stdint.h>
#include <avr/io.h>
#include <avr/fuse.h>
#include <util/delay.h>

#include "usi_i2c_slave.h"

FUSES = {
    /* LOW {SUT_CKSEL=INTRCOSC_8MHZ_6CK_14CK_64MS, CKOUT=CLEAR, CKDIV8=CLEAR}*/
    .low = 0xE2,
    /* HIGH {BODLEVEL=2V7, EESAVE=CLEAR, WDTON=CLEAR, SPIEN=SET, DWEN=CLEAR,
     * RSTDISBL=CLEAR} */
    .high = 0xDD,
    /*EXTENDED {SELFPRGEN=CLEAR}*/
    .extended = 0xFF,
};

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

    int16_t test = 0;
    while (1) {
	/*3*/
	test = usi_i2c_read_registers_s16(0x8);
        if(test == -27128) {
	    /*Conmutación pin 3*/
            PORTB ^= (1<<PINB3);
        }
	_delay_ms(500);
    }
}
