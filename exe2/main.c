#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

struct repeating_timer timer_red, timer_green;

bool timer_red_callback(struct repeating_timer *t) {
    gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R)); 
    return true;
}

bool timer_green_callback(struct repeating_timer *t) {
    gpio_put(LED_PIN_G, !gpio_get(LED_PIN_G)); 
    return true;
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { 
        if (gpio == BTN_PIN_R)
            flag_r = 1;
        else if (gpio == BTN_PIN_G)
            flag_g = 1;
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0); 

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_put(LED_PIN_G, 0); 

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    bool timer_red_running = false;
    bool timer_green_running = false;

    while (true) {
        if (flag_r) {
            flag_r = 0;

            if (timer_red_running) {
                cancel_repeating_timer(&timer_red); 
                gpio_put(LED_PIN_R, 0); 
                timer_red_running = false;
            } else {
                add_repeating_timer_ms(500, timer_red_callback, NULL, &timer_red); 
                timer_red_running = true;
            }
        }

        if (flag_g) {
            flag_g = 0;

            if (timer_green_running) {
                cancel_repeating_timer(&timer_green); 
                gpio_put(LED_PIN_G, 0); 
                timer_green_running = false;
            } else {
                add_repeating_timer_ms(250, timer_green_callback, NULL, &timer_green); 
                timer_green_running = true;
            }
        }

        sleep_ms(10); 
    }

    return 0;
}