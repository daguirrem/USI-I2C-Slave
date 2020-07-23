/*
 * File:   i2c.c
 * Autor:  David A. Aguirre Morales - david.aguirre1598@outlook.com
 *
 * Fecha de creación:   23 de junio de 2020, 08:33 PM
 * Última modificación: 22 de julio de 2020
 *                      documentación, descripción de lo que se ha realizado y
 *                      futuras actualizaciones
 * 
 * Descripción :
 * Libreria para la implementación del periferico USI en modo I²C.
 * Declaración de funciones
 * 
 * ESTADO:
 *  SOLO LEE O ESCRIBE UN REGISTRO A LA VEZ (IMPORTANTE)
 * 
 * FUTURAS ACTUALIZACIONES:
 *  IMPLEMENTACIÓN ESCRITURA Y LECTURA DE VARIOS REGISTROS CONSECUTIVOS SEGÚN EL 
 *  ESTANDAR.
 */

#include "i2c.h"
#include <avr/interrupt.h>

#define SDAP  PIN0
#define SCLP  PIN2
#define I2CD  DDRB
#define I2CP  PORTB

static uint8_t status = 0;                  /*Status (Estado Actual)*/
static uint8_t data   = 0;                  /*Data*/
static uint8_t rdir   = 0;                  /*Register direction*/
static uint8_t ack    = 0;                  /*ACK (Indicador de modo ACK)*/

/*Interrupciones*/
ISR(USI_START_vect){
    I2CD |= ( 1<<SCLP );                    /*Mantener SCL*/
	
    if (status == 2) {                      /*Repeated START*/
        status = 0;                         /*Lea de nuevo la dirección*/
    }
    else {	
        USICR |= (1<<USIOIE);               /*Interrupcion por desborde*/
        USIDR = 0;
    }
	
    USISR =  ~((1<<USICNT3) |               /*Reinicio de banderas*/
               (1<<USICNT2) |               /*y el contador*/
               (1<<USICNT1) |
               (1<<USICNT0));
	
    I2CD &= ~(( 1<<SCLP ));                 /*Liberar SCL*/
}

ISR(USI_OVF_vect){
    
    if(ack){
	
	if(status == 5){		    /*¿NACK o ACK? (por parte del*/
	    if ( bit_is_clear(PINB,SDAP)) { /*maestro)*/
		status = 4;		    /*En caso de ACK, prepare el*/
		I2CD |=  ( 1<<SDAP );	    /*siguiente envío del registro*/
	    }
	    else {			    /*En caso de NACK, termine la */
		status = 0;		    /*trasmisión*/
		I2CP &= ~(( 1<<SDAP ));
		I2CD &= ~(( 1<<SDAP ));
		USICR &= ~(1<<USIOIE);
	    }
	}
	if ( status == 4) {                 /*Modo envío de datos (PRE)*/
	    I2CD |=  ( 1<<SCLP );           /*Mantener SCL en bajo*/
            USIDR = i2c_slave.registers[rdir];
            I2CP |=  ( 1<<SDAP );           /*SDA como salida, para envío*/
        }
        else {
	    I2CD |=  ( 1<<SCLP );           /*Mantener SCL en bajo*/
            I2CD &= ~(( 1<<SDAP ));         /*Liberar SDA, para el resto de */
        }                                   /*modos*/

        if(status == 3) {		    /*Escritura por parte del maestro*/
            i2c_slave.registers[rdir] = data;
            USICR &= ~(1<<USIOIE);
            status = 0;
        }
	
	ack = 0;
        USISR &= ~((1<<USICNT3) |           /*Reinicio de banderas*/
                   (1<<USICNT2) |           /*y el contador*/
                   (1<<USICNT1) |
                   (1<<USICNT0));
	
        
    }
    else {
	I2CD |=  ( 1<<SCLP );               /*Mantener SCL en bajo*/
	USISR = ~USISR &                    /*Reinicio contador*/
                ~(( 1<<USICNT3 )|
                  ( 1<<USICNT2 )|
                  ( 1<<USICNT1 )|
                  ( 1<<USICNT0 ));
	
        if(status == 0){                    /*Lectura de direccion (e) y modo*/
            uint8_t wrrd = USIDR&0x1;       /*Write / Read*/
            uint8_t tdir = USIDR>>1;        /*Temporal direction (readed)*/
            i2c_slave.cmode = wrrd;
            if((i2c_slave.cmode == 1) &&
               (i2c_slave.direction == tdir)) {
                    I2CD |= ( 1<<SDAP );    /*ACK*/
                    ack = 1;
                    status = 4;
            }
            else if(tdir == i2c_slave.direction){
                    I2CD |=  ( 1<<SDAP );   /*ACK*/
                    ack = 1;
                    status++;
            }
        }
        else if(status == 1){               /*Lectura de dirección de registro*/
            rdir = USIDR;                   /*register direction*/
            I2CD |=  ( 1<<SDAP );	    /*ACK*/
            ack = 1;
            status++;
        }
        else if(status == 2){               /*Lectura de los datos entrantes*/
            data  = USIDR;                   
            I2CD |= ( 1<<SDAP );            /*ACK*/
            ack = 1;
            status++;
        }
        else if(status == 4){               /*Modo de envió de datos (POST)*/
            status++;			    /*Prepare lectura de ACK o NACK*/
	    USISR |= ( 1<<USICNT0 );	    /*para siguiente flanco*/
	    I2CD  &= ~( 1<<SDAP);	    /*SDA como entrada*/
	    ack = 1;
        }
        if(ack) {
            USISR |= ( 1<<USICNT3 )|        /*Inicializacion contador en 14*/
                     ( 1<<USICNT2 )|
                     ( 1<<USICNT1 );
        }
        USIDR    = 0;                       /*Limpieza buffer entrada*/
        USISR	|= ( 1<<USIOIF ) |          /*Limpieza banderas*/
                   ( 1<<USISIF ) ;

    }
    I2CD &= ~( 1<<SCLP );		    /*Liberar SCL*/
}

void usi_i2c_slave(uint8_t dir){
    I2CP &= ~(( 1<<SDAP ) | ( 1<<SCLP ));   /*Configuracion pines SDA y SCL*/
    I2CD &= ~(( 1<<SDAP ) | ( 1<<SCLP ));

    USICR =  ( 1<<USISIE )|                 /*Interrupcion START*/
             ( 1<<USIWM1 )|                 /*Modo i2c*/
             ( 1<<USICS1 );                 /*con fuente de reloj externo*/

    USISR =  ( 1<<USISIF )|                 /*Limpieza de banderas*/
             ( 1<<USIOIF )| 
             ( 1<<USIPF  )| 
             ( 1<<USIDC  );

    i2c_slave.direction = dir;              /*Asignacion de dirección*/
    sei();                                  /*Interrupciones globales*/
}