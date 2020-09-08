/*
* File:   i2c.c
* Autor:  David A. Aguirre Morales - david.aguirre1598@outlook.com
*
* Fecha de creación:   23 de junio de 2020, 08:33 PM
* Última modificación: 03 de septiembre de 2020
*			           i2c_write_internalData: 64 bits error corregido.
*
* Descripción :
*  Libreria para la implementación del periferico USI en modo I²C.
*  Declaración de funciones e interrupciones.
*
* Estado:
*  Aprobado.
*
* Pendiente:
*  Fix bug 0x20.
*/

/* REFERENCIAS:
*  Understanding I²C, Texas Instruments (https://www.ti.com/lit/an/slva704/slva704.pdf).
*  ATtiny45 DATASHEET, Atmel (https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2586-AVR-8-bit-Microcontroller-ATtiny25-ATtiny45-ATtiny85_Datasheet.pdf)
*/

/* GITHUB
* https://github.com/daguirrem/usi_i2c_slave
*/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "usi_i2c_slave.h"

struct i2c_slave_s{
    uint8_t direction;
    uint8_t status;     /*Status (Estado Actual)		*/
    uint8_t rdir;       /*Register direction (Dirección actual)	*/
    uint8_t ack;        /*ACK (Indicador de modo ACK)		*/
    uint8_t registers[I2C_SLAVE_SZ_REG];
};

static struct i2c_slave_s i2c_slave = {
    0,0,0,0,{}
};

/*Interrupciones*/
/*Interrupción por detección de START*/
ISR(USI_START_vect){
    /*Espere a que el modo START termine*/
    loop_until_bit_is_clear(I2CPN,SCLP);
    /*Mantener SCL*/
    I2CD |= ( 1<<SCLP );
    /*¿Repeated START?*/
    if (i2c_slave.status == 2) {
        /*Si, Vuelva al inicio para que lea de nuevo la dirección*/
        i2c_slave.status = 0;
    }
    else {
        /*No, prepare la interrupción por desborde*/
        USICR |= (1<<USIOIE);
        USIDR = 0;
    }
    /*Reinicio de todas la banderas y del contador*/
    USISR =  ~( (1<<USICNT3)|(1<<USICNT2)|(1<<USICNT1)|(1<<USICNT0) );
    /*Liberar SCL*/
    I2CD &= ~(( 1<<SCLP ));
}

