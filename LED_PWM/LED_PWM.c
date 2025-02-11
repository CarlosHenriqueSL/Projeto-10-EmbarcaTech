/*
 * Código para controle do LED via PWM no RP2040 utilizando a Pico SDK.
 *
 * Configura a GPIO 12 para PWM com resolução de 8 bits (wrap = 255) e frequência
 * de aproximadamente 1 kHz. O LED realiza um efeito de fade in (aumenta brilho)
 * e fade out (diminui brilho).
 */

#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define LED_PWM_PIN 12

int main(void) {
    stdio_init_all();

    // Configura a GPIO 12 para função PWM.
    gpio_set_function(LED_PWM_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(LED_PWM_PIN);

    // Configura o PWM para 8 bits (wrap = 255) e frequência ~1kHz.
    // Para frequência: freq = 125MHz / (divider * (wrap+1)).
    // Utilizando um divisor de aproximadamente 500 obtemos ~1kHz.
    pwm_set_clkdiv(slice_num, 500.0f);
    pwm_set_wrap(slice_num, 255);

    // Habilita o PWM no slice.
    pwm_set_enabled(slice_num, true);

    while (true) {
        // Fade in: aumenta o brilho de 0 a 255.
        for (uint brightness = 0; brightness <= 255; brightness++) {
            pwm_set_gpio_level(LED_PWM_PIN, brightness);
            sleep_ms(5);
        }
        // Fade out: diminui o brilho de 255 a 0.
        for (int brightness = 255; brightness >= 0; brightness--) {
            pwm_set_gpio_level(LED_PWM_PIN, brightness);
            sleep_ms(5);
        }
    }

    return 0;
}
