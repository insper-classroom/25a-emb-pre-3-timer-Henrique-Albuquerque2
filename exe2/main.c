#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

volatile int timer_r = 0;
volatile int timer_g = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_R)
            flag_r = 1;
        else if (gpio == BTN_PIN_G)
            flag_g = 1;
    }
}

bool timer_callback_g(repeating_timer_t *rt){
    timer_g = 1;
    return true;
}

bool timer_callback_r(repeating_timer_t *rt){
    timer_r = 1;
    return true;
}


int main() {
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    int timer_g_ms = 500;
    repeating_timer_t timer_g_p;

    int timer_r_ms = 250;
    repeating_timer_t timer_r_p;

    if (!add_repeating_timer_ms(timer_g_ms, 
            timer_callback_g,
            NULL, 
            &timer_g_p)) {
    printf("Failed to add timer\n");
    }

    int led_state_g = 0;
    int acionado_g = 0;

    if (!add_repeating_timer_ms(timer_r_ms, 
            timer_callback_r,
            NULL, 
            &timer_r_p)) {
    printf("Failed to add timer\n");
    }

    int led_state_r = 0;
    int acionado_r = 0;

    while (true) {

        if (flag_r) {
            printf("Button pressed\n");
            flag_r = 0;
            if (acionado_r == 1) {
                printf("Cancel Timer\n");
                led_state_r = 0;
                acionado_r = 0;
                gpio_put(LED_PIN_R, led_state_r);
                cancel_repeating_timer(&timer_r_p);
            }
            else if (acionado_r == 0) {
                printf("Start Timer\n");
                led_state_r = 1;
                acionado_r = 1;
                gpio_put(LED_PIN_R, led_state_r);
            }
        }
        if (acionado_r) {
            if (timer_r) {
                printf("Timer\n");
                led_state_r = !led_state_r;
                gpio_put(LED_PIN_R, led_state_r);
                timer_r = 0;
            }
        }
        if (flag_g) {
            printf("Button pressed\n");
            flag_g = 0;
            if (acionado_g == 1) {
                printf("Cancel Timer\n");
                led_state_g = 0;
                acionado_g = 0;
                gpio_put(LED_PIN_G, led_state_g);
                cancel_repeating_timer(&timer_g_p);
            }
            else if (acionado_g == 0) {
                printf("Start Timer\n");
                led_state_g = 1;
                acionado_g = 1;
                gpio_put(LED_PIN_G, led_state_g);
            }
        }
        if (acionado_g) {
            if (timer_g) {
                printf("Timer\n");
                led_state_g = !led_state_g;
                gpio_put(LED_PIN_G, led_state_g);
                timer_g = 0;
            }
        }
    }
}