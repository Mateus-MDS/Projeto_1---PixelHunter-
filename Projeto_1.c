#include <stdio.h>             // Biblioteca padrão para entrada e saída
#include "pico/stdlib.h"       // Biblioteca padrão do Pico
#include "hardware/adc.h"      // Biblioteca para ADC
#include "hardware/i2c.h"      // Biblioteca para I2C
#include "inc/ssd1306.h"       // Biblioteca para display OLED
#include "inc/font.h"          // Biblioteca para fontes
#include "hardware/pio.h"      // Biblioteca para PIO
#include "hardware/clocks.h"   // Biblioteca para clocks
#include "pico/stdio_usb.h"    // Biblioteca para USB
#include "animacoes_led.pio.h" // Biblioteca para animações LED

/* ========== DEFINIÇÕES DE HARDWARE ========== */

// Configuração da matriz de LEDs
#define NUM_PIXELS 25          // Número de LEDs na matriz
#define matriz_leds 7          // Pino de saída para matriz

// Configuração I2C para o display
#define I2C_PORT i2c1          // Porta I2C utilizada
#define I2C_SDA 14             // Pino SDA
#define I2C_SCL 15             // Pino SCL
#define ENDERECO 0x3C          // Endereço I2C do display
#define WIDTH 128              // Largura do display
#define HEIGHT 64              // Altura do display

// Definições dos pinos
#define Eixo_Y 27              // Pino GP27 para eixo Y (ADC0)
#define Eixo_x 26              // Pino GP26 para eixo X (ADC1)
#define joystick 22            // Pino GP22 para botão do joystick
#define LED_VERMELHO 13        // Pino GP13 para LED vermelho
#define LED_AZUL 12            // Pino GP12 para LED azul
#define LED_VERDE 11           // Pino GP11 para LED verde
#define BOTAO_A 5              // Pino GP5 para botão A
#define BOTAO_B 6              // Pino GP6 para botão B
#define BUZZER 21              // Pino GP21 para buzzer

// Configuração de debounce
#define DEBOUNCE_DELAY_MS 50   // Tempo de debounce em ms

/* ========== VARIÁVEIS GLOBAIS ========== */

// Variáveis para sensores e estados do jogo
uint percent_Eixo_x, percent_Eixo_Y;
uint Eixo_x_value, Eixo_Y_value;
uint vibrando_value = 10, corrente_value = 10;
uint valor_Eixo_Y, valor_vibracao, valor_Eixo_x, valor_corrente;
uint SISTEMA = 1, pos_x = 3, pos_y = 3;
uint Fase = 1, Fase_nova = 10, Fase_atual = 0;
uint local_x = 4, local_y = 30, numero = 24;
uint Estado_jogo = 0, iniciar_jogo = 0, tempo_esgotado = 0;
uint8_t last_pos_x = 0, last_pos_y = 0;
volatile bool flag_display = false;
uint quadrado_fase = 0; // Estilo da borda (0-5)

// Strings para estados dos sensores
const char* estado_Eixo_Y;
const char* estado_Eixo_x;
const char* estado_vibracao;
const char* estado_corrente;

// Flags para ações dos botões
volatile bool acao_botao_A = false;
volatile bool acao_botao_B = false;
volatile bool acao_botao_joystick = false;

// Variáveis para matriz de LEDs
PIO pio;                      // Controlador PIO
uint sm;                      // State Machine do PIO
uint contagem = 5;            // Número a ser exibido na matriz
ssd1306_t ssd;                // Display OLED

// Estados dos botões para debounce
bool button_a_state = false;
bool button_b_state = false;
bool button_joystick_state = false;

