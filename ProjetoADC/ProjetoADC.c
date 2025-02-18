#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "inc/ssd1306.h"

// Caso WIDTH e HEIGHT não estejam definidos em ssd1306.h, descomente as linhas abaixo:
// #define WIDTH 128
// #define HEIGHT 64

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C

#define LED_PIN_GREEN 11
#define LED_PIN_BLUE 12
#define LED_PIN_RED 13

// Joystick: ADC0 (GPIO26) para eixo X e ADC1 (GPIO27) para eixo Y
#define JOY_X_ADC 0
#define JOY_Y_ADC 1

// Botões
#define JOY_BUTTON_PIN 22
#define BUTTON_A_PIN 5
#define DEBOUNCE_DELAY_MS 200

// PWM
#define PWM_WRAP 4095

// Define centro e dead zone para o joystick
#define JOYSTICK_CENTER 2048
#define DEADZONE 100

// Variáveis globais
volatile bool thick_border = false;
volatile bool led_green_state = false;
volatile bool leds_off = false; // Começa com LEDs desligados
volatile uint32_t last_button_press = 0;

// Handler de interrupção
void gpio_irq_handler(uint gpio, uint32_t events)
{
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if ((now - last_button_press) < DEBOUNCE_DELAY_MS)
        return;
    last_button_press = now;

    if (gpio == JOY_BUTTON_PIN && (events & GPIO_IRQ_EDGE_FALL))
    {
        thick_border = !thick_border;
        if (!leds_off)
        { // Só altera se os LEDs estiverem habilitados
            led_green_state = !led_green_state;
            gpio_put(LED_PIN_GREEN, led_green_state);
        }
    }
    else if (gpio == BUTTON_A_PIN && (events & GPIO_IRQ_EDGE_FALL))
    {
        leds_off = !leds_off; // Toggle no estado

        // Atualiza todos os LEDs conforme o estado
        gpio_put(LED_PIN_GREEN, leds_off ? false : led_green_state);
        // Aqui, definimos um nível fixo (metade do PWM_WRAP) quando desligando
        pwm_set_gpio_level(LED_PIN_RED, leds_off ? 0 : PWM_WRAP / 2);
        pwm_set_gpio_level(LED_PIN_BLUE, leds_off ? 0 : PWM_WRAP / 2);
    }
}

int main()
{
    stdio_init_all();

    // Inicialização I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Configuração ADC
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);

    // Configuração botões
    gpio_init(JOY_BUTTON_PIN);
    gpio_set_dir(JOY_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(JOY_BUTTON_PIN);

    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_set_irq_enabled_with_callback(JOY_BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true);

    // Configuração PWM para LED Vermelho e LED Azul
    gpio_set_function(LED_PIN_RED, GPIO_FUNC_PWM);
    gpio_set_function(LED_PIN_BLUE, GPIO_FUNC_PWM);
    uint slice_num_red = pwm_gpio_to_slice_num(LED_PIN_RED);
    uint slice_num_blue = pwm_gpio_to_slice_num(LED_PIN_BLUE);
    pwm_set_wrap(slice_num_red, PWM_WRAP);
    pwm_set_wrap(slice_num_blue, PWM_WRAP);
    pwm_set_enabled(slice_num_red, true);
    pwm_set_enabled(slice_num_blue, true);

    // Configuração do LED Verde
    gpio_init(LED_PIN_GREEN);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);

    // Inicialização do display OLED
    ssd1306_t ssd;
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);

    while (true)
    {
        ssd1306_fill(&ssd, false);

        // Desenho da borda
        ssd1306_rect(&ssd, 0, 0, ssd.width, ssd.height, true, false);
        if (thick_border)
        {
            ssd1306_rect(&ssd, 1, 1, ssd.width - 2, ssd.height - 2, true, false);
            sleep_ms(150);
        }

        // Leitura do joystick
        adc_select_input(JOY_X_ADC);
        uint16_t adc_x = adc_read();
        adc_select_input(JOY_Y_ADC);
        uint16_t adc_y = adc_read();

        // Mapeia os valores ADC para a posição do quadrado (8x8 pixels)
        // Observe que invertemos os eixos para que o movimento corresponda à direção física do joystick
        uint8_t square_x = ((adc_y) * (ssd.width - 8)) / 4095;
        uint8_t square_y = ((4095 - adc_x) * (ssd.height - 8)) / 4095;
        ssd1306_rect(&ssd, square_y, square_x, 8, 8, true, true);
        ssd1306_send_data(&ssd);

        // Controle dos LEDs via PWM
        int delta_x = abs((int)adc_x - JOYSTICK_CENTER);
        int delta_y = abs((int)adc_y - JOYSTICK_CENTER);

        // Aplica zona morta: se estiver muito próximo do centro, considera delta = 0
        if (delta_x < DEADZONE)
            delta_x = 0;
        if (delta_y < DEADZONE)
            delta_y = 0;

        uint32_t pwm_red = leds_off ? 0 : ((uint32_t)delta_y * PWM_WRAP) / JOYSTICK_CENTER;
        uint32_t pwm_blue = leds_off ? 0 : ((uint32_t)delta_x * PWM_WRAP) / JOYSTICK_CENTER;

        pwm_set_gpio_level(LED_PIN_RED, pwm_red);
        pwm_set_gpio_level(LED_PIN_BLUE, pwm_blue);

        sleep_ms(150);
    }
    return 0;
}
