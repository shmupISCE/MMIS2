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
#include "definitions.h"
#include "MAX30205.h"                // SYS function prototypes
#include "bmp.h"
#include "fonts.h"
#include "ili9341.h"

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    font_t font = {20,38,font_lucida_sans_20x38};                               // Definisanje fonta za ispis teksta na displeju.
    MAX30205_sens_t sens;                                                       // Definisanje strukture za cuvanje podataka ocitanih sa senzora.
    char temp[10] = {0};                                                        // char niz za ispis temperature u degC na displeju.
    int8_t t;                                                                   // Promenljiva koja ce skladistit vrednost high bajta podatka o temperaturi ocitanog sa MAX30205.                                    
    int8_t tp;                                                                  // Promenljiva koja ce skladistiti vrednost high bajta podatka o temperaturi ocitanog sa MAX30205 u prethodnoj iteraciji.
                                                                                // Ova vrednost nam je neophodna za crtanje prave izmedju dve temperaturne tacke na grafiku.
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    MAX30205_init();
    ili9341_init();
    
    ili9341_clear_display(COLOR_WHITE);                                         // Brisanje ekrana i bojenje u belo.
    ili9341_draw_raw_image(0,0,bmp,317,234);                                    // Prikaz slike dimenzija 317x234, pocevsi od tacke (0,0).
                                                                                // Vrednosti piksela slike su skladistene u okviru bmp niza (definisan je u bmp.h fajlu, a dobijen je konverzijom bmp formata slike u C niz).
    CORETIMER_DelayMs(500);
    while ( true )
    {
        tp = 0;
        for(uint8_t i=0;i<11;i++){
            
            MAX30205_read_temperature(&sens);                                   // Ocitavanje temperature sa MAX30205 senzora.
            
            t = sens.temp_high_byte;                                            // Vrednost most significant (high) bajta podatka o temperaturi.
            
            ili9341_draw_filled_circle(25*i+22,72+3*t,3,COLOR_RED);             // Crtanje ispunjenog kruga ciji je centar u tacki (x,y), gde je x preskalirana tako da bude u koordinatnom pocetku nacrtanog grafika sa slike, 
                                                                                // a y oznacava ocitanu temperaturu, takodje preskaliranu tako da odgovara grafiku sa slike.
            if(i>0){                                                                    
                ili9341_draw_line(25*(i-1)+22, 72+3*tp, 25*i+22, 72+3*t, COLOR_RED, 1); // Prava izmedju tacaka prethodnog i trenutnog ocitavanja temperature.
            }
            
            sprintf(temp, "%.1f", sens.temp);                                   // Konverzija float vrednosti temperature u niz karaktera za ispis na displeju.
            ili9341_write_text(145,10,temp,COLOR_RED,&font);                    // Ispis teksta sa podatkom o izmerenoj temperaturi, pocevsi od tacke (145,10), definisanim fontom.
            
            CORETIMER_DelayMs(1000);
            
            ili9341_draw_filled_rectangle(141,15,91,34, COLOR_WHITE);           // Crtanje belog pravougaonika preko ispisanog teksta, kako bi u sledecoj iteraciji ponovo mogao da se ispise podatak o temperaturi.
            tp = t;                                                             // Vrednost temperature izmerena u datoj iteraciji postaje prethodna vrednost temperature.
        }   
    ili9341_clear_display(COLOR_WHITE);                                         // Brisanje displeja i bojenje u belo.
    ili9341_draw_raw_image(0,0,bmp,317,234);                                    // Prikaz slike na displeju.
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

