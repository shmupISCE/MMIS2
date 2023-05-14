/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************


// Tasteri d?ojstika na EasyPIC Fusion v7 razvojnoj plo?i:
// RA4 - gore
// RA5 - dole
// RA6 - levo
// RA7 - desno 


typedef enum {NONE, UP, DOWN, LEFT, RIGHT} button_pressed_t;                    // Definisan je enum tip button_pressed_t koji definise 
                                                                                // moguce vrednosti na osnovu tastera koji je pritisnut.

volatile button_pressed_t button = NONE;                                        // Definisana je promenljiva button tipa button_pressed_t, 
                                                                                // inicijalna vrednost je NONE (nijedan taster nije pritisnut).
                                                                                // Promenljiva je volatile - naznacava kompajleru da se vrednost promenljive
                                                                                // moze promeniti u bilo kom trenutku, u ovom slucaju od strane ISR eksternih interrupt-a, 
                                                                                // i kompajler ne treba da vrsi optimizaciju nad datom promenljivom.

//******************************************************************************
// Callback funkcije za eksterne interrupt-e na pinovima RA4, RA5, RA6 i RA7: 

void GPIO_RA4_IntHandle(GPIO_PIN pin, uintptr_t context)
{
    button = UP;                                                                // Ukoliko se generise interrupt za pin RA4 promenljiva button dobija vrednost UP.
}

void GPIO_RA5_IntHandle(GPIO_PIN pin, uintptr_t context)
{
    button = DOWN;                                                              // Ukoliko se generise interrupt za pin RA5 promenljiva button dobija vrednost DOWN.
}

void GPIO_RA6_IntHandle(GPIO_PIN pin, uintptr_t context)
{
    button = LEFT;                                                              // Ukoliko se generise interrupt za pin RA6 promenljiva button dobija vrednost LEFT.
}

void GPIO_RA7_IntHandle(GPIO_PIN pin, uintptr_t context)
{
    button = RIGHT;                                                             // Ukoliko se generise interrupt za pin RA7 promenljiva button dobija vrednost RIGHT.
}
//******************************************************************************

// Funkcija za upravljanje osvetljajem pinova na portu B u odnosu na taster koji je pritisnut tj. vrednost promenljive button:
void led_curtain_handle(button_pressed_t button){
    uint32_t maska = 0;
    uint32_t val;
    
    for(uint8_t i=0;i<4;i++){
        val = GPIO_PortLatchRead(GPIO_PORT_B);                                  // Vrednost na portu B
        
        if(button == LEFT){
            maska = 0x000F;                                                     // Ukoliko je pritisnut taster RA6 u prvom trenutku treba ukljuciti
                                                                                // diode RB0, RB1, RB2 i RB3 (vrednosti na portu B za donja 4 bita se postavljaju na 1, sve ostalo su 0 - 0x000F).
            
            val = val | (maska<<4*i);                                           // U svakom narednom koraku potrebno je ukljuciti sledece 4 diode
                                                                                // (vrednost sledeca cetiri bita treba postaviti na 1),
                                                                                // ali i zadrzati prethodno ukljucene diode (vrednost val treba da se menja: 0x000F -> 0x00FF -> 0x0FFF -> 0xFFFF).
        }
        else if (button == RIGHT){
            maska = 0xF000;           
            val = val | (maska>>4*i);                                           // Vrednost val treba da se menja: 0xF000 -> 0xFF00 -> 0xFFF0 -> 0xFFFF.
        }
        else if (button == UP){
            maska = 0x1111;                                                     // Ukoliko je pritisnut taster RA4 u prvom trenutku treba ukljuciti
                                                                                // diode RB0, RB4, RB8 i RB12 (0b0001000100010001 - 0x1111).
            val = val | (maska<<i);                                             // U svakom narednom koraku potrebno je ukljuciti i red dioda iznad prethodno ukljucenih dioda.
                                                                                // Vrednost val treba da se menja: 0x1111 -> 0x3333 -> 0x7777 -> 0xFFFF.
        }
        else if (button == DOWN){
            maska = 0x8888;
            val = val | (maska>>i);                                             // Vrednost val treba da se menja: 0x8888 -> 0xCCCC -> 0xEEEE -> 0xFFFF.
        }
        else{
            val = val;
        }
        GPIO_PortWrite(GPIO_PORT_B,0xFFFF,val);                                 // Upis nove (maskirane) vrednosti na port B
        CORETIMER_DelayMs(1000);                                                // na svaku 1 s. 
                                                                                // Funkcija GPIO_PortWrite kao ulazne argumente ocekuje port (GPIO_PORT_B), 
                                                                                // masku kojom se naznacava na kojim pinovima porta se vrsi upis (maska 0xFFFF u 
                                                                                // ovom slucaju oznacava upis na pinove RB0-RB15), i vrednost koja se upisuje na port (val).
    }
}
int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );

    // Funkcija GPIO_PinInterruptCallbackRegister omogucava registrovanje callback funkcije za interrupt na odredjenom pinu.
    // Kao ulazne parametre prima pin na koji se interrupt odnosi,
    // odgovarajucu callback funkciju za eksterni interrupt na datom pinu, kao i callback context.
    GPIO_PinInterruptCallbackRegister(GPIO_PIN_RA4, GPIO_RA4_IntHandle, (uintptr_t)NULL);
    
    // Funkcija GPIO_PinIntEnable omogucava upotrebu interrupt-a odredjene vrste na zadatom pinu.
    // Kao ulazne parametre prima pin na koji se interrupt odnosi i vrstu interrupt-a 
    // (GPIO_INTERRUPT_STYLE - GPIO_INTERRUPT_ON_MISMATCH, GPIO_INTERRUPT_ON_RISING_EDGE, GPIO_INTERRUPT_ON_FALLING_EDGE, GPIO_INTERRUPT_ON_BOTH_EDGES).
    GPIO_PinIntEnable(GPIO_PIN_RA4, GPIO_INTERRUPT_ON_MISMATCH);
    
    GPIO_PinInterruptCallbackRegister(GPIO_PIN_RA5, GPIO_RA5_IntHandle, (uintptr_t)NULL);
    GPIO_PinIntEnable(GPIO_PIN_RA5, GPIO_INTERRUPT_ON_MISMATCH);
    
    GPIO_PinInterruptCallbackRegister(GPIO_PIN_RA6, GPIO_RA6_IntHandle, (uintptr_t)NULL);
    GPIO_PinIntEnable(GPIO_PIN_RA6, GPIO_INTERRUPT_ON_MISMATCH);
    
    GPIO_PinInterruptCallbackRegister(GPIO_PIN_RA7, GPIO_RA7_IntHandle, (uintptr_t)NULL);
    GPIO_PinIntEnable(GPIO_PIN_RA7, GPIO_INTERRUPT_ON_MISMATCH);
    
    while ( true )
    {
        switch(button)
        {
            case NONE:
                GPIO_PortClear(GPIO_PORT_B,0xFFFFFFFF);
                break;
            case DOWN:
                led_curtain_handle(DOWN);
                button=NONE;
                break;
            case LEFT:
                led_curtain_handle(LEFT);  
                button=NONE;
                break;
            case RIGHT:
                led_curtain_handle(RIGHT);
                button=NONE;
                break;
            case UP:
                led_curtain_handle(UP);
                button=NONE;
                break;
            default:
                button = NONE;
                break;
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

