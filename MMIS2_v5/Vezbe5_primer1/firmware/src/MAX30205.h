#ifndef MAX30205_H    /* Guard against multiple inclusion */
#define MAX30205_H


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <string.h>
    
// Slave adresa MAX30205:
#define MAX30205_SLAVE_ADDR (0x90>>1)
    
// Adrese registara MAX30205:
#define MAX30205_TEMPERATURE   0x00
#define MAX30205_CONFIGURATION 0x01
#define MAX30205_THYST         0x02
#define MAX30205_TOS           0x03
    
// Struktura koja sadrzi podatke ocitane sa senzora:
typedef struct{
    int8_t temp_high_byte;
    int8_t temp_low_byte;
    
    float temp;
}MAX30205_sens_t;

// Funkcije za rad sa MAX30205:
void MAX30205_init();
void MAX30205_hyst_config(uint16_t t_hyst);
void MAX30205_os_config(uint16_t t_os);
void MAX30205_read_temperature();

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
