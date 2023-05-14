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

volatile bool read_flag = false; 
volatile bool write_flag = false; 
char start_msg[] = "___START___\r\n";

// Callback funkcija za read interrupt:
void UART1_ReadCallback(uintptr_t context)
{
    read_flag = true;
}

// Callback funkcija za write interrupt:
void UART1_WriteCallback(uintptr_t context)
{
    write_flag = true;
}
int main ( void )
{
    char msg[1];
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    UART1_WriteCallbackRegister(UART1_WriteCallback, (uintptr_t)NULL);          // UARTn_WriteCallbackRegister funckija registruje callback funkciju za write interrupt.
    UART1_ReadCallbackRegister(UART1_ReadCallback, (uintptr_t)NULL);            // UARTn_ReadCallbackRegister funckija registruje callback funkciju za read interrupt.
    
    GPIO_PortClear(GPIO_PORT_A, 0x000F);                                        // Postavljenje vrednosti na pinovima RA3-RA0 na 0.

    UART1_Write(start_msg,sizeof(start_msg));                                   // Ispis start poruke - zapocinje UART prenos podataka.
    
    while ( true )
    {
        if(write_flag){                                                         // Ukoliko se desio write interrupt:
                                
            write_flag = false;                                                 // Reset flag promenljive.
            UART1_Read(msg, 1);                                                 // Citanje primljenih podataka.
        }
        
        else if(read_flag){                                                     // Ukoliko se desio read interrupt:
            read_flag = false;                                                  // Reset flag promenljive.
            UART1_Write(msg, 1);                                                // Ispis primljenog podatka.

            switch(msg[0])                                                      // Ukljucivanje diode RA0-RA3 u zavisnosti od unetog karaktera.
            {
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

    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

