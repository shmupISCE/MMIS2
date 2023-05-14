/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
#include "max30100.h"


// Citanje podatka o ID cipa iz Part ID registra:
uint8_t MAX30100_get_device_ID(){
        // static promenljive - pamcenje vrednosti koju je promenljiva imala pri prethodnom pozivu funkcije.
        static uint8_t wr[1];                                                   // Podatak koji saljemo - adresa registra.
        static uint8_t rd[1];                                                   // Podatak koji ocitavamo - sadrzaj registra na prosledjenoj adresi.
        wr[0]=MAX30100_PART_ID;                                                 // Adresa Part ID registra MAX30100.
        I2C2_WriteRead(MAX30100_SLAVE_ADDR, wr, 1, rd, 1);                      // Funkciji I2Cx_WriteRead kao ulazna argumente ocekuje slave adresu cipa,
                                                                                // pokazivac na write bafer, kolicinu podataka u write baferu, pokazivac na read bafer i kolicinu podataka koja ce biti ucitana u read bafer.
        while(I2C2_IsBusy());                                                   // Blokiranje koda dok traje prenos podataka putem I2C interfejsa.
        return rd[0];
}


void MAX30100_init(MAX30100_MODE_t mode, MAX30100_LED_CURRENT_t red_led_current, MAX30100_LED_CURRENT_t ir_led_current, MAX30100_SAMPLE_RATE_t sample_rate, MAX30100_LED_PULSE_WIDTH_t pulse_width){
    static uint8_t wr[2];                                                       // Podaci koje saljemo - niz od dva elementa, za adresu registra i za vrednost koja ce biti upisana u registar.
                                                                                // Ova dva podatka salju se jedan za drugim.
    
    // Podesavanje moda rada:
    wr[0]= MAX30100_MODE_CFG;
    wr[1] = mode;                                                               // Prosledjena vrednost moda upisuje se u Mode Configuration registar.
    I2C2_Write(MAX30100_SLAVE_ADDR, wr,2);                                      // I2Cx_Write funkcija kao ulazne argumente prima slave adrecu cipa, pokazivac na write bafer i kolicinu podataka u write baferu koju je potrebno poslati.
    while(I2C2_IsBusy());                        
                                            
    wr[1] = 0x00;                   
    // Podesavanje struje za RED i IR LED: 
    wr[0]= MAX30100_LED_CFG;                       
    wr[1] = (red_led_current<<4)|ir_led_current;                                // Vrednost red_led_current potrebno je upisati na bitove B7:B4, a vrednost ir_led_current na bitove B3:B0.
    I2C2_Write(MAX30100_SLAVE_ADDR, wr,2);         
    while(I2C2_IsBusy());
    
    wr[1] = 0x00;
    // Podesavanje SpO2 - sample rate, pulse width i ADC res:
    wr[0]= MAX30100_SPO2_CFG;
    wr[1] = (sample_rate<<2)|pulse_width;                                       // Vrednost koja odredjuje sample rate se upisuje na bitove B4:B2, a vrednost koja odredjuje pulse width na B1:B0.
    I2C2_Write(MAX30100_SLAVE_ADDR, wr,2);
    while(I2C2_IsBusy());
}

void MAX30100_FIFO_init(){
    static uint8_t wr[2];
    
    // Reset write pointer-a, overflow counter-a i read pointer-a
    // (postavljanje sadrzaja datih registara na 0):
    wr[0]= MAX30100_FIFO_WR_PTR ;
    wr[1] = 0x00;
    I2C2_Write(MAX30100_SLAVE_ADDR, wr,2);
    while(I2C2_IsBusy());
    wr[0]= MAX30100_FIFO_OVERFLOW_CNT;
    I2C2_Write(MAX30100_SLAVE_ADDR, wr,2);
    while(I2C2_IsBusy());
    wr[0]= MAX30100_FIFO_RD_PTR;
    I2C2_Write(MAX30100_SLAVE_ADDR, wr,2);
    while(I2C2_IsBusy());
}

