#include <avr/io.h>
#include "pwmi.h"
#include "spi.h"

void pwmi_init(){

}

void pwmi_load(unsigned char data[]){
        spi_send_array(data);
}