/*Interrupción por desborde de contador*/
ISR(USI_OVF_vect){

    /*¿Modo ACK? (¿Estoy en el bit correspondiente al ACK?)*/
    if(i2c_slave.ack){

        /*¿NACK o ACK? (por parte del maestro)*/
        if(i2c_slave.status == 5){
            if ( bit_is_clear(I2CPN,SDAP)) {
                /*En caso de ACK, prepare el siguiente envío del registro*/
                i2c_slave.status = 4;
                I2CD |=  ( 1<<SDAP );
                i2c_slave.rdir++;
                loop_until_bit_is_clear(I2CPN,SCLP);
            }
            else {
                /*En caso de NACK, termine la trasmisión*/
                loop_until_bit_is_clear(I2CPN,SCLP);
                i2c_slave.status = 0;
                i2c_slave.rdir = 0;
                I2CP &= ~(( 1<<SDAP ));
                I2CD &= ~(( 1<<SDAP ));
                USICR &= ~(1<<USIOIE);
            }
        }
        /*Modo recepción de datos (POST ACK)*/
        if(i2c_slave.status == 3) {
            /*Mantener SCL en bajo*/
            I2CD |=  ( 1<<SCLP );
            /*¿Stop?*/
            if(bit_is_set(USISR,USIPF)){
                /*Si, Detenga la trasmisión*/
                i2c_slave.rdir = 0;
                i2c_slave.status = 0;
                USICR &= ~(1<<USIOIE);
            }
            else {
                /*No, Prepare el siguiente registro*/
                i2c_slave.status=2;
                i2c_slave.rdir++;
            }
            /*Liberar SDA*/
            I2CD &= ~(( 1<<SDAP ));
        }
        /*Modo envío de datos (PRE)*/
        else if ( i2c_slave.status == 4) {
            /*Mantener SCL en bajo*/
            I2CD |=  ( 1<<SCLP );
            /*Cargue el registro de salido con los datos*/
            USIDR = i2c_slave.registers[i2c_slave.rdir];
            /*SDA como salida, para envío*/
            I2CP |=  ( 1<<SDAP );
        }
        else {
            /*Mantener SCL en bajo*/
            I2CD |=  ( 1<<SCLP );
            /*Liberar SDA, para el resto de modos*/
            I2CD &= ~(( 1<<SDAP ));
        }

        /*Alterne el modo ACK*/
        i2c_slave.ack = 0;
        /*Reinicio de todas la banderas y del contador*/
        USISR =  ~( ( 1<<USICNT3 )|( 1<<USICNT2 )|( 1<<USICNT1 )|( 1<<USICNT0 ) );
    }
    else {
        /*Mantener SCL en bajo*/
        I2CD |=  ( 1<<SCLP );
        /*Reiniciar únicamente el contador*/
        USISR = ~USISR & ~( ( 1<<USICNT3 )|( 1<<USICNT2 )|( 1<<USICNT1 )|( 1<<USICNT0 ) );

        /*Lectura de direccion (Esclavo) y modo (Escribir o Leer)*/
        if(i2c_slave.status == 0){
            uint8_t wrrd = USIDR&0x1;	/*Escribir o leer*/
            uint8_t dire = USIDR>>1;	/*Dirección leída del maestro*/

            /*¿El maestro envió mi dirección?*/
            if(dire == i2c_slave.direction) {
                /*Compruebe si el maestro quiere escribir o leer*/
                if(wrrd == 1) {
                    /*Si quiere leer, active el modo envío de datos*/
                    i2c_slave.status = 4;
                }
                else {
                    /*Si no, lea el registro objetivo*/
                    i2c_slave.status++;
                }
                /*Prepare el modo ACK*/
                I2CD |=  ( 1<<SDAP );
                i2c_slave.ack = 1;
            }
        }
        /*Lectura de dirección de registro objetivo*/
        else if(i2c_slave.status == 1) {
            /*Guarde la dirección del registro objetivo*/
            i2c_slave.rdir = USIDR;
            /*Prepare el modo ACK*/
            I2CD |=  ( 1<<SDAP );
            i2c_slave.ack = 1;
            /*Prepare modo recepción de datos (PRE ACK)*/
            i2c_slave.status++;
        }
        /*Modo recepción de datos (PRE ACK)*/
        else if (i2c_slave.status == 2) {
            /*Guarde los datos enviados por el maestro en la dirección dada*/
            i2c_slave.registers[i2c_slave.rdir] = USIDR;
            /*Prepare el modo ACK*/
            I2CD |= ( 1<<SDAP );
            i2c_slave.ack = 1;
            /*Prepare modo recepción de datos (POST ACK)*/
            i2c_slave.status++;
        }
        /*Modo de envió de datos (PRE ACK)*/
        else if(i2c_slave.status == 4) {
            /*Prepare la interrupción al siguiente flanco de subida en SCL*/
            /*(Flanco correspondiente al ACK)*/
            USISR |= ( 1<<USICNT0 );	    /*14+1 = 15*/
            /*Modo ACK*/
            I2CD  &= ~( 1<<SDAP);
            i2c_slave.ack = 1;
            /*Prepare lectura de ACK o NACK*/
            i2c_slave.status++;
        }

        /*Si el modo ACK fue configurado inicialice el contador en 14*/
        /*para provocar una interrupción en el siguiente clock en SCL*/
        if(i2c_slave.ack) {
            USISR |= ( 1<<USICNT3 )|( 1<<USICNT2 )|( 1<<USICNT1 );
        }
        /*Limpieza buffer entrada*/
        USIDR    = 0;
        /*Limpieza banderas de interrupción*/
        USISR	|= ( 1<<USIOIF )|( 1<<USISIF ) ;
    }
    /*Liberar SCL*/
    I2CD &= ~( 1<<SCLP );
}