/* ========== DEFINIÇÕES DE MAPEAMENTO ========== */
#define MAP_x(value, in_min, in_max, out_min, out_max) \
    (59-((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min))

#define MAP_y(value, in_min, in_max, out_min, out_max) \
    ((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)

/* ========== PROTÓTIPOS DE FUNÇÕES ========== */
void Contagem_matriz_leds();
bool debounce(uint pin, bool *last_state);
void ALERTA_VITORIA();
void ALERTA_DERROTA();
void ALERTA_VITORIA_FINAL();
void desenhar_bordas(ssd1306_t *ssd);
void jogo_acerte_o_quadrado();
void Estado_do_jogo();
void gpio_irq_handler(uint gpio, uint32_t events);

/* ========== FUNÇÕES DE CONTROLE DE LEDS ========== */

// Converte intensidade em valor RGB para matriz de LEDs
uint32_t matrix_rgb(double intensity) {
    unsigned char value = intensity * 255;
    return (value << 16) | (value << 8) | value;
}

// Matriz com representações dos números 0-5 para display 5x5
double numeros[7][25] = {
    {0, 0.2, 0.2, 0.2, 0, 0.2, 0, 0, 0, 0.2, 0.2, 0, 0, 0, 0.2, 0.2, 0, 0, 0, 0.2, 0, 0.2, 0.2, 0.2, 0}, //0
    {0, 0, 0.2, 0, 0, 0, 0, 0.2, 0, 0, 0, 0, 0.2, 0, 0.2, 0, 0.2, 0.2, 0, 0, 0, 0, 0.2, 0, 0}, //1
    {0.2, 0.2, 0.2, 0.2, 0.2, 0, 0.2, 0, 0, 0, 0, 0.2, 0, 0, 0, 0.2, 0, 0, 0, 0.2, 0, 0.2, 0.2, 0.2, 0}, //2
    {0.2, 0.2, 0.2, 0.2, 0.2, 0, 0, 0, 0, 0.2, 0, 0.2, 0.2, 0.2, 0.2, 0, 0, 0, 0, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2}, //3
    {0.2, 0, 0, 0, 0, 0, 0, 0, 0, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0, 0, 0, 0.2, 0.2, 0, 0, 0, 0.2}, //4
    {0.2, 0.2, 0.2, 0.2, 0.2, 0, 0, 0, 0, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0, 0, 0, 0, 0.2, 0.2, 0.2, 0.2, 0.2}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} //tudo apagado
};

// Atualiza a matriz de LEDs com o número atual
void Contagem_matriz_leds() {
    for (int i = 0; i < NUM_PIXELS; i++) {
        uint32_t valor_led = matrix_rgb(numeros[contagem][i]);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

/* ========== FUNÇÕES DE DEBOUNCE ========== */

// Implementação do debounce para evitar leituras múltiplas
bool debounce(uint pin, bool *last_state) {
    static uint32_t last_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    bool current_state = gpio_get(pin) == 0; // Lê o estado atual

    if (current_state != *last_state) {
        if (current_time - last_time > DEBOUNCE_DELAY_MS) {
            last_time = current_time;
            *last_state = current_state;
            return current_state;
        }
    }
    return *last_state;
}

/* ========== FUNÇÕES DE ÁUDIO ========== */

// Toca alerta de vitória
void ALERTA_VITORIA() {
    // Som ascendente
    for(int i = 100; i > 50; i -= 5) {
        gpio_put(BUZZER, 1); sleep_us(i); gpio_put(BUZZER, 0); sleep_us(i);
    }
    
    // Três bips rápidos
    for(int i = 0; i < 3; i++) {
        gpio_put(BUZZER, 1); sleep_ms(100); gpio_put(BUZZER, 0); sleep_ms(50);
    }
    
    // Final vibrante
    for(int i = 0; i < 200; i++) {
        gpio_put(BUZZER, 1); sleep_us(75); gpio_put(BUZZER, 0); sleep_us(75);
    }
}

// Toca alerta de derrota
void ALERTA_DERROTA() {
    // Som descendente
    for(int i = 50; i < 200; i += 10) {
        gpio_put(BUZZER, 1); sleep_us(i); gpio_put(BUZZER, 0); sleep_us(i);
    }
    
    // Dois bips graves
    for(int i = 0; i < 2; i++) {
        gpio_put(BUZZER, 1); sleep_ms(300); gpio_put(BUZZER, 0); sleep_ms(150);
    }
    
    // Final longo
    gpio_put(BUZZER, 1); sleep_ms(600); gpio_put(BUZZER, 0); sleep_ms(100);
    gpio_put(BUZZER, 1); sleep_ms(600); gpio_put(BUZZER, 0);
}

// Toca alerta de vitória final
void ALERTA_VITORIA_FINAL() {
    // Fanfarra ascendente
    for(int note = 150; note > 50; note -= 20) {
        for(int i = 0; i < 50; i++) {
            gpio_put(BUZZER, 1); sleep_us(note); gpio_put(BUZZER, 0); sleep_us(note);
        }
        sleep_ms(20);
    }
    
    // Sequência celebratória
    int notes[] = {100, 80, 60, 80, 100};
    for(int n = 0; n < 5; n++) {
        for(int i = 0; i < 100; i++) {
            gpio_put(BUZZER, 1); sleep_us(notes[n]); gpio_put(BUZZER, 0); sleep_us(notes[n]);
        }
        sleep_ms(50);
    }
    
    // Final explosivo
    for(int i = 0; i < 400; i++) {
        gpio_put(BUZZER, 1); sleep_us(30); gpio_put(BUZZER, 0); sleep_us(30);
    }
}

/* ========== FUNÇÕES DE DISPLAY ========== */

// Desenha bordas conforme a fase atual
void desenhar_bordas(ssd1306_t *ssd) {
    switch (quadrado_fase) {
        case 1: ssd1306_rect(ssd, local_x, local_y, numero, numero, true, false); break;
        case 2: ssd1306_rect(ssd, local_x, local_y, numero, numero, true, false); break;
        case 3: ssd1306_rect(ssd, local_x, local_y, numero, numero, true, false); break;
        case 4: ssd1306_rect(ssd, local_x, local_y, numero, numero, true, false); break;
        case 5: ssd1306_rect(ssd, local_x, local_y, numero, numero, true, false); break;
    }
    ssd1306_send_data(ssd);
}

/* ========== LÓGICA DO JOGO ========== */

// Controla a lógica principal do jogo
void jogo_acerte_o_quadrado() {
    if (Fase == 0) {
        // Tela de reinício
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, "PARA REINICIAR", 10, 20);
        ssd1306_draw_string(&ssd, "PRECIONE", 25, 30);
        ssd1306_draw_string(&ssd, "O BOTAO B", 20, 40);
        ssd1306_send_data(&ssd);
        quadrado_fase = 1;
        sleep_ms(3000);
    }
    if (Fase == 1) {
        // Tela de início
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, "PIXEL HUNTER", 20, 20);
        ssd1306_draw_string(&ssd, "INICIANDO JOGO", 10, 30);
        ssd1306_draw_string(&ssd, "ACERTE DENTRO", 15, 40);
        ssd1306_draw_string(&ssd, "DO QUADRADO", 20, 50);
        ssd1306_send_data(&ssd);
        
        quadrado_fase = 1;
        local_x = 4;
        local_y = 30;
        numero = 24;
        sleep_ms(3000);

        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, "CLIQUE NO BOTAO", 2, 20);
        ssd1306_draw_string(&ssd, "JOYSTICK", 30, 30);
        ssd1306_draw_string(&ssd, "PARA INICIAR", 15, 40);
        ssd1306_draw_string(&ssd, "A RODADA", 25, 50);
        ssd1306_send_data(&ssd);
    }
    // Configurações para cada fase
    if (Fase == 2) { local_x = 20; local_y = 105; numero = 16; quadrado_fase = 2; }
    if (Fase == 3) { local_x = 40; local_y = 80; numero = 14; quadrado_fase = 3; }
    if (Fase == 4) { local_x = 50; local_y = 12; numero = 11; quadrado_fase = 4; }
    if (Fase == 5) { local_x = 9; local_y = 110; numero = 9; quadrado_fase = 5; }
    if (Fase == 6) {
        // Tela de vitória final
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, "PARABENS", 25, 20);
        ssd1306_draw_string(&ssd, "VOCE VENCEU", 20, 30);
        ssd1306_draw_string(&ssd, "TODAS AS FASES", 10, 40);
        ssd1306_send_data(&ssd);

        ALERTA_VITORIA_FINAL();
        sleep_ms(3000);
        Fase = 0;
    }
}

