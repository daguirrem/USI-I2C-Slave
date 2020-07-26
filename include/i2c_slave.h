/*
 * File:   i2c.h
 * Autor:  David A. Aguirre Morales david.aguirre1598@outlook.com
 *
 * Fecha de creación:   23 de junio de 2020, 08:35 PM
 * Última modificación: 26 de julio de 2020
 *                      Documentación
 * 
 * Descripción:
 *  Libreria para la implementación del periferico USI en modo I²C.
 *  Descripción de funciones.
 * 
 * ESTADO:
 *  Funcional.
 * 
 * FUTURAS ACTUALIZACIONES:
 *  Ninguna
 */

/* ESTADO HEADER
 *  Completo.
 * 
 * FUTURAS ACTUALIZACIONES:
 *  Ninguna
 */

/* Ejemplo de implementación
 * ...
 * #include <usi_i2c_slave.h>
 * ...
 * int main(void){
 *  ...
 *  usi_i2c_slave(direction_of_slave);
 *  ...
 *  while(1){
 *	...
 *  }
 * 
 * }
 * 
 * + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
 * 
 * Si desea modificar registros del periferico:
 * 
 * ...
 * #include <usi_i2c_slave.h>
 * ...
 * int main(void){
 *  ...
 *  usi_i2c_slave(direction_of_slave);
 *  ...
 *  //Usa un byte de los registros disponibles (IMPORTANTE **)
 *  usi_i2c_save_registers_s8(signed char var,direction_to_save_data);
 * 
 *  //Usa dos bytes de los registros disponibles (IMPORTANTE **)
 *  usi_i2c_save_registers_u16(unsigned int var,direction_to_save_data);
 * 
 *  //Usa cuatro bytes de los registros disponibles (IMPORTANTE **)
 *  usi_i2c_save_registers_u32(unsigned long var,direction_to_save_data);
 *  
 *  etc.
 *  ...
 *  while(1) {
 *	...
 *  }
 * 
 * }
 * 
 * **Los registros internos del esclavo están organizados de byte en byte por dirección:
 * 
 *    i2c_slave_registers: [0x00] (1 byte)
 *		           [0x01] (1 byte)
 *			   [... ] ...
 *			   [0x7F] (1 byte) (0x7F == 127 CONFIGURABLE)
 * 
 *    Si se esribe una variable tipo DWORD (32bits) esta ocupará 4 bytes disponibles:
 *   
 *     usi_i2c_save_registers_u32(0xFFC90132,0x00); :
 *     i2c_slave_registers: [0x00] 0xFF (1 byte)
 *			    [0x01] 0xC9 (1 byte)
 *			    [0x02] 0x01 (1 byte)
 *			    [0x73] 0x32 (1 byte)
 * 
 * + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
 * 
 * Si desea leer constantemente registros (escritos por el maestro)
 * 
 * ...
 * #include <usi_i2c_slave.h>
 * ...
 * int main(void) {
 *  ...
 *  usi_i2c_slave(direction_of_slave);
 *  ...
 *  long int signed_dword_var = 0;
 *  unsigned char unsigned_byte_var = 0;
 *  unsigned int unsigned_word_var = 0;
 * 
 *  while(1){
 *	signed_dword_var = usi_i2c_read_registers_s32(direction_of_data_in_registers);
 *	unsigned_byte_var = usi_i2c_read_registers_u8(direction_of_data_in_registers);
 *	unsigned_word_var = usi_i2c_read_registers_u16(direction_of_data_in_registers);
 *  }
 * 
 * }
 * 
 * + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
 * 
 * -¿Cómo se envían los bytes leídos en mi I2C?
 *  
 *  EJEMPLO, EL MAESTRO DESEA LEER UN DWORD EN LOS REGISTROS 0x04 con el valor 0xFFC90132
 *  DE MI ESCLAVO CON DIRECCIÓN 0x1F
 * 
 *  el paquete I2C se eviará de la siguiente manera:
 * 
 *     0011111| 0| 0| 00000100| 0|    0011111| 1| 0| 11111111| 0| 11001001| 0| 00000001| 0| 00110010| 1|
 * | S|   DIRE| W| A|     DIRR| A| RS    DIRE| R| A|	 0xFF|A*|     0xC9|A*|	   0x01|A*|	0x32|N*| ST|
 * 
 *  NOTESE QUE SE ENVÍAN PRIMERO LOS BYTES MÁS SIGNIFICATIVOS (MSBY FIRST)--------------
 * 
 *  -¿Cómo se guardan los bytes escrios en mi I2C? (*Por un maestro)
 *  
 *  EJEMPLO, EL MAESTRO DESEA ESCRIBIR UN WORD EN LOS REGISTROS 0x01 CON EL VALOR 0x51AC
 *  DE MI ESCLAVO CON DIRECCIÓN 0x1F
 * 
 *  el paquete I2C se eviará de la siguiente manera:
 * 
 *     0011111| 0| 0| 00000001| 0| 10101100| 0| 01010001| 0|
 * | S|   DIRE| W| A|     DIRR| A|     0xAC| A|     0x51| A| ST|
 * 
 *  NOTESE QUE DEBE ENVÍAR PRIMERO LOS BYTES MENOS SIGNIFICATIVOS (LSBY FIRST)----------
 * 
 *  NOTA:
 * 
 *  S = START
 *  RS = REPEATED START
 *  ST = STOP
 * 
 *  DIRE = DIRECCIÓN ESCLAVO
 *  DIRR = DIRECCIÓN REGISTRO
 *  
 *  W = WRITE
 *  R = READ
 * 
 *  A = ACK
 *  N = NACK
 *  * = CONTROLADO POR EL MAESTRO (MASTER CONTROLLED)
 * 
 */

