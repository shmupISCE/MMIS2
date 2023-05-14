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
#include <math.h>
#include <string.h>

#include "max30100.h"
// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
int main ( void )
{
    MAX30100_sens_t senzor;
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    // Inicijalizacija MAX30100 i reset FIFO bafera:
    MAX30100_init(MAX30100_MODE_SPO2_HR, MAX30100_LED_CURRENT_17_4mA, MAX30100_LED_CURRENT_30_6mA, MAX30100_SAMPLING_RATE_50Hz, MAX30100_PULSE_WIDTH_1600uS_ADC_16);
    MAX30100_FIFO_init();
    
    while ( true )       
    {
       MAX30100_read_led_data(&senzor);                                         // Ocitavanje podataka sa dioda
        
        // Slanje podataka putem UART1 - prikaz vrednosti u Data Visualizer-u
        // Format: <0x03><high byte><low byte><0xFC>
        UART1_WriteByte(0x03);                                                  // Pocetak poruke
        UART1_WriteByte(senzor.ir_led>>8);                                      // High byte 16-bitne vrednosti sa IR diode 
        UART1_WriteByte(senzor.ir_led);                                         // Low byte 16-bitne vrednosti sa IR diode
        UART1_WriteByte(0xFC);                                                  // Kraj poruke
        
        CORETIMER_DelayMs(10);
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

