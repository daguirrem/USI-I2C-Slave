# USI_I2C_Slave

Implementación del periferico USI de los ATtiny compatibles en modo I²C - Esclavo, Esta librería provee el funcionamiento básico del periférico, y un ejemplo de implementación.

## Empezando

Para implementar la librería en su proyecto, se debe copiar los archivos /include/usi_i2c_slave.h y /src/usi_i2c_slave.c en la raíz del proyecto, y añadir la carpeta "include" en los directorios del compilador.

### Prerequisitos

MPLAB X o Atmel Studio 7

AVR Toolchain o XC8 2.0 ++

MCU Compatible con USI de ATMEL

### usi_i2c_slave.h

Configurar el modo I²C modificando los pines y puertos usados por el periférico USI en el respectivo MCU (ver datasheet)

```c
#define SDAP  PIN0		/*#PIN correspondiente al SDA en el puerto*/
#define SCLP  PIN2		/*#PIN correspondiente al SCL en el puerto*/

#define I2CPN PINB		/*Registro PINx donde está el periférico I²C*/
#define I2CD  DDRB		/*Registro DDRx donde está el periférico I²C*/
#define I2CP  PORTB		/*Registro PORTx donde está el periférico I²C*/
```
y configurar el tamaño de registros internos (cantidad de direcciones) que va a manejar el modo esclavo

```c
#define I2C_SLAVE_SZ_REG number_of_registers (Eg. 100)
```

#### Ejemplo de implementación

Ejemplo para ATtiny45

En el archivo principal se debe incluir el header con la función de inicialización
```c
...
#include <usi_i2c_slave.h>
...
int main(void){
    ...
    i2c_slave_init(direction_of_slave);
    ...
    while(1){
    ...
    }

}
```
#### Si desea modificar registros del periferico:

```c
...
#include <usi_i2c_slave.h>
...
int main(void){
...
    i2c_slave_init(direction_of_slave);
    ...
    //Usa un byte de los registros disponibles (IMPORTANTE **)
    i2c_slave_write_internalData(target_direction_in_registers, signed_or_unsigned_char_var,bit8);
    //Usa dos bytes de los registros disponibles (IMPORTANTE **)
    i2c_slave_write_internalData(target_direction_in_registers, signed_or_unsigned_int_var,bit16);
    //Usa cuatro bytes de los registros disponibles (IMPORTANTE **)
    i2c_slave_write_internalData(target_direction_in_registers, signed_or_unsigned_long_var,bit32);
    //Para guardar un flotante (Usa cuatro registros disponibles)
    i2c_slave_write_internalData_F(target_direction_in_registers, float_var);
    etc.
    ...
    while(1) {
        ...
    }
 }
```
**Los registros internos del esclavo están organizados de byte en byte por dirección:
```c
    i2c_slave.registers: [0x00] (1 byte)
                         [0x01] (1 byte)
                         [... ] ...
                         [0x7F] (1 byte) (0x7F == 127 CONFIGURABLE)

                         donde: I2C_SLAVE_SZ_REG 128
```
   Si se esribe una variable tipo DWORD (32bits) esta ocupará 4 bytes disponibles:
```c
    -> i2c_slave_write_internalData(0x00,0xFFC90132,bit32);

    i2c_slave.registers: [0x00] 0xFF (1 byte)
                         [0x01] 0xC9 (1 byte)
                         [0x02] 0x01 (1 byte)
                         [0x03] 0x32 (1 byte)
```

#### Si desea leer constantemente registros (escritos por el maestro)

```c
...
#include <usi_i2c_slave.h>
...
int main(void) {
    ...
    i2c_slave_init(direction_of_slave);
    ...
    long int signed_dword_var = 0;
    unsigned char unsigned_byte_var = 0;
    unsigned int unsigned_word_var = 0;

    while(1){
        signed_dword_var  = (long int) i2c_slave_read_internalData(direction_of_data_in_registers,bit32);
        unsigned_byte_var = (unsigned char) i2c_slave_read_internalData(direction_of_data_in_registers,bit8);
        unsigned_word_var = (unsigned int) i2c_slave_read_internalData(direction_of_data_in_registers,bit16);
    }

}
```
#### ¿Cómo se envían los bytes leídos en mi I²C?

 Ejemplo:
 El maestro desea leer un dword de los registros: 0x04, con el valor: 0xFFC90132
 del esclavo con dirección: 0x1F

 el paquete I²C se eviará de la siguiente manera:
```c
    0011111| 0| 0| 00000100| 0|    0011111| 1| 0| 11111111| 0| 11001001| 0| 00000001| 0| 00110010| 1|
| S|   DIRE| W| A|     DIRR| A| RS    DIRE| R| A|     0xFF|A*|     0xC9|A*|     0x01|A*|     0x32|N*| ST|
```
 El esclavo envía al maestro primero los bytes más significativos

 #### ¿Cómo se guardan los bytes escrios en mi I²C? (*Por un maestro)

 Ejemplo:
 El maestro desea escribir un word en los registros: 0x01, con el valor: 0x51AC
 del esclavo con dirección: 0x1F

 el paquete I²C se eviará de la siguiente manera:
```c
    0011111| 0| 0| 00000001| 0| 10101100| 0| 01010001| 0|
| S|   DIRE| W| A|     DIRR| A|     0xAC| A|     0x51| A| ST|
```
 El maestro debe enviar primero los bytes menos significativos

 #### NOTA:
 ```
 S = START
 RS = REPEATED START
 ST = STOP

 DIRE = DIRECCIÓN ESCLAVO
 DIRR = DIRECCIÓN REGISTRO

 W = WRITE
 R = READ

 A = ACK
 N = NACK
 * = CONTROLADO POR EL MAESTRO (MASTER CONTROLLED)
 ```

 Para mas información vea el archivo header: usi_i2c_slave.h

 Para más información técnica vea el archivo: usi_i2c_slave.c

## Autor

* **David Alejandro Aguirre Morales** - [daguirrem](https://github.com/daguirrem)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Bug List

* Un escaneo al bus I2C hará que el dispositivo responda a la dirección 0x20, sin embargo esto no afecta
el funcionamiento, gracias a [favoritelotus](https://github.com/favoritelotus) por el reporte; más información en [0x20 Bug](https://github.com/daguirrem/usi_i2c_slave/issues/1)

## Para tener en cuenta:

* Velocidades máximas de comunicación probadas:
  -100KHz a 8MHz
  -200KHz a 16MHz
* Esta librería hace uso de la interrupciones, por lo que las activa de manera global.
