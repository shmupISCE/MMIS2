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
char msg[1];

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    GPIO_PortClear(GPIO_PORT_A, 0x000F);                                        // Postaviti vrednosti na pinovima RA0, RA1, RA2 i RA3 na 0 (maska 0x000F mapira bitove koji odgovaraju RA0-RA3).
    
    while ( true )
    {
        if(UART1_ReceiverIsReady()){                                            // Ukoliko je moguce ocitati podatak sa UART-a.
            UART1_Read(msg,1);                                                  // Ocitavanje jednog karaktera sa UART-a.

            UART1_Write(msg,1);                                                 // Slanje primljenog podatka nazad na UART.
        }
        switch(msg[0])                                                          // U odnosu na uneti karakter ukljucuje se dioda koja odgovara pinu RA0/RA1/RA2/RA3.
        {                                                                       // Funkcija GPIO_RAx_Set() postavlja stanje na pinu RAx na logicku 1. Pravi se pauza od 500 ms.
            case '0':
                GPIO_RA0_Set();      
                CORETIMER_DelayMs(500);
                break;
            case '1':
                GPIO_RA1_Set();
                CORETIMER_DelayMs(500);
                break;
            case '2':
                GPIO_RA2_Set();
                CORETIMER_DelayMs(500);
                break;
            case '3':
                GPIO_RA3_Set();
                CORETIMER_DelayMs(500);
                break;
            default:
                GPIO_PortClear(GPIO_PORT_A, 0x000F);   
                break;
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

