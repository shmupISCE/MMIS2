#include "max30100.h"

uint8_t MAX30100_get_device_ID(){
        static uint8_t wr[1];
        static uint8_t rd[1];
        wr[0]=MAX30100_PART_ID;
        I2C2_WriteRead(MAX30100_SLAVE_ADDR, wr, 1, rd, 1);
        return rd[0];
}


void MAX30100_init(MAX30100_MODE_t mode, MAX30100_LED_CURRENT_t red_led_current, MAX30100_LED_CURRENT_t ir_led_current, MAX30100_SAMPLE_RATE_t sample_rate, MAX30100_LED_PULSE_WIDTH_t pulse_width){
    static uint8_t wr[2];
    
    // Podesavanje moda rada
    wr[0]= MAX30100_MODE_CFG;
    wr[1] = mode;
    I2C2_Write(MAX30100_SLAVE_ADDR, wr,2);
    while(I2C2_IsBusy());
    wr[1] = 0x00;
    // Podesavanje struje za RED i IR LED 
    wr[0]= MAX30100_LED_CFG;
    wr[1] = (red_led_current<<4)|ir_led_current;
    I2C2_Write(MAX30100_SLAVE_ADDR, wr,2);
    while(I2C2_IsBusy());
    wr[1] = 0x00;
    // Podesavanje SpO2 - sample rate, pulse width i ADC res
    wr[0]= MAX30100_SPO2_CFG;
    wr[1] = (sample_rate<<2)|pulse_width;
    I2C2_Write(MAX30100_SLAVE_ADDR, wr,2);
    while(I2C2_IsBusy());
}

void MAX30100_FIFO_init(){
    static uint8_t wr[2];
    
    // Reset write pointer-a, overflow counter-a i read pointer-a
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

    I2C2_WriteRead(MAX30100_SLAVE_ADDR, &wr_data, 1, tmp_reg, 4);
    while(I2C2_IsBusy());
    sens->ir_led = (tmp_reg[0]<<8|tmp_reg[1]);
    sens->red_led = (tmp_reg[2]<<8|tmp_reg[3]);
}

void MAX30100_reset(){
    static uint8_t wr[2]; 
    wr[0]= MAX30100_MODE_CFG;
    wr[1] = MAX30100_RESET;
    I2C2_Write(MAX30100_SLAVE_ADDR, wr,2);
    while(I2C2_IsBusy());
}

void MAX30100_enable_temp_read(){
    static uint8_t wr[2];
    static uint8_t mode_cfg_reg;
    wr[0]=MAX30100_MODE_CFG;
    I2C2_WriteRead(MAX30100_SLAVE_ADDR, wr, 1, &mode_cfg_reg, 1);
    while(I2C2_IsBusy());
    
    wr[1] = mode_cfg_reg|0x08;
    
    I2C2_Write(MAX30100_SLAVE_ADDR, wr, 2);
    while(I2C2_IsBusy());
    

}
void MAX30100_read_temp_data(MAX30100_sens_t* sens){
    static uint8_t wr;
    
    static uint8_t temp_int1;
    static int8_t temp_int2;
    static uint8_t temp_frac;

    wr = MAX30100_TEMP_INTEGER;

    I2C2_WriteRead(MAX30100_SLAVE_ADDR, &wr, 1, &temp_int1, 1);
    while(I2C2_IsBusy());
    temp_int2 = (int8_t)temp_int1;
     
    wr = MAX30100_TEMP_FRACTION;

    I2C2_WriteRead(MAX30100_SLAVE_ADDR, &wr, 1, &temp_frac, 1);
    while(I2C2_IsBusy());

    sens->temperature = (float)(temp_int2+temp_frac*0.0625);

}

/* *****************************************************************************
 End of File
 */
