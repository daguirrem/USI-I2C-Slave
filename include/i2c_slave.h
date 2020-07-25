/*
 * File:   i2c.h
 * Autor:  David A. Aguirre Morales david.aguirre1598@outlook.com
 *
 * Fecha de creación:   23 de junio de 2020, 08:35 PM
 * Última modificación: 25 de julio de 2020
 *                      Nuevos prototipos de funciones de lectura
 *						Actualización a completo
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
 *	 x: - u, unsigned (Números naturales y cero)
 *      - s, signed   (Números enteros)
 * 
 *	 y: - 8,  1 byte  (byte)
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
 *	 x: - u, unsigned (Números naturales y cero)
 *      - s, signed   (Números enteros)
 * 
 *	 y: - 8,  1 byte  (byte)
 *      - 16, 2 bytes (word)
 *      - 32, 4 bytes (dword)
 * Argumentos:
 *  -> dir:  Dirección de los registros I²C donde se desea leer el dato.
 * Retorno:
 *  <- Dato almacenado en los registros */
uint8_t usi_i2c_read_registers_u8 (uint8_t dir);
uint16_t usi_i2c_read_registers_u16(uint8_t dir);
uint32_t usi_i2c_read_registers_u32(uint8_t dir);
int8_t usi_i2c_read_registers_s8 (uint8_t dir);
int16_t usi_i2c_read_registers_s16(uint8_t dir);
int32_t usi_i2c_read_registers_s32(uint8_t dir);

#endif	/* _I2C_SLAVE_H */