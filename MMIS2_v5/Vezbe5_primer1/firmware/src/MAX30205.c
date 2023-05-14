#include "MAX30205.h"
#include "peripheral/i2c/master/plib_i2c2_master.h"

void MAX30205_init(){
    static uint8_t wr_data[2];                                                  // Niz podataka za slanje MAX30205.
    
    wr_data[0] = MAX30205_CONFIGURATION;                                        // Adresa registra u koji je potrebno upisati podatke - Configuration registar.
    wr_data[1] = 0x00;                                                          // Podaci koji se upisuju u registar na prosledjenoj adresi - sve 0 za osnovni mod rada.
    I2C2_Write(MAX30205_SLAVE_ADDR, wr_data, 2);                                // Slanje podataka - slave adresa, adresa registra, podaci.
    while(I2C2_IsBusy());
}
void MAX30205_hyst_config(uint16_t t_hyst){
    static uint8_t wr_data[3];                                                  // Niz podataka za slanje.
                                                                                // Thyst registar je 16-bitni - pored adrese registra saljemo i dva bajta podataka, dakle niz ima tri elementa.
    wr_data[0] = MAX30205_THYST;                                                // Adresa registra u koji se upisuju podaci - Thyst.
    wr_data[1] = t_hyst>>8;                                                     // Most significant byte podataka koji upisujemo u Thyst registar.
    wr_data[2] = t_hyst;                                                        // Least significant byte podataka koji upisujemo u Thyst registar.
    I2C2_Write(MAX30205_SLAVE_ADDR, wr_data, 3);                                // Slanje podataka - slave adresa, adresa registra, MSB podataka, LSB podataka.
    while(I2C2_IsBusy());
}
void MAX30205_os_config(uint16_t t_os){
    static uint8_t wr_data[3];
    
    wr_data[0] = MAX30205_TOS;
    wr_data[1] = t_os>>8;
    wr_data[2] = t_os;
    I2C2_Write(MAX30205_SLAVE_ADDR, wr_data, 3);
    while(I2C2_IsBusy());
}
void MAX30205_read_temperature(MAX30205_sens_t* sens){
    static uint8_t rd_data[2];                                                  // Niz u koji ce biti smestena dva bajta podataka procitana iz Temperature registra.
    static uint8_t wr_data;                                                     // Promenljiva koja ce imati vrednost adrese Temperature registra.
    
    static int16_t temp;                                                        
    
    float temperature;
    
    wr_data = MAX30205_TEMPERATURE;
    
    I2C2_WriteRead(MAX30205_SLAVE_ADDR, &wr_data, 1, rd_data, 2);               // Slanje podataka slave adresa, adresa registra, a zatim citanje podataka o temperaturi.
                                                                                // Prvi podatak koji se ocitava je most significant byte i on se smesta u rd_data[0], a zatim se ocitava least significant byte i on se smesta u rd_data[1].
    temp = (int16_t)(rd_data[0]<<8|rd_data[1]);                                 // Oznacena celobrojna vrednost temperature, dobijena na osnovu high i low bajta podataka procitanih iz Temperature registra.
    
    temperature = temp*0.00390625;                                              // Float vrednost temperature u stepenima Celzijusa, dobijena mnozenjem vrednosti iz Temperature registra i vrednosti koju ima LSB u datom registru.
    
    sens->temp_high_byte = (int8_t)rd_data[0];                                  // Cuvanje podataka na strukturi - high bajt, low bajt i float vrednost temperature.
    sens->temp_low_byte = (int8_t)rd_data[1];
    sens->temp = temperature;
}