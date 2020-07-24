/*
 * File:   i2c.h
 * Autor:  David A. Aguirre Morales david.aguirre1598@outlook.com
 *
 * Fecha de creación:   23 de junio de 2020, 08:35 PM
 * Última modificación: 24 de julio de 2020
 *						Eliminación CMODE de la estructura "i2c_slave"
 * 
 * Descripción:
 *  Libreria para la implementación del periferico USI en modo I²C.
 *  Descripción de funciones.
 * 
 * ESTADO:
 *  SOLO LEE O ESCRIBE UN REGISTRO A LA VEZ (IMPORTANTE)
 * 
 * FUTURAS ACTUALIZACIONES:
 *  IMPLEMENTACIÓN ESCRITURA Y LECTURA DE VARIOS REGISTROS CONSECUTIVOS SEGÚN EL 
 *  ESTANDAR.
 */

/* ESTADO HEADER
 *  DESCRIPCIÓN BÁSICA DE LA FUNCIÓN
 * 
 * FUTURAS ACTUALIZACIONES:
 *  DOCUMENTACIÓN
 */

#ifndef _I2C_H_
#define	_I2C_H_
#include <stdint.h>

/* i2c_slave ESTRUCTURA
 *  direction: Dirección del esclavo.
 *  registers: Registros del periferico para ser leídos o escritos, hasta 128.
 */
struct i2c_slave{
	uint8_t direction	  ;
	uint8_t registers[128];
} i2c_slave;

/*----------------------------------------------------------------------------*/

/* usi_i2c_slave()
 * Descripción:
 *  Inicialización del periferico USI para trabajar el protocolo I²C en modo
 *  esclavo.		
 * Argumentos:
 *  -> dir:	Direccion deseada del modo esclavo
 * Retorno:
 *  <- ninguno */
void usi_i2c_slave(uint8_t dir);

#endif	/* _I2C_H */