void i2c_slave_init(uint8_t dir){
    I2CP &= ~(( 1<<SDAP ) | ( 1<<SCLP ));   /*Configuración pines SDA y SCL*/
    I2CD &= ~(( 1<<SDAP ) | ( 1<<SCLP ));

    USICR =  ( 1<<USISIE )|                 /*Interrupción START*/
    ( 1<<USIWM1 )|                 /*Modo I²C*/
    ( 1<<USICS1 );                 /*con fuente de reloj externo*/

    USISR =  ( 1<<USISIF )|                 /*Limpieza de banderas*/
    ( 1<<USIOIF )|
    ( 1<<USIPF  )|
    ( 1<<USIDC  );

    i2c_slave.direction = dir;              /*Asignación de dirección*/
    sei();                                  /*Interrupciones globales*/
}

void i2c_slave_write_internalData
(size_t rDir, const i2c_data_t data,databits_t datatype){

    switch (datatype){
        default:
        #if defined(I2C_REG_8)
        case bit8:
        *((uint8_t*)(i2c_slave.registers+rDir)) = data;
        break;
        #endif /*defined(I2C_REG_8)*/

        #if defined(I2C_REG_16)
        case bit16:
        *((uint16_t*)(i2c_slave.registers+rDir)) = (data<<8) | (data>>8);
        break;
        #endif /*defined(I2C_REG_16)*/

        #if defined(I2C_REG_32)
        case bit32:
        *((uint32_t*)(i2c_slave.registers+rDir)) =
            ((data&0x000000FF)<<24)|((data&0xFF000000)>>24)|
            ((data&0x0000FF00)<< 8)|((data&0x00FF0000)>> 8);
        break;
        #endif /*defined(REG_32)*/

        #if defined(I2C_REG_64)
        case bit64:
        *((uint64_t*)(i2c_slave.registers+rDir)) =
            ((data&0x00000000000000FF)<<56)|((data&0xFF00000000000000)>>56)|
            ((data&0x000000000000FF00)<<48)|((data&0x00FF000000000000)>>48)|
            ((data&0x0000000000FF0000)<<24)|((data&0x0000FF0000000000)>>24)|
            ((data&0x00000000FF000000)<< 8)|((data&0x000000FF00000000)>> 8);
        break;
        #endif /*defined(REG_64)*/
    }
}

i2c_data_t i2c_slave_read_internalData (size_t rDir, databits_t datatype){
    i2c_data_t data;
    switch (datatype){
        default:
        #if defined(I2C_REG_8)
        case bit8:
        data = *((uint8_t*)(i2c_slave.registers+rDir));
        break;
        #endif /*defined(I2C_REG_8)*/

        #if defined(I2C_REG_16)
        case bit16:
        data = *((uint16_t*)(i2c_slave.registers+rDir));
        break;
        #endif /*defined(I2C_REG_16)*/

        #if defined(I2C_REG_32)
        case bit32:
        data = *((uint32_t*)(i2c_slave.registers+rDir));
        break;
        #endif /*defined(I2C_REG_32)*/

        #if defined(I2C_REG_64)
        case bit64:
        data = *((uint64_t*)(i2c_slave.registers+rDir));
        break;
        #endif /*defined(I2C_REG_64)*/
    }
    return data;
}

#if defined(I2C_REG_FL)
void i2c_slave_write_internalData_F (size_t rDir, const float data){
    /*__data_representation*/
    uint32f_t __data_r;
    __data_r._float = data;
    *((uint32_t*)(i2c_slave.registers+rDir)) =
        ((__data_r._uint32&0x000000FF)<<24)|((__data_r._uint32&0xFF000000)>>24)|
        ((__data_r._uint32&0x0000FF00)<< 8)|((__data_r._uint32&0x00FF0000)>> 8);
}
float i2c_slave_read_internalData_F (size_t rDir){
    return *((double*)(i2c_slave.registers+rDir));
}

#if defined(DEBUG)
void i2c_slave_write_internalData_D_DEBUG (size_t rDir, const double data){
        *((double*)(i2c_slave.registers+rDir)) = data;
}
#endif /*defined(DEBUG)*/
#endif /*defined(I2C_REG_32)*/
