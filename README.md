# Atividade_02_02

**Objetivo**

Este projeto tem como objetivo consolidar o entendimento dos conceitos de interrupções, debouncing de botões e controle de LEDs no microcontrolador RP2040, utilizando a placa de desenvolvimento BitDogLab.

**Componentes**

Matriz de LEDs 5x5 WS2812: GPIO 7.
LED RGB vermelho: GPIO 13.
Botão A: GPIO 5.
Botão B: GPIO 6.

**Funcionalidades**

1. LED RGB:
O LED vermelho pisca 5 vezes por segundo.

2. Botão A (GPIO 5):
Incrementa o número exibido na matriz de LEDs cada vez que é pressionado.

3. Botão B (GPIO 6):
Decrementa o número exibido na matriz de LEDs cada vez que é pressionado.

4. Matriz de LEDs WS2812:
Exibe números de 0 a 9 na matriz 5x5.

**Requisitos do Projeto**

- Uso de interrupções: as funcionalidades dos botões são implementadas através de rotinas de interrupção (IRQ).
- Debouncing: o tratamento de bouncing nos botões é realizado via software com um atraso de 300 ms entre cada leitura.
- Controle de LEDs: o código manipula o LED vermelho e LEDs endereçáveis WS2812.
- Organização do código: o código foi estruturado e comentado para facilitar a compreensão.

**Funcionamento**

1. O LED vermelho pisca 5 vezes a cada segundo.
2. É feito a exibição de 0 a 9 na matriz de LEDs WS2812 5x5.
3. Ao pressionar o botão A, o número exibido na matriz de LEDs é incrementado.
4. Ao pressionar o botão B, o número exibido na matriz de LEDs é decrementado.

**Como Usar**

1. Compile o código utilizando o VSCode.
2. Carregue o código na placa BitDogLab.
3. Pressione os botões A ou B para incrementar ou decrementar o número exibido na matriz de LEDs.

**LINK DO VÍDEO DE FUNCIONAMENTO:** 