#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/mutex.h"
#include "hardware/spi.h"

#include "main.h"
#include "misc.h"
#include "sensors/bmp.h"
#include "sensors/gps.h"
#include "helpers/repeater.h"



//RUNTIME VARIABLES
static Repeater LED_repeater(3000);
static Repeater BMP_repeater(500);

//MAIN CORE FUNCTIONS

int main() {

    //init serial
    setup_default_uart();
    debug("\n=== WM Pi Pico HAB Tracker V1.0 ===\n");

    debug("\n>>> Initialising modules ...\n\n");

    debug("> Initialise LED...");
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    LED_repeater.update_delay(50, 50, 50);
    debug(" Done\n");

    debug("> Initialise mutex...");
    mutex_init(&mtx);
    debug(" Done\n");

    debug("\n>>> Spooling thread... \n");
    multicore_launch_core1(core_entry);
    uint32_t r = multicore_fifo_pop_blocking();
    if (r != CORE_INIT_FLAG) {
        debug("<!> (0) Invalid parameter from Core 1!\n");
    } else {
        multicore_fifo_push_blocking(CORE_INIT_FLAG);
        debug("> (0) Core 0 initialised.\n");
    }
    
    //Initialise SPI 0 @ 500kHz
    spi_init(SPI_PORT_0, 500000);
    //GPIO for SPI
    gpio_set_function(MISO_0, GPIO_FUNC_SPI);
    gpio_set_function(SCLK_0, GPIO_FUNC_SPI);
    gpio_set_function(MOSI_0, GPIO_FUNC_SPI);
    debug("> (0) SPI initialised.\n");

    //Initialise BMP280
    initBMP280();
    debug("> (0) BMP280 initialised.\n");

    while(1) {
        //mainloop
        
        //TODO:
        check_LED(&state);
        check_BMP(&state);
        //collect N02 data
        //collect GPS data
        
    }
}

//THREADED FUNCTIONS

void core_entry() {

    multicore_fifo_push_blocking(CORE_INIT_FLAG);
    uint32_t r = multicore_fifo_pop_blocking();

    if (r != CORE_INIT_FLAG) {
        debug("<!> (1) Invalid parameter from Core 0!\n");
    } else {
        debug("> (1) Core 1 initialised.\n");
    }

    

    while(1) {
        //threadloop
        
        //TODO:
        //Send Lora message
        
    }
        
}

void check_LED(struct STATE *s) {
    mutex_enter_blocking(&mtx);
    long alt = s->Altitude;
    mutex_exit(&mtx);
    
    if (alt < LOW_POWER_ALTITUDE && LED_repeater.can_fire()) {
        gpio_put(LED_PIN, !gpio_get(LED_PIN));
    }
}

void check_BMP(struct STATE *s) {
    if (BMP_repeater.can_fire()) {
        int32_t temp = readTemp();
        uint32_t press = readPress();
        mutex_enter_blocking(&mtx);
        printf("Temp = %.2fC | Press = %.2fPa \n", temp / 100.0, press / 256.0);
        s->Pressure = press;
        s->ExternalTemperature = temp;
        mutex_exit(&mtx);
    }
}


