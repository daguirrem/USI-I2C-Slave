# USI-I2C-Slave

Implementación del periferico USI de los ATtiny compatibles en modo I²C - Esclavo, Esta librería provee el funcionamiento básico del periférico, y un ejemplo de implementación.

## Empezando

Para implementar la librería en su proyecto, se debe copiar los archivos /include/usi_i2c_slave.h y /src/usi_i2c_slave.c en la raíz del proyecto, y añadir la carpeta "include" en los directorios del compilador.

### Prerequisitos

MPLAB X o Atmel Studio 7

AVR Toolchain o XC8 2.0 ++

MCU Compatible con USI de ATMEL

### usi_i2c_slave.h

Se crea una estructura donde se almacena la dirección del esclavo configurable, y la cantidad de registros que se desean manejar, la cantidad depende de la cantidad de memoria RAM que tenga el MCU, este valor determina la cantidad de direcciones que tendrá disponible el periférico.

```c
struct i2c_slave{
    uint8_t direction	  ;
    uint8_t registers[10]
} i2c_slave;

```
o

```c
struct i2c_slave{
    uint8_t direction	  ;
    uint8_t registers[256]
} i2c_slave;

```

#### Ejemplo de implementación

```c
...
#include <usi_i2c_slave.h>
...
int main(void){
    ...
    usi_i2c_slave(direction_of_slave);
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
    usi_i2c_slave(direction_of_slave);
    ...
    //Usa un byte de los registros disponibles (IMPORTANTE **)
    usi_i2c_save_registers_s8(signed_char_var,direction_to_save_data);

    //Usa dos bytes de los registros disponibles (IMPORTANTE **)
    usi_i2c_save_registers_u16(unsigned_int_var,direction_to_save_data);

    //Usa cuatro bytes de los registros disponibles (IMPORTANTE **)
    usi_i2c_save_registers_u32(unsigned_long_var,direction_to_save_data);
    
    etc.
    ...
    while(1) {
        ...
    }

}
```
**Los registros internos del esclavo están organizados de byte en byte por dirección:
```c
   i2c_slave_registers: [0x00] (1 byte)
                        [0x01] (1 byte)
                        [... ] ...
                        [0x7F] (1 byte) (0x7F == 127 CONFIGURABLE)
```
   Si se esribe una variable tipo DWORD (32bits) esta ocupará 4 bytes disponibles:
```c
    -> usi_i2c_save_registers_u32(0xFFC90132,0x00);
    
    i2c_slave_registers: [0x00] 0xFF (1 byte)
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
    usi_i2c_slave(direction_of_slave);
    ...
    long int signed_dword_var = 0;
    unsigned char unsigned_byte_var = 0;
    unsigned int unsigned_word_var = 0;

    while(1){
        signed_dword_var = usi_i2c_read_registers_s32(direction_of_data_in_registers);
        unsigned_byte_var = usi_i2c_read_registers_u8(direction_of_data_in_registers);
        unsigned_word_var = usi_i2c_read_registers_u16(direction_of_data_in_registers);
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

## Para tener en cuenta:

* Velocidades máximas de comunicación:
* -100KHz a 8MHz
* -200KHz a 16MHz