// Gerencia o estado do jogo (vitória/derrota)
void Estado_do_jogo() {
    if (Estado_jogo == 1) {
        // Vitória
        gpio_put(LED_VERDE, true);
        ALERTA_VITORIA();
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, "VOCE ACERTOU", 20, 20);
        
        // Exibe a fase concluída
        switch (Fase) {
            case 2: ssd1306_draw_string(&ssd, "FASE 01", 30, 30); break;
            case 3: ssd1306_draw_string(&ssd, "FASE 02", 30, 30); break;
            case 4: ssd1306_draw_string(&ssd, "FASE 03", 30, 30); break;
            case 5: ssd1306_draw_string(&ssd, "FASE 04", 30, 30); break;
            case 6: ssd1306_draw_string(&ssd, "FASE 05", 30, 30); break;
        }
        ssd1306_draw_string(&ssd, "CONCLUIDA", 20, 40);
        ssd1306_send_data(&ssd);
    } 
    else if (Estado_jogo == 2) {
        // Derrota
        char fase_str[20];
        sprintf(fase_str, "SCORE %d", Fase_atual * 200);

        gpio_put(LED_VERMELHO, true);
        ALERTA_DERROTA();
        ssd1306_fill(&ssd, false);
        
        if (tempo_esgotado == 1) {
            ssd1306_draw_string(&ssd, "TEMPO ESGOTADO", 10, 20);
        } else {
            ssd1306_draw_string(&ssd, "VOCE PERDEU", 10, 20);
        }
        
        ssd1306_draw_string(&ssd, "TENTE NOVAMENTE", 2, 30);
        ssd1306_draw_string(&ssd, fase_str, 20, 10);
        ssd1306_send_data(&ssd);

        // Reinicia o jogo
        Fase = 0;
        Fase_nova = 10;
        Estado_jogo = 0;
        pos_x = 3;
        pos_y = 3;
        quadrado_fase = 0;
    }
}

