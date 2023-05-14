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
#include "max30100.h"
#include "ESP8266.h"
// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    
    char temp[10]={0};                                                          // Niz karaktera u kome ce biti smesten podatak o izmerenoj temperaturi.
    
    MAX30100_sens_t senzor;                                                     // Struktura za cuvanje podataka sa senzora MAX30100.

    STATUS_MSG_t status=CWMODE;                                                 // Promenljiva koja menja vrednost na osnovu odgovora ESP8266 na AT komande.
                                                                                // Na osnovu vrednosti ove promenljive krecemo se kroz masinu stanja. Inicijalna vrednost joj je CWMODE - prvi korak je podesavanje moda rada ESP modula.
    /* Initialize all modules */
    SYS_Initialize ( NULL );
   
    // MAX30100 initialize - SpO2 + HR mode, I_red = 24 mA, I_ir = 37 mA, Fs = 50 Hz, PW = 1,6 ms, n_ADC = 16 bit
    MAX30100_init(MAX30100_MODE_SPO2_HR, MAX30100_LED_CURRENT_24mA,  MAX30100_LED_CURRENT_37mA, MAX30100_SAMPLING_RATE_50Hz, MAX30100_PULSE_WIDTH_1600uS_ADC_16);
    printf("MAX30100 init done\r\n");
    
    while ( true )
    {
        
        MAX30100_enable_temp_read();                                            // Postavljanje bita 6 u registru - omogucava ocitavanje temperature.
        MAX30100_read_temp_data(&senzor);                                       // Ocitavanje temperature.
        sprintf(temp, "%.2f", senzor.temperature);                              // Konverzija float vrednosti temperature u string, u formatu .2f.
                
        switch(status){
            case CWMODE:
                status = ESP8266_set_mode();                                    // Podesavanje moda rada ESP8266.
                printf("CWMODE\r\n");
                
                if(status==MODE_OK){                                            // Ukoliko je podesavanje moda uspesno potrebno je preci na sledeci korak - podesavanje WiFi konekcije.
                    status = CWJAP;
                    printf("CWMODE OK ----> CWJAP\r\n");
                }
                else{
                    status = CWMODE;                                            // Ukoliko je podesavanje moda neuspesno potrebno je vratiti se na prethodni korak - ponovno podesavanje moda rada.      
                    printf("CWMODE ERROR\r\n");
                }
                break;
            case CWJAP:
                CORETIMER_DelayMs(1000);
                status = ESP8266_connect_to_AP();                               // Konekcija na access point.
                printf("CWJAP\r\n");
                if(status==WIFI_OK){                                            // Ukoliko je konekcija uspesna potrebno je preci na sledeci korak - otvaranje TCP konekcije.
                    status = CIPSTART;
                    printf("CWJAP OK ----> CIPSTART\r\n");
                }
                else{
                    status = CWJAP;                                             // Ukoliko konekcija na AP nije uspela, potrebno je pokusati ponovo - povratak na prethodni korak.
                    printf("CWJAP ERROR\r\n");
                }
                break;
            case CIPSTART:
                status = ESP8266_TCP_connection();                              // Otvaranje TCP konekcije.
                printf("CIPSTART\r\n");
                
                if(status==TCP_CLOSED){                                         // Ukoliko je doslo do greske potrebno je vratiti se na prethodni korak i ponovo otvoriti TCP konekciju.
                    status = CIPSTART;
                    printf("TCP CLOSED\r\n");
                }
                else{                                                           // Ukoliko je konekcija uspela ili je vec bila otvorena prelazi se na sledeci korak - slanje podataka.
                    status = CIPSEND;
                    printf("TCP ALREADY CONNECTED/TCP OK ----> CIPSEND\r\n");
                }
                break;
            case CIPSEND:
                status = ESP8266_cipsend();                                     // Slanje AT+CIPSEND komande.
                printf("CIPSEND\r\n");
                if(status==CIPSEND_OK){                                         // Ukoliko je odgovor na AT+CIPSEND OK potrebno je poslati podatke.
                    status = SEND;
                    printf("CIPSEND OK ----> SEND\r\n");
                }
                else{                                                           // Ukoliko AT+CIPSEND komanda nije uspesno obradjena TCP konekcija ce biti zatvorena - potrebno je vratiti se na prethodni korak i ponovo otvoriti TCP konekciju.
                    status = CIPSTART;                  
                    printf("CIPSEND CLOSED\r\n");
                }
                break;
            case SEND:             
                status = ESP8266_data_to_thingspeak(temp);                      // Slanje podataka na ThingSpeak.
                printf("SEND\r\n");
                CORETIMER_DelayMs(10000);
                
                if(status==SEND_OK){                                            // Ukoliko su podaci uspesno poslati TCP konekcija ce biti zatvorena - potrebno je vratiti se na korak otvaranje TCP konekcije, otvoriti TCP konekciju i nastaviti sa slanjem podataka.
                    printf("SEND OK\r\n");
                }
                else{                                                           // Ukoliko su podaci uspesno poslati TCP konekcija ce biti zatvorena - potrebno je vratiti se na korak otvaranje TCP konekcije.
                    printf("SEND ERROR\r\n");
                }
                status = CIPSTART;
                break;
            default:
                status = CWMODE;
                printf("default\r\n");
                break;
        }
        CORETIMER_DelayMs(3000);
    }
  
    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}
