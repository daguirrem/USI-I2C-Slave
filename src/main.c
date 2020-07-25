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

#include "i2c.h"

uint8_t x = 0;

void usi_i2c_save_registers_u8 (uint8_t data, uint8_t dir);
void usi_i2c_save_registers_u16(uint16_t data, uint8_t dir);
void usi_i2c_save_registers_u32(uint32_t data, uint8_t dir);
void usi_i2c_save_registers_s8 (int8_t data, uint8_t dir);
void usi_i2c_save_registers_s16(int16_t data, uint8_t dir);
void usi_i2c_save_registers_s32(int32_t data, uint8_t dir);


int main(void) {

    usi_i2c_slave( 0x1F );                      /*1*/

    usi_i2c_save_registers_u16(0xCAB3,0x3);
    usi_i2c_save_registers_u32(0xFFC90132,0x5);

//    i2c_slave.registers[3] = 0xCA;            /*2*/
//    i2c_slave.registers[4] = 0xB3;

    DDRB |= ( 1<<PINB3 );                       /*Pin 3 como salida*/
    uint16_t *i2c_16_in;
    i2c_16_in = (uint16_t*) (i2c_slave.registers+1);
    while (1) {
        if(*i2c_16_in == 0x81F9){		/*3*/
            PORTB ^= (1<<PINB3);                /*Conmutación pin 3*/
        }
	_delay_ms(500);
    }
}

void usi_i2c_save_registers_u8(uint8_t data, uint8_t dir){
    uint8_t * registers;
    registers = (uint8_t*) (i2c_slave.registers + dir);
    *registers = data;
}
void usi_i2c_save_registers_u16(uint16_t data, uint8_t dir){
    uint16_t * registers;
    registers = (uint16_t*) (i2c_slave.registers + dir);
    //Intercambiar posición de bytes
    *registers = ((data&0xFF)<<8)|((data&0xFF00)>>8);
}
void usi_i2c_save_registers_u32(uint32_t data, uint8_t dir){
    uint32_t * registers;
    registers = (uint32_t*) (i2c_slave.registers + dir);
    //Intercambiar posición de bytes
    *registers = ((data&0x000000FF)<<24)|((data&0xFF000000)>>24)|
		 ((data&0x0000FF00)<<8)|((data&0x00FF0000) >>8);
}

void usi_i2c_save_registers_s8(int8_t data, uint8_t dir){
    int8_t * registers;
    registers = (int8_t*) (i2c_slave.registers + dir);
    *registers = data;
}
void usi_i2c_save_registers_s16(int16_t data, uint8_t dir){
    int16_t * registers;
    registers = (int16_t*) (i2c_slave.registers + dir);
    //Intercambiar posición de bytes
    *registers = ((data&0xFF)<<8)|((data&0xFF00)>>8);
}
void usi_i2c_save_registers_s32(int32_t data, uint8_t dir){
    int32_t * registers;
    registers = (int32_t*) (i2c_slave.registers + dir);
    //Intercambiar posición de bytes
    *registers = ((data&0x000000FF)<<24)|((data&0xFF000000)>>24)|
		 ((data&0x0000FF00)<<8)|((data&0x00FF0000) >>8);
}


