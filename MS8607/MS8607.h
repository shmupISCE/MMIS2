#ifndef MS8607_H    /* Guard against multiple inclusion */
#define MS8607_H
                
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MS8607_PT_SLAVE_ADDR                0x76
#define MS8607_H_SLAVE_ADDR                 0x40
    
#define MS8607_PT_RESET                     0x1E
#define MS8607_PT_READ_ADC                  0x00
#define MS8607_PT_D1_CONVERSION_OSR256      0x40
#define MS8607_PT_D1_CONVERSION_OSR512      0x42   
#define MS8607_PT_D1_CONVERSION_OSR1024     0x44
#define MS8607_PT_D1_CONVERSION_OSR2048     0x46
#define MS8607_PT_D1_CONVERSION_OSR4096     0x48
#define MS8607_PT_D1_CONVERSION_OSR8192     0x4A
    
#define MS8607_PT_D2_CONVERSION_OSR256      0x50
#define MS8607_PT_D2_CONVERSION_OSR512      0x52   
#define MS8607_PT_D2_CONVERSION_OSR1024     0x54
#define MS8607_PT_D2_CONVERSION_OSR2048     0x56
#define MS8607_PT_D2_CONVERSION_OSR4096     0x58
#define MS8607_PT_D2_CONVERSION_OSR8192     0x5A
    
#define MS8607_PT_READ_PROM_CRC_FACTORY_DEF 0xA0
#define MS8607_PT_READ_PROM_C1              0xA2
#define MS8607_PT_READ_PROM_C2              0xA4
#define MS8607_PT_READ_PROM_C3              0xA6
#define MS8607_PT_READ_PROM_C4              0xA8
#define MS8607_PT_READ_PROM_C5              0xAA
#define MS8607_PT_READ_PROM_C6              0xAC
    
#define MS8607_H_RESET                      0xFE
#define MS8607_H_MEASURE_HOLD_MASTER        0xE5  
#define MS8607_H_MEASURE_NO_HOLD_MASTER     0xF5
#define MS8607_H_WRITE_USER_REG             0xE6
#define MS8607_H_READ_USER_REG              0xE7    

#define MS8607_H_MEASURE_POLL               0x81
    
#define MS8607_H_OSR_4096                   0x00
#define MS8607_H_OSR_2048                   0x01
#define MS8607_H_OSR_1024                   0x80
#define MS8607_H_OSR_256                    0x81
    
    
typedef struct{
    uint16_t pressure_sensitivity;
    uint16_t pressure_offset;
    uint16_t temp_coeff_pressure_sensitivity;
    uint16_t temp_coeff_pressure_offset;
    uint16_t ref_temp;
    uint16_t temp_coeff_temp;
    
    uint32_t digital_pressure_value;
    uint32_t digital_temperature_value;
    
    float dt;
    float offset;
    float sensitivity;
    
    float temperature;
    float pressure;
    
    uint16_t digital_relative_humidity_value;
    float relative_humidity;
}MS8607_sens_t;
    
void MS8607_reset_PTsens();
void MS8607_reset_Hsens();
void MS8607_read_PT_calibration_data(MS8607_sens_t* sens);
void MS8607_read_temperature(MS8607_sens_t* sens, uint8_t convert_d2_osr_cmd);
void MS8607_read_pressure(MS8607_sens_t* sens, uint8_t convert_d1_osr_cmd);
void MS8607_read_relative_humidity(MS8607_sens_t* sens, uint8_t res, uint8_t hold_mode);
    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif 

/* *****************************************************************************
 End of File
 */