void MAX30100_read_led_data(MAX30100_sens_t* sens){
    
    static uint8_t wr_data;
    static uint8_t tmp_reg[4];
    
    wr_data = MAX30100_FIFO_DATA;

    I2C2_WriteRead(MAX30100_SLAVE_ADDR, &wr_data, 1, tmp_reg, 4);               // Ocitavanje 4 bajta sa adrese FIFO Data registra (dva bajta za IR i dva bajta za RED LED).
                                                                                // Za rekonstrukciju vrednosti sa RED i IR LED neophodno je procitati FIFO Data registar 4 puta.
    while(I2C2_IsBusy());                                           
    
    
    // Rekonstruisanje 16-bitne vrednosti sa izlaza ADC i postavljanje vrednosti na strukturu:
    // high byte<<8 | low byte 
    sens->ir_led = (tmp_reg[0]<<8|tmp_reg[1]);     
    sens->red_led = (tmp_reg[2]<<8|tmp_reg[3]);
}

void MAX30100_reset(){
    // Kako bi svi registri bili resetovani u Mode Configuration registru je neophodno postaviti RESET bit (B6) na 1.
    static uint8_t wr[2]; 
    wr[0]= MAX30100_MODE_CFG;
    wr[1] = MAX30100_RESET;
    I2C2_Write(MAX30100_SLAVE_ADDR, wr,2);
    while(I2C2_IsBusy());
}

void MAX30100_enable_temp_read(){
    // U Mode Configuration registru bit B3 (TEMP_EN) je self-clearing bit.
    // Postavljanjem vrednosti ovog bita na 1 inicira se ocitavanje temperaturnog senzora.
    // Po ocitavanju temperature ovaj bit se resetuje na 0 (self-clearing).
    static uint8_t wr[2];
    static uint8_t mode_cfg_reg;
    wr[0]=MAX30100_MODE_CFG;
    I2C2_WriteRead(MAX30100_SLAVE_ADDR, wr, 1, &mode_cfg_reg, 1);               // Citanje sadrzaja Mode Configuration registra.
    while(I2C2_IsBusy());
    
    wr[1] = mode_cfg_reg|0x08;                                                  // Izmena vrednosti u Mode Configuration registru tako da se bit B3 postavi na 1, a svi ostali bitovi zadrze prethodnu vrednost.
    
    I2C2_Write(MAX30100_SLAVE_ADDR, wr, 2);                                     // Upis modifikovane vrednosti u Mode Configuration registar.
    while(I2C2_IsBusy());
}
void MAX30100_read_temp_data(MAX30100_sens_t* sens){
    static uint8_t wr;
    
    static uint8_t temp_int1;
    static int8_t temp_int2;
    static uint8_t temp_frac;

    wr = MAX30100_TEMP_INTEGER;

    I2C2_WriteRead(MAX30100_SLAVE_ADDR, &wr, 1, &temp_int1, 1);                 // Citanje sadrzaja Temp Integer registra.
    while(I2C2_IsBusy());
    temp_int2 = (int8_t)temp_int1;                                              // typecast u oznacenu celobrojnu vrednost (pozitivne i negativne temperature).
    
    wr = MAX30100_TEMP_FRACTION;

    I2C2_WriteRead(MAX30100_SLAVE_ADDR, &wr, 1, &temp_frac, 1);                 // Citanje sadrzaja Temp Fraction registra.
    while(I2C2_IsBusy());

    // Temperatura izmerena senzorom je T_int+T_frac.
    // uint8_t vrednost ocitana iz Temp Fraction registra mnozi se vrednoscu LSB (2^(-4)).
    // Na sens strukturu postavlja se vrednost temperature konvertovana u float tip.
    sens->temperature = (float)(temp_int2+temp_frac*0.0625);

}
/* *****************************************************************************
 End of File
 */
