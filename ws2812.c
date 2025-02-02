/*
 * Por: Larissa Sousa
 * 
 * Este código é uma adaptação do código original do SDK Pico
 * para a utilização da matriz de LEDs WS2812 do BitDogLab, acionamento do LED RGB e utilização de 2 botões.
 * 
 * Original em: 
 * https://github.com/raspberrypi/pico-examples/tree/master/pio/ws2812
*/

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7
#define NUM_DIGITS 10 // Qtd de dígitos: 0 a 9
#define tempo 100

// Definição dos padrões para os números 0-9 na matriz 5x5
const uint8_t numbers[NUM_DIGITS][NUM_PIXELS] = {
    {1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,1,1,1,1}, // 0
    {1,1,1,1,1, 0,0,1,0,0, 0,0,1,0,1, 0,1,1,0,0, 0,0,1,0,0}, // 1
    {1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1}, // 2
    {1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1}, // 3
    {1,0,0,0,0, 0,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1}, // 4
    {1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1}, // 5
    {1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1}, // 6
    {0,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 0,0,0,0,1, 1,1,1,1,1}, // 7
    {1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1}, // 8
    {1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1}  // 9
};

// Variáveis globais 
uint32_t led_buffer[NUM_PIXELS] = {0};
uint8_t current_digit = 0; // Dígito atual a ser exibido
uint8_t selected_r = 0; // Intensidade do led vermelho (0 a 255)
uint8_t selected_g = 0; // Intensidade do led verde (0 a 255)
uint8_t selected_b = 10; // Intensidade do led azul (0 a 255)
const uint ledRed_pin = 13; // Led vermelho => GPIO13
const uint button_increment = 5; // Botão A => GPIO5
const uint button_decrement = 6; // Botão B => GPIO6
static volatile uint a = 1;
static volatile uint32_t last_time_decrement = 0; // Armazena o tempo do último evento no decremento (microssegundos) 
static volatile uint32_t last_time_increment = 0; // Armazena o tempo do último evento no incremento

// Prototipo da função de interrupção
static void gpio_irq_handler_increment(uint gpio, uint32_t events);
static void gpio_irq_handler_decrement(uint gpio, uint32_t events);

// Callback da interrupção
static void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == button_increment) {
        gpio_irq_handler_increment(gpio, events);
    } else if (gpio == button_decrement) {
        gpio_irq_handler_decrement(gpio, events);
    }
}

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

// Atualiza o buffer de LEDs com o padrão do número atual
void update_led_buffer() {
    for (int i = 0; i < NUM_PIXELS; i++) {
        if (numbers[current_digit][i] == 1) {
            led_buffer[i] = urgb_u32(selected_r, selected_g, selected_b);
        } else {
            led_buffer[i] = 0;
        }
    }
}

void set_leds_from_buffer() {
    // Envia o estado de todos os LEDs para a matriz
    for (int i = 0; i < NUM_PIXELS; i++) {
        put_pixel(led_buffer[i]);
    }
}

int main() {
    stdio_init_all();

    gpio_init(ledRed_pin); // Inicializa o pino do LED
    gpio_set_dir(ledRed_pin, GPIO_OUT); // Configura o pino como saída

    gpio_init(button_increment); // Inicializa o botão de incremento
    gpio_set_dir(button_increment, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(button_increment); // Ativa o pull-up interno
    gpio_set_irq_enabled_with_callback(button_increment, GPIO_IRQ_EDGE_FALL, true, gpio_callback); // Configura interrupção para borda de queda e registra o callback

    gpio_init(button_decrement); // Inicializa o botão de decremento
    gpio_set_dir(button_decrement, GPIO_IN); 
    gpio_pull_up(button_decrement); 
    gpio_set_irq_enabled_with_callback(button_decrement, GPIO_IRQ_EDGE_FALL, true, gpio_callback);
   
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    while (1) {
        update_led_buffer();
        set_leds_from_buffer();

        gpio_put(ledRed_pin, true);
        sleep_ms(tempo);
        gpio_put(ledRed_pin, false);
        sleep_ms(tempo);

    }

    return 0;
}

// Função de interrupção para incremento
void gpio_irq_handler_increment(uint gpio, uint32_t events) {

    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    printf("A = %d\n", a);
    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time_increment > 300000) // 300 ms de debouncing
    {
        last_time_increment = current_time; // Atualiza o tempo do último evento

        current_digit = (current_digit == NUM_DIGITS - 1) ? 0 : current_digit + 1; 
        a++; // incrementa a variavel de verificação
    }
}

// Função de interrupção para decremento
void gpio_irq_handler_decrement(uint gpio, uint32_t events) {

    uint32_t current_time = to_us_since_boot(get_absolute_time());
    printf("A = %d\n", a);
    
    if (current_time - last_time_decrement > 300000) // 350 ms de debouncing
    {
        last_time_decrement = current_time; 

        current_digit = (current_digit == 0) ? NUM_DIGITS - 1 : current_digit - 1;
        a++; 
    }
}
