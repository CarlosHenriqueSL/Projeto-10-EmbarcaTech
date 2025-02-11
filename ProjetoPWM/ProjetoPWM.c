/*
 * Código para controle do servomotor via PWM no RP2040 utilizando a Pico SDK.
 *
 * Requisitos:
 *  - Configurar a GPIO 22 para PWM com ~50Hz (período de 20ms).
 *  - Posição 180°: pulso de 2400µs, aguardar 5s.
 *  - Posição 90°:  pulso de 1470µs, aguardar 5s.
 *  - Posição 0°:   pulso de 500µs,  aguardar 5s.
 *  - Varredura suave entre 0° e 180° (incrementos/decrementos de 5µs com atraso de 10ms).
 */

#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define SERVO_PWM_PIN 22
#define PULSE_MIN 500    // Pulso para 0° (500µs)
#define PULSE_MAX 2400   // Pulso para 180° (2400µs)

int main(void) {
    stdio_init_all();

    // Configura a GPIO 22 para função PWM.
    gpio_set_function(SERVO_PWM_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PWM_PIN);

    // Configura o clock do PWM para que cada tick tenha 1µs:
    // 125MHz / 125 = 1MHz (1µs por tick)
    pwm_set_clkdiv(slice_num, 125.0f);

    // Define o wrap para obter um período de 20ms (20000µs):
    // Período = (wrap + 1) / 1MHz  => wrap = 20000 - 1 = 19999.
    pwm_set_wrap(slice_num, 19999);

    // Habilita o PWM no slice.
    pwm_set_enabled(slice_num, true);

    // --- Posições fixas com espera de 5 segundos em cada ---
    
    // Posição de 180° (pulso de 2400µs).
    pwm_set_gpio_level(SERVO_PWM_PIN, PULSE_MAX);
    sleep_ms(5000);

    // Posição de 90° (pulso de 1470µs).
    pwm_set_gpio_level(SERVO_PWM_PIN, 1470);
    sleep_ms(5000);

    // Posição de 0° (pulso de 500µs).
    pwm_set_gpio_level(SERVO_PWM_PIN, PULSE_MIN);
    sleep_ms(5000);

    // --- Varredura suave entre 0° e 180° ---
    while (true) {
        // Incrementa de 0° a 180° (500µs a 2400µs)
        for (uint pulse = PULSE_MIN; pulse <= PULSE_MAX; pulse += 5) {
            pwm_set_gpio_level(SERVO_PWM_PIN, pulse);
            sleep_ms(10);
        }
        // Decrementa de 180° a 0°
        for (int pulse = PULSE_MAX; pulse >= (int)PULSE_MIN; pulse -= 5) {
            pwm_set_gpio_level(SERVO_PWM_PIN, pulse);
            sleep_ms(10);
        }
    }

    return 0;
}