/* ========== FUNÇÃO PRINCIPAL ========== */

int main() {
    // Inicializa hardware e periféricos
    stdio_init_all();
    stdio_usb_init();
    sleep_ms(2000);

    // Configuração ADC
    adc_init();
    adc_gpio_init(Eixo_x);
    adc_gpio_init(Eixo_Y);

    // Configuração GPIO
    gpio_init(joystick);
    gpio_set_dir(joystick, GPIO_IN);
    gpio_pull_up(joystick);

    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);

    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B);

    // LEDs e buzzer
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_put(LED_VERMELHO, false);

    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_put(LED_AZUL, false);

    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_put(LED_VERDE, false);

    gpio_init(BUZZER);
    gpio_set_dir(BUZZER, GPIO_OUT);

    // Inicialização PIO para matriz de LEDs
    pio = pio0;
    uint offset = pio_add_program(pio, &animacoes_led_program);
    sm = pio_claim_unused_sm(pio, true);
    animacoes_led_program_init(pio, sm, offset, matriz_leds);

    // Configuração I2C para display
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicialização display OLED
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Configura interrupções
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(joystick, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    uint32_t last_print_time = 0;
    bool cor = true;

    // Loop principal
    while (true) {
        // Leitura dos sensores
        adc_select_input(0);
        Eixo_x_value = adc_read();
        adc_select_input(1);
        Eixo_Y_value = adc_read();
        bool sw_value = gpio_get(joystick) == 0;

        // Atualização do display quando há mudança de fase
        if (Fase != Fase_nova) {
            gpio_put(LED_VERDE, false);
            gpio_put(LED_VERMELHO, false);
            Estado_jogo = 0;
            jogo_acerte_o_quadrado();
            Fase_nova = Fase;
        }

        // Debounce dos botões
        bool button_a_pressed = debounce(BOTAO_A, &button_a_state);
        bool button_b_pressed = debounce(BOTAO_B, &button_b_state);
        bool button_joystick_pressed = debounce(joystick, &button_joystick_state);
        
        // Mapeamento das posições do joystick
        pos_x = MAP_x(Eixo_x_value, 0, 4095, 3, HEIGHT - 8);
        pos_y = MAP_y(Eixo_Y_value, 0, 4095, 3, WIDTH - 8);

        // Atualização do display quando o cursor se move
        if ((Fase != 0) && (iniciar_jogo == 1)) {
            if (pos_x != last_pos_x || pos_y != last_pos_y || flag_display) {
                last_pos_x = pos_x;
                last_pos_y = pos_y;
                
                ssd1306_fill(&ssd, false);
                ssd1306_rect(&ssd, pos_x, pos_y, 8, 8, true, true);
                desenhar_bordas(&ssd);
            }
        }

        // Lógica do botão A (verificação de acerto)
        if (acao_botao_A) {
            if ((pos_x >= local_x) && (pos_x + 8 <= local_x + numero) && 
                (pos_y >= local_y) && (pos_y + 8 <= local_y + numero)) {
                // Acertou
                contagem = 6; 
                Contagem_matriz_leds();
                iniciar_jogo = 0;
                Estado_jogo = 1;
                Fase++;
                Estado_do_jogo();
                sleep_ms(3000);
                
                ssd1306_fill(&ssd, false);
                ssd1306_draw_string(&ssd, "CLIQUE NO BOTAO", 2, 20);
                ssd1306_draw_string(&ssd, "JOYSTICK", 30, 30);
                ssd1306_draw_string(&ssd, "PARA INICIAR", 10, 40);
                ssd1306_draw_string(&ssd, "A RODADA", 25, 50);
                ssd1306_send_data(&ssd);
            } else {
                // Errou
                iniciar_jogo = 0;
                tempo_esgotado = 0;
                contagem = 6; 
                Contagem_matriz_leds();
                Estado_jogo = 2;
                quadrado_fase = 1;
                Estado_do_jogo();
                sleep_ms(3000);
            }
            acao_botao_A = false;
        }

        // Lógica do botão B (reinício)
        if (acao_botao_B) {
            Fase = 1;
            Estado_jogo = 0;
            quadrado_fase = 1;
            Fase_nova = 10;
            pos_x = 3;
            pos_y = 3;
            Estado_do_jogo();
            acao_botao_B = false;
            contagem = 6; 
            tempo_esgotado = 0;
            Contagem_matriz_leds();
        }

        // Lógica do botão do joystick (iniciar jogo)
        if (acao_botao_joystick) {
            iniciar_jogo = 1;
            acao_botao_joystick = false;
            contagem = 5;
        }

        // Atualização da fase atual
        if (Fase == 0) {
            Fase_atual = 0;
        } else {
            Fase_atual = Fase - 1;
        }

        // Atualização periódica (1 segundo)
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        if (current_time - last_print_time >= 1000) {
            printf("Fase Atual: %u, Tempo: %d\n", Fase_atual, contagem);
            printf("Pontos: %d\n", Fase_atual * 200);
            
            // Lógica do temporizador
            if(iniciar_jogo == 1) {
                Contagem_matriz_leds();
                if ((contagem == 0) && (Estado_jogo == 0)) {
                    tempo_esgotado = 1;
                    contagem = 6; 
                    Contagem_matriz_leds();
                    iniciar_jogo = 0;
                    Estado_jogo = 2;
                    quadrado_fase = 1;
                    Estado_do_jogo();
                    sleep_ms(3000);
                }
                contagem--;
            }

            last_print_time = current_time;
        }
    }
}

/* ========== HANDLER DE INTERRUPÇÃO ========== */

// Tratamento das interrupções dos botões
void gpio_irq_handler(uint gpio, uint32_t events) {
    static uint32_t last_time = 0;
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Debouncing de 300ms
    if (current_time - last_time > 300000) {
        last_time = current_time;

        // Botão A pressionado
        if (gpio == BOTAO_A && !gpio_get(BOTAO_A)) {
            acao_botao_A = true;
        }

        // Botão B pressionado
        if (gpio == BOTAO_B && !gpio_get(BOTAO_B)) {
            acao_botao_B = true;
        }

        // Botão do joystick pressionado
        if (gpio == joystick && !gpio_get(joystick)) {
            acao_botao_joystick = true;
        }
    }
}