#ifndef _I2C_SLAVE_H_
#define	_I2C_SLAVE_H_
#include <stdint.h>

/* i2c_slave ESTRUCTURA
 *  direction: Dirección del esclavo.
 *  registers: Registros del periferico para ser leídos o escritos, hasta 128.
 */
struct i2c_slave{
    uint8_t direction	  ;
    uint8_t registers[128];
} i2c_slave;

/*-----------------------------------------------------------------------------------*/

/* usi_i2c_slave()
 * Descripción:
 *  Inicialización del periferico USI para trabajar el protocolo I²C en modo
 *  esclavo.		
 * Argumentos:
 *  -> dir: Direccion deseada del modo esclavo
 * Retorno:
 *  <- ninguno */
void usi_i2c_slave(uint8_t dir);

/* usi_i2c_save_registers_xy()
 * Descripción:
 *  Guarda un dato (data) en registros del I²C, dependiendo del tipo de dato (xy) dode:
 *   x: - u, unsigned (Números naturales y cero)
 *      - s, signed   (Números enteros)
 * 
 *   y: - 8,  1 byte  (byte)
 *      - 16, 2 bytes (word)
 *      - 32, 4 bytes (dword)
 * Argumentos:
 *  -> data: Datos que se desean almacentar
 *  -> dir:  Dirección de los registros I²C donde se desea almacenar.
 * Retorno:
 *  <- ninguno */
void usi_i2c_save_registers_u8 (uint8_t  data, uint8_t dir);
void usi_i2c_save_registers_u16(uint16_t data, uint8_t dir);
void usi_i2c_save_registers_u32(uint32_t data, uint8_t dir);
void usi_i2c_save_registers_s8 (int8_t   data, uint8_t dir);
void usi_i2c_save_registers_s16(int16_t  data, uint8_t dir);
void usi_i2c_save_registers_s32(int32_t  data, uint8_t dir);

/* usi_i2c_read_registers_xy()
 * Descripción:
 *  Lee un byte (data) en registros del I²C escritos por el maestro, dependiendo del tipo 
 *  de dato (xy) dode:
 *   x: - u, unsigned (Números naturales y cero)
 *      - s, signed   (Números enteros)
 * 
 *   y: - 8,  1 byte  (byte)
 *      - 16, 2 bytes (word)
 *      - 32, 4 bytes (dword)
 * Argumentos:
 *  -> dir: Dirección de los registros I²C donde se desea leer el dato.
 * Retorno:
 *  <- Dato almacenado en los registros */
uint8_t usi_i2c_read_registers_u8 (uint8_t dir);
uint16_t usi_i2c_read_registers_u16(uint8_t dir);
uint32_t usi_i2c_read_registers_u32(uint8_t dir);
int8_t usi_i2c_read_registers_s8 (uint8_t dir);
int16_t usi_i2c_read_registers_s16(uint8_t dir);
int32_t usi_i2c_read_registers_s32(uint8_t dir);

#endif	/* _I2C_SLAVE_H */