PixelHunter - Jogo de Precis�o para BitDogLab

Descri��o
O PixelHunter � um jogo eletr�nico interativo desenvolvido para a placa BitDogLab com microcontrolador RP2040, que testa a coordena��o motora e tempo de rea��o do jogador. O objetivo � posicionar um cursor dentro de �reas espec�ficas (quadrados) exibidas no display OLED, utilizando um joystick anal�gico. O sistema inclui exibi��o gr�fica no display OLED, feedback visual com LEDs RGB, alertas sonoros com buzzer e contagem regressiva em matriz de LEDs.

O jogo � capaz de oferecer desafios progressivos, com fases que aumentam em dificuldade, e fornecer feedback imediato sobre o desempenho do jogador.

Objetivos
Criar um jogo interativo com controles intuitivos via joystick
Implementar sistema de fases progressivas com aumento de dificuldade
Fornecer feedback visual atrav�s do display OLED e LEDs RGB
Implementar alertas sonoros para acertos e erros
Desenvolver um sistema de contagem regressiva usando matriz de LEDs
Utilizar interrup��es para tratamento eficiente dos bot�es

Componentes Utilizados
Componente - Conex�o BitDogLab - Fun��o no Projeto

BitDogLab ? Microcontrolador RP2040 ? Placa base

Display OLED ? Exibe interface do jogo e gr�ficos ? Conectado via I2C (GP14 - SDA, GP15 - SCL)

Joystick ? Controla o cursor do jogador ? Entradas anal�gicas GP26 (X) e GP27 (Y)

Bot�o do Joystick ? Inicia o jogo / fases ? GP22

Bot�o A ? Confirma a posi��o do cursor ? GP5

Bot�o B ? Reinicia o jogo ? GP6

LED RGB ? Indica status (verde: vit�ria, vermelho: derrota) ? GP11 (verde), GP12 (azul), GP13 (vermelho)

Matriz de LEDs ? Exibe contagem regressiva do tempo ? Conectada ao GP7 via PIO

Buzzer ? Reproduz sons de vit�ria ou derrota ? GP21

Funcionamento
1. Menu Inicial
Exibi��o no display OLED:

PIXELHUNTER
Pressione BOT�O JOYSTICK
Fase: 1
O bot�o do joystick inicia o jogo

2. Fases de Jogo
Cada fase apresenta um quadrado em posi��o aleat�ria

O jogador controla um cursor (8x8 pixels) usando o joystick

Para avan�ar, deve posicionar o cursor dentro do quadrado e pressionar o bot�o A

Dificuldade progressiva:

Fase 1: Quadrado 24x24 pixels

Fase 5: Quadrado 9x9 pixels

3. Sistema de Feedback
Acerto:

LED verde acende

Buzzer emite som de vit�ria

Avan�a para pr�xima fase

Erro/Tempo esgotado:

LED vermelho acende

Buzzer emite som de derrota

Reinicia a fase atual

4. Contagem Regressiva
Matriz de LEDs exibe contagem de 5 a 0

Cada n�mero permanece por 1 segundo

Tempo esgotado resulta em derrota

5. Vit�ria Final
Ao completar todas as fases:

Buzzer toca melodia comemorativa

Display mostra mensagem de parab�ns

6. Controles
Joystick: Move o cursor na tela

Bot�o A: Confirma posi��o do cursor

Bot�o B: Reinicia o jogo a qualquer momento

Bot�o Joystick: Inicia jogo no menu

Estrutura do C�digo
Inicializa��o:

Configura��o de GPIOs, ADC, I2C e PIO

Inicializa��o do display OLED

Carregamento dos padr�es para matriz de LEDs

Leitura de Entradas:

Joystick (eixos X e Y)

Bot�es (com debounce de 50ms)

L�gica do Jogo:

Controle do cursor

Verifica��o de colis�o (cursor dentro do quadrado)

Gerenciamento de fases

Contagem regressiva

Sa�das:

Atualiza��o do display OLED

Controle dos LEDs RGB

Ativa��o do buzzer

Atualiza��o da matriz de LEDs

Rotinas de Interrup��o:

Tratamento dos bot�es

Temporiza��o precisa

Como Executar o Projeto
Conectar os componentes conforme a tabela de conex�es

Compilar e carregar o c�digo no microcontrolador RP2040

Opera��o:

Movimentar o joystick para controlar o cursor

Pressionar bot�o A para confirmar posi��o

Pressionar bot�o B para reiniciar

Observar feedback no display, LEDs e buzzer

Autor:
Nome: Mateus Moreira da Silva

Reposit�rio: GitHub

V�deo de Demonstra��o: YouTube