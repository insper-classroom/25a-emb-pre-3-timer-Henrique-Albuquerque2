#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;

volatile int timer = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        flag_f_r = 1;
    } else if (events == 0x8) { // rise edge
    }
}

bool timer_callback(repeating_timer_t *rt){
    timer = 1;
    return true;
}


int main() {
    stdio_init_all();

    int timer_0_ms = 500;
    repeating_timer_t timer_0;
    
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);

    if (!add_repeating_timer_ms(timer_0_ms, 
                                timer_callback,
                                NULL, 
                                &timer_0)) {
        printf("Failed to add timer\n");
    }
    int led_state = 0;
    int acionado = 0;
    while (true) {

        if (flag_f_r) {
            printf("Button pressed\n");
            flag_f_r = 0;
            if (acionado == 1) {
                printf("Cancel Timer\n");
                led_state = 0;
                acionado = 0;
                gpio_put(LED_PIN_R, led_state);
                cancel_repeating_timer(&timer_0);
            }
            else if (acionado == 0) {
                printf("Start Timer\n");
                led_state = 1;
                acionado = 1;
                gpio_put(LED_PIN_R, led_state);
            }
        }
        if (acionado) {
            if (timer) {
                printf("Timer\n");
                led_state = !led_state;
                gpio_put(LED_PIN_R, led_state);
                timer = 0;
            }
        }
    }
}