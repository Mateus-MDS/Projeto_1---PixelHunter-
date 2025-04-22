PixelHunter - Jogo de Precisão para BitDogLab

Descrição
O PixelHunter é um jogo eletrônico interativo desenvolvido para a placa BitDogLab com microcontrolador RP2040, que testa a coordenação motora e tempo de reação do jogador. O objetivo é posicionar um cursor dentro de áreas específicas (quadrados) exibidas no display OLED, utilizando um joystick analógico. O sistema inclui exibição gráfica no display OLED, feedback visual com LEDs RGB, alertas sonoros com buzzer e contagem regressiva em matriz de LEDs.

O jogo é capaz de oferecer desafios progressivos, com fases que aumentam em dificuldade, e fornecer feedback imediato sobre o desempenho do jogador.

Objetivos
Criar um jogo interativo com controles intuitivos via joystick
Implementar sistema de fases progressivas com aumento de dificuldade
Fornecer feedback visual através do display OLED e LEDs RGB
Implementar alertas sonoros para acertos e erros
Desenvolver um sistema de contagem regressiva usando matriz de LEDs
Utilizar interrupções para tratamento eficiente dos botões

Componentes Utilizados
Componente - Conexão BitDogLab - Função no Projeto

BitDogLab ? Microcontrolador RP2040 ? Placa base

Display OLED ? Exibe interface do jogo e gráficos ? Conectado via I2C (GP14 - SDA, GP15 - SCL)

Joystick ? Controla o cursor do jogador ? Entradas analógicas GP26 (X) e GP27 (Y)

Botão do Joystick ? Inicia o jogo / fases ? GP22

Botão A ? Confirma a posição do cursor ? GP5

Botão B ? Reinicia o jogo ? GP6

LED RGB ? Indica status (verde: vitória, vermelho: derrota) ? GP11 (verde), GP12 (azul), GP13 (vermelho)

Matriz de LEDs ? Exibe contagem regressiva do tempo ? Conectada ao GP7 via PIO

Buzzer ? Reproduz sons de vitória ou derrota ? GP21

Funcionamento
1. Menu Inicial
Exibição no display OLED:

PIXELHUNTER
Pressione BOTÃO JOYSTICK
Fase: 1
O botão do joystick inicia o jogo

2. Fases de Jogo
Cada fase apresenta um quadrado em posição aleatória

O jogador controla um cursor (8x8 pixels) usando o joystick

Para avançar, deve posicionar o cursor dentro do quadrado e pressionar o botão A

Dificuldade progressiva:

Fase 1: Quadrado 24x24 pixels

Fase 5: Quadrado 9x9 pixels

3. Sistema de Feedback
Acerto:

LED verde acende

Buzzer emite som de vitória

Avança para próxima fase

Erro/Tempo esgotado:

LED vermelho acende

Buzzer emite som de derrota

Reinicia a fase atual

4. Contagem Regressiva
Matriz de LEDs exibe contagem de 5 a 0

Cada número permanece por 1 segundo

Tempo esgotado resulta em derrota

5. Vitória Final
Ao completar todas as fases:

Buzzer toca melodia comemorativa

Display mostra mensagem de parabéns

6. Controles
Joystick: Move o cursor na tela

Botão A: Confirma posição do cursor

Botão B: Reinicia o jogo a qualquer momento

Botão Joystick: Inicia jogo no menu

Estrutura do Código
Inicialização:

Configuração de GPIOs, ADC, I2C e PIO

Inicialização do display OLED

Carregamento dos padrões para matriz de LEDs

Leitura de Entradas:

Joystick (eixos X e Y)

Botões (com debounce de 50ms)

Lógica do Jogo:

Controle do cursor

Verificação de colisão (cursor dentro do quadrado)

Gerenciamento de fases

Contagem regressiva

Saídas:

Atualização do display OLED

Controle dos LEDs RGB

Ativação do buzzer

Atualização da matriz de LEDs

Rotinas de Interrupção:

Tratamento dos botões

Temporização precisa

Como Executar o Projeto
Conectar os componentes conforme a tabela de conexões

Compilar e carregar o código no microcontrolador RP2040

Operação:

Movimentar o joystick para controlar o cursor

Pressionar botão A para confirmar posição

Pressionar botão B para reiniciar

Observar feedback no display, LEDs e buzzer

Autor:
Nome: Mateus Moreira da Silva

Repositório: GitHub: https://github.com/Mateus-MDS/Projeto_1---PixelHunter-.git

Vídeo de Demonstração: YouTube: https://youtu.be/bcOXOqESEd0
