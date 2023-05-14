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
#include <string.h>


#define ADC_Vref 3.3                                                            // Referentni napon A/D konvertora.
#define ADC_MAX_COUNT 4095                                                      // Usled 12-bitne rezolucije A/D konvertora maksimalna vrednost izlaza je 4095 (opseg vrednosi od 0 do 2^(n)-1, n=12).

uint16_t adc_out;                                                               // Promenljiva namenjena za podatke sa izlaza A/D konvertora.
float voltage;                                                                  // Promenljiva namenjena za podatke o vrednosti napona koja odgovara kvantima sa izlaza A/D konvertora.
volatile bool adc_out_ready=false;                                              // Volatile flag promenljiva - menja se iz ISR.
uint8_t voltage_buf[sizeof(voltage)];                                           // Niz u kome ce biti skladisteni bajtovi promenjive voltage - neophodno za slanje podataka u Data Visualizer.


// Callback za ADC interrupt: 
void ADCHS_ResultHandler(ADCHS_CHANNEL_NUM channel, uintptr_t context){
    adc_out = ADCHS_ChannelResultGet(ADCHS_CH4);                                // Ocitavanje rezultata A/D konverzije sa kanala 4.
    adc_out_ready = true;                                                       // Flag naznacava da je interrupt generisan.
}

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    // Funkcija ADCHS_CallbackRegister omogucava registrovanje callback funkcije za interrupt na odredjenom kanalu ADC.
    // Kao ulazne parametre prima kanal po kome se vrsi A/D konverzija,
    // odgovarajucu callback funkciju za interrupt na datom kanalu, kao i callback context.
    ADCHS_CallbackRegister(ADCHS_CH4, ADCHS_ResultHandler, (uintptr_t)NULL);
    
    TMR3_Start();                                                               // Pokretanje TMR3 modula.
    
    while ( true )
    {
        if(adc_out_ready){                                                      // Ukoliko su podaci ocitani sa ADC...
            adc_out_ready = false;                                              // Resetovanje flag promenljive.
            voltage = (float)adc_out*ADC_Vref/ADC_MAX_COUNT;                    // Konverzija izlaza ADC u kvantima u napon u opsegu od 0 do Vref.
            memcpy(voltage_buf, &voltage, sizeof(voltage));                     // memcpy funkcija (string.h) kopira n bajtova (u ovom slucaju sizeof(voltage)) iz jednog dela memorije
                                                                                // pocevsi od adrese &voltage, u drugi deo, pocevsi od adrese voltage_buf (adresa prvog elementa niza).
                                
            // Prikaz napona u Data Visualizer-u:
            UART1_WriteByte(0x03);                                              // Pocetak poruke
            for(uint8_t i=0;i<sizeof(voltage);i++){
                UART1_WriteByte(voltage_buf[i]);                                // Slanje podatka o naponu bajt po bajt.
            }
            UART1_WriteByte(0xFC);                                              // Kraj poruke
            
            //printf("ADC out: %u, Voltage: %.2f\n", adc_out, voltage);
            
            if(voltage>0.3 && voltage <= 1.0){
                GPIO_PortWrite(GPIO_PORT_B, 0xFF00, 0x1100);                    // Funckija GPIO_PortWrite kao ulazne argumente prima port, masku i vrednost koju je potrebno upisati na dati port.
                                                                                // Drugi argument koji se prosledjuje funkciji  je maska
                                                                                // koja oznacava pinove na kojima se vrsi upis vrednosti.
                                                                                // Zeljeni pinovi su RB8-RB15, dakle maska je 0b 1111 1111 0000 0000 (0xFF00).
            }                                                                   // U ovom slucaju je potrebno ukljuciti diode RB8 i RB12 (0b 0001 0001 0000 0000 tj. 0x1100).
            else if(voltage>1.0 && voltage <= 2.0){
                GPIO_PortWrite(GPIO_PORT_B, 0xFF00, 0x3300);                    // Ukljuciti diode RB8, RB9, RB12 i RB13 (0b 0011 0011 0000 0000).
            }
            else if(voltage>2.0 && voltage <= 3.0){
                GPIO_PortWrite(GPIO_PORT_B, 0xFF00, 0x7700);                    // Ukljuciti diode RB8, RB9, RB10, RB12, RB13 i RB14 (0b 0111 0111 0000 0000).
            }
            else if(voltage>3.0){
                GPIO_PortWrite(GPIO_PORT_B, 0xFF00, 0xFF00);                    // Ukljuciti sve diode od RB8 do RB15 (0b 1111 1111 0000 0000).
            }
            else{
                GPIO_PortClear(GPIO_PORT_B, 0xFF00);                            // Upisati 0 na sve pinove oznacene maskom 0xFF00 (RB8-RB15).
            }
            
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

