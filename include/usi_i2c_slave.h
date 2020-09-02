/*
 * File:   usi_i2c_slave.h
 * Autor:  David A. Aguirre Morales david.aguirre1598@outlook.com
 *
 * Fecha de creación:   23 de junio de 2020, 08:35 PM
 * Última modificación: 02 de septiembre de 2020
 *                      Actualización funciones escritura y lectura de
 *                      registros internos. (nuevos enums y defines)
 *                      Actualización i2c_init
 *                      i2c_slave ahora es privado
 *
 * Descripción:
 *  Libreria para la implementación del periferico USI en modo I²C.
 *  Descripción de funciones.
 *
 * ESTADO:
 *  Aprobado.
 *
 * PENDIENTE:
 *  Nada.
 */

/* Ejemplo de implementación
 * ver "README.md"
 */

/* GITHUB
 * https://github.com/daguirrem/usi_i2c_slave
 */

#ifndef _USI_I2C_SLAVE_H_
#define	_USI_I2C_SLAVE_H_

#include <stdint.h>
#include <stddef.h>

/*Configuración de pines y puertos usados por el periférico USI*/

#define SDAP  PIN0		/*#PIN correspondiente al SDA en el puerto*/
#define SCLP  PIN2		/*#PIN correspondiente al SCL en el puerto*/

#define I2CPN PINB		/*Registro PINx donde está el periférico I²C*/
#define I2CD  DDRB		/*Registro DDRx donde está el periférico I²C*/
#define I2CP  PORTB		/*Registro PORTx donde está el periférico I²C*/

/*Tamaño registros del periférico I²C*/
#define I2C_SLAVE_SZ_REG 100

/*--------------------------------------------------------------------------------*/
/*SISTEMA*/
/*Macros que definen los tipos de datos que el i2c va a usar en sus registros*/
/*Nota: comentar los que no se van a  usar*/
#define I2C_REG_8       /*Trabaja con registros de 8bits*/
#define I2C_REG_16      /*Trabaja con registros de 16bits*/
#define I2C_REG_32      /*Trabaja con registros de 32bits*/
//#define I2C_REG_64      /*Trabaja con registros de 64bits*/
//#define I2C_REG_FL      /*Trabaja con registros de 32bits en modo Flotante*/

#if defined(I2C_REG_FL) && !defined(I2C_REG_32)
#define I2C_REG_32
#endif /*defined(I2C_REG_FL)*/

/*Macros internos del sistema*/
#define rdata_c(v) uint##v##_t
#if defined(I2C_REG_64)
#define i2c_data_t rdata_c(64)
#elif defined(I2C_REG_32)
#define i2c_data_t rdata_c(32)
#elif defined(I2C_REG_16)
#define i2c_data_t rdata_c(16)
#elif defined(I2C_REG_8)
#define i2c_data_t rdata_c(8)
#endif /*defined(I2C_REG_64)*/

/* NOTA:
 * i2c_data_t obtiene el valor de uintXX_t dependiendo del máximo tipo de datos que
 * esté definido.
 * XX puede ser: 8,16, 32 o 64.
 */

/*--------------------------------------------------------------------------------*/
/*UNIONS*/

/* uint32d_u
 * Descripción:
 *  Localiza tipo de datos int32 y float en misma dirección
 *  de memoria para hacer operaciones a nivel de bit con flotantes.
 */
typedef union uint32d_u {
    uint32_t _uint32;
    double _float;
} uint32f_t;

/*--------------------------------------------------------------------------------*/
/*ENUMS*/

/* databits_e
 * Descripción:
 *  Provee cantidad de bytes en un tipo de dato.
 */
typedef enum databits_e {
    bit8  = 1,
    bit16 = 2,
    bit32 = 4,
    bit64 = 8,
} databits_t;

/*--------------------------------------------------------------------------------*/
/*FUNCIONES*/

/* i2c_slave_init()
 * Descripción:
 *  Inicialización del periferico USI para trabajar el protocolo I²C en modo
 *  esclavo.
 * Argumentos:
 *  -> dir: Direccion deseada del modo esclavo
 * Retorno:
 *  <- ninguno */
void i2c_slave_init(uint8_t dir);


/* i2c_slave_write_internalData()
 * Descripción:
 *  Prepara, dependendo del tipo de variable /datatype, el dato /data
 *  en la dirección /rDir interna del períferico para que pueda ser envíada de
 *  manera correcta a un maestro cuando se requiera.
 * Argumentos:
 *  -> rDir: dirección del registro interno del periférico
 *  -> data: variable que se va a almacenar
 *  -> datatype: tipo de datos de la variable /data (ver ENUM databits_e)
 * Retorno:
 *  <- Ninguno
 */
void i2c_slave_write_internalData
(size_t rDir, const i2c_data_t data, databits_t datatype);

#if defined(I2C_REG_FL)

/* i2c_slave_write_internalData_F()
 * Descripción:
 *  Variante de "i2c_slave_write_internalData()", la cual maneja una variable de
 *  tipo floante. (NO NECESITA ESPECIFICAR EL TIPO DE DATOS)
 */
void i2c_slave_write_internalData_F (size_t rDir, const float data);

#endif /*defined(I2C_REG_32)*/

/* i2c_slave_read_internalData()
 * Descripción:
 *  Realiza una lectura de una variable, dependiendo de su tipo de datos /datatype,
 *  escrita por un maestro en la dirección interna /rDir
 *      NOTA: No funciona para hacer una lectura de una variable escrita por el
 *      mismo MCU (usando i2c_slave_write_internalData)
 * Argumentos:
 *  -> rDir: dirección donde se encuentra la variable
 *  -> datatype: tipo de datos que se desea leer
 * Retorno:
 *  <- i2c_data_t, variable leída
 */
i2c_data_t i2c_slave_read_internalData (size_t rDir, databits_t datatype);

#if defined(I2C_REG_FL)

/* i2c_slave_read_internalData()
 * Descripción:
 *  Variante de "i2c_slave_read_internalData()", la cual maneja una variable de
 *  tipo floante. (NO NECESITA ESPECIFICAR EL TIPO DE DATOS)
 */
float i2c_slave_read_internalData_F (size_t rDir);

#endif /*defined(I2C_REG_32)*/

/*DEBUGDEBUGDEBUGDEBUGDEBUGDEBUGDEBUGDEBUGDEBUGDEBUGDEBUGDEBUGDEBUGDEBUGDEBUGDEBUG*/

#if defined(DEBUG) && defined (I2C_REG_FL)
void i2c_slave_write_internalData_D_DEBUG (size_t rDir, const double data);
#endif

#endif	/* _USI_I2C_SLAVE_H */
