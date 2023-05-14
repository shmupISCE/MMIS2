#include "ESP8266.h"

// Potrebne AT komande:
char WIFI_ST_MODE_CMD[] = "AT+CWMODE=1\r\n";
char JOIN_AN_AP_CMD[] = "AT+CWJAP=\"MMIS2\",\"MiVolimoMMIS2\"\r\n";
char START_TCP_CONNECTION_CMD[] = "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n";
char SEND_DATA_LENGTH_CMD[] = "AT+CIPSEND=77\r\n";
char QUIT_AP_CMD[] = "AT+CWQAP";

STATUS_MSG_t ESP8266_set_mode(){
    char answ[128];
    
    UART2_Write((uint8_t*)WIFI_ST_MODE_CMD, strlen(WIFI_ST_MODE_CMD));          // Slanje AT+CWMODE=1 komande.
    while(UART2_WriteCountGet()!=0);                                            // Blokiranje koda dok se write bafer ne isprazni.
    
    CORETIMER_DelayMs(100);
    UART2_Read((uint8_t*)answ, sizeof(answ));                                   // Citanje odgovora ESP8266 modula na AT+CWMODE=1 komandu.
    
    char* find_ok = strstr(answ, "OK\r\n");                                     // Provera da li u odgovoru ESP8266 postoji niz karaktera "OK".
    
    if(find_ok!=NULL){                                                          
        return MODE_OK;                                                         // Ukoliko je ESP8266 odgovorio sa OK funkcija vraca MODE_OK vrednost.
    }
    else{
        return MODE_ERROR;                                                      // U suprotnom funkcija vraca MODE_ERROR vrednost.
    }
}

STATUS_MSG_t ESP8266_connect_to_AP(){
    char answ[128];
    
    UART2_Write((uint8_t*)JOIN_AN_AP_CMD, strlen(JOIN_AN_AP_CMD));              // Slanje AT+CWJAP komande - konekcija na AP.
    while(UART2_WriteCountGet()!=0);
    
    CORETIMER_DelayMs(500);
    UART2_Read((uint8_t*)answ, sizeof(answ));                                   
    
    char* find_error = strstr(answ, "+CWJAP=\r\n");                             // Provera da li u odgovoru ESP8266 na AT+CWJAP komandu postoji kod za neku od 4 moguce greske, oznacen kao +CWJAP=<error code>.
    
    if(find_error!=NULL){
        return WIFI_ERROR;
    }
    else{
        return WIFI_OK;
    }
}

STATUS_MSG_t ESP8266_TCP_connection(){
    char answ[128];
    
    UART2_Write((uint8_t*)START_TCP_CONNECTION_CMD, strlen(START_TCP_CONNECTION_CMD));  // Slanje AT+CIPSTART komande.
    while(UART2_WriteCountGet()!=0);
    
    CORETIMER_DelayMs(100);
    UART2_Read((uint8_t*)answ, sizeof(answ));
    
    char* find_ok = strstr(answ, "OK\r\n");                                     // Provera da li je TCP konekcija ok.
    char* find_connected = strstr(answ, "ALREADY CONNECTED\r\n");               // Provera da li je TCP konekcija vec otvorena.
    
    if(find_ok!=NULL){
        return TCP_OK;
    }
    else if(find_connected){
        return TCP_ALREADY_CONNECTED;
    }
    else{
        return TCP_CLOSED;                                                      
    }
}
STATUS_MSG_t ESP8266_cipsend(){
    char answ[128];
    
    UART2_Write((uint8_t*)SEND_DATA_LENGTH_CMD, strlen(SEND_DATA_LENGTH_CMD));  // Slanje AT+CIPSEND komande.
    while(UART2_WriteCountGet()!=0);
    
    CORETIMER_DelayMs(100);
    UART2_Read((uint8_t*)answ, sizeof(answ));
    
    char* find_ok = strstr(answ, "OK\r\n");                                     // Provera da li je ESP8266 odgovorio sa OK i da li je moguce poslati podatke.
    if(find_ok!=NULL){
        return CIPSEND_OK;
    }
    else{
        return CIPSEND_CLOSED;
    }
}

STATUS_MSG_t ESP8266_data_to_thingspeak(char* temperature){

    char answ[128];
    char write_a_channel_feed[80];
    
    strcpy(write_a_channel_feed,"GET https://api.thingspeak.com/update?api_key=27U2W5I4WH4060PD&field1=");  // Kopiranje karaktera "GET https..." u deo memorije koji zauzima niz write_a_channel_feed. 

    strcat(write_a_channel_feed,temperature);                                   // Konkatenacija stringa write_a_channel_feed i stringa sa podatkom o ocitanoj temperaturi.
    strcat(write_a_channel_feed,"\r\n");                                        // Konkatenacija write_a_channel_feed+temperatura sa stringom "\r\n".
            
    UART2_Write((uint8_t*)write_a_channel_feed, strlen(write_a_channel_feed));  // Slanje podatka o temperaturi na ThingSpeak.
    while(UART2_WriteCountGet()!=0);
    
    CORETIMER_DelayMs(100);
    UART2_Read((uint8_t*)answ, sizeof(answ));
    
    char* find_ok = strstr(answ, "Recv");                                       // Provera da li su podaci poslati. 

    if(find_ok!=NULL){
        return SEND_OK;
    }
    else{
        return SEND_ERROR;
    }
}