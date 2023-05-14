#include "MS8607.h"

void MS8607_reset_PTsens(){
    static uint8_t wr;
    wr = MS8607_PT_RESET;
    I2C2_Write(MS8607_PT_SLAVE_ADDR, &wr, 1);
    while(I2C2_IsBusy());
}

void MS8607_reset_Hsens(){
    static uint8_t wr;
    wr = MS8607_H_RESET;
    I2C2_Write(MS8607_H_SLAVE_ADDR, &wr, 1);
    while(I2C2_IsBusy());
}

void MS8607_read_PT_calibration_data(MS8607_sens_t* sens){
    static uint8_t wr[6] = {MS8607_PT_READ_PROM_C1, MS8607_PT_READ_PROM_C2, MS8607_PT_READ_PROM_C3, MS8607_PT_READ_PROM_C4, MS8607_PT_READ_PROM_C5, MS8607_PT_READ_PROM_C6};
    static uint8_t rd[2];
    static uint8_t rd_buf[12];
    
    for(uint8_t i=0;i<6;i++){
        I2C2_WriteRead(MS8607_PT_SLAVE_ADDR, &wr[i], 1, rd, 2);
        while(I2C2_IsBusy());
        rd_buf[2*i] = rd[0];
        rd_buf[2*i+1] = rd[1];
    }
    
    sens->pressure_sensitivity = rd_buf[0]<<8|rd_buf[1];
    sens->pressure_offset = rd_buf[2]<<8|rd_buf[3];
    sens->temp_coeff_pressure_sensitivity = rd_buf[4]<<8|rd_buf[5];
    sens->temp_coeff_pressure_offset = rd_buf[6]<<8|rd_buf[7];
    sens->ref_temp = rd_buf[8]<<8|rd_buf[9];
    sens->temp_coeff_temp = rd_buf[10]<<8|rd_buf[11];
}

void MS8607_read_temperature(MS8607_sens_t* sens, uint8_t convert_d2_osr_cmd){
    static uint8_t wr;
    static uint8_t rd[3];
    
    wr = convert_d2_osr_cmd;
    I2C2_Write(MS8607_PT_SLAVE_ADDR, &wr, 1);
    while(I2C2_IsBusy());
    
    CORETIMER_DelayMs(100);
    
    wr = MS8607_PT_READ_ADC;
    I2C2_WriteRead(MS8607_PT_SLAVE_ADDR, &wr, 1, rd, 3);
    while(I2C2_IsBusy());
    
    sens->digital_temperature_value = (rd[0]<<16)|(rd[1]<<8)|rd[2];
    
    sens->dt = (float)sens->digital_temperature_value-(sens->ref_temp*256.0);
    
    sens->temperature = 2000.0+sens->dt*(float)sens->temp_coeff_temp/8388608.0;
    
}
void MS8607_read_pressure(MS8607_sens_t* sens, uint8_t convert_d1_osr_cmd){
    static uint8_t wr;
    static uint8_t rd[3];
    
    wr = convert_d1_osr_cmd;
    I2C2_Write(MS8607_PT_SLAVE_ADDR, &wr, 1);
    while(I2C2_IsBusy());
    
    CORETIMER_DelayMs(100);
    
    wr = MS8607_PT_READ_ADC;
    I2C2_WriteRead(MS8607_PT_SLAVE_ADDR, &wr, 1, rd, 3);
    while(I2C2_IsBusy());
    
    sens->digital_pressure_value = (rd[0]<<16)|(rd[1]<<8)|rd[2];

    sens->offset = (float)sens->pressure_offset*131072.0 + ((float)sens->temp_coeff_pressure_offset*sens->dt)/64.0;
    
    sens->sensitivity = (float)sens->pressure_sensitivity*65536.0 + ((float)sens->temp_coeff_pressure_sensitivity*sens->dt)/128.0;

    sens->pressure = ((float)sens->digital_pressure_value*sens->sensitivity/2097152.0-sens->offset)/32768.0;
}

void MS8607_read_relative_humidity(MS8607_sens_t* sens, uint8_t res, uint8_t hold_mode){
    static uint8_t wr[2];
    static uint8_t rd[2];
    
    wr[0] = MS8607_H_WRITE_USER_REG;
    wr[1] = res;
    I2C2_Write(MS8607_H_SLAVE_ADDR, wr, 2);
    while(I2C2_IsBusy());
    
    CORETIMER_DelayMs(100);
    
    wr[0] = hold_mode;
    I2C2_WriteRead(MS8607_H_SLAVE_ADDR, wr, 1, rd, 2);
    while(I2C2_IsBusy());
    
    sens->digital_relative_humidity_value = rd[0]<<8|rd[1];
    
    sens->relative_humidity = -600.0 +12500.0*sens->digital_relative_humidity_value/65536.0;
}