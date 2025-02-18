# Projeto 10: **ADC na RP2040**
**EMBARCATECH - UNIDADE 04**

## Aluno: 
- **Carlos Henrique Silva Lopes**

## **Link do vídeo de Ensaio**
https://drive.google.com/file/d/1Gk6JlI6aHrq8O4bVZ-2WSq2am1uBoFyU/view?usp=drivesdk

## **Descrição do Projeto**
Este repositório contém um exemplo de código para o Raspberry Pi Pico que integra um display SSD1306, um joystick (analógico com botão) e LEDs RGB controlados via PWM.

## Funcionalidades

- **Display SSD1306 (I2C):**
  - Exibe um quadrado móvel que se posiciona de acordo com os valores lidos do joystick.
  - Desenha uma borda que pode ser "engrossada" (quando acionada) para feedback visual.

- **Joystick:**
  - Utiliza ADC para ler os eixos X e Y (GPIO26 e GPIO27).
  - Possui um botão (GPIO22) que, ao ser pressionado, alterna o estado do LED verde e ativa/desativa a borda grossa.

- **LEDs com PWM:**
  - **LED Vermelho:** O brilho é ajustado de acordo com o desvio do eixo X.
  - **LED Azul:** O brilho é ajustado de acordo com o desvio do eixo Y.
  - **LED Verde:** Alterna seu estado (ligado/desligado) quando o botão do joystick (GPIO22) é pressionado.
  - Implementa uma *zona morta* (dead zone) em torno do centro (valor 2048) para evitar que os LEDs acendam indevidamente quando o joystick está em repouso.

- **Botão Adicional (GPIO5):**
  - Permite alternar o estado geral dos LEDs (desliga/ativa) com debounce.


## Configuração do Projeto

1. Clone este repositório para sua máquina:

   ```
   https://github.com/CarlosHenriqueSL/Projeto-10-EmbarcaTech.git
   cd Projeto-10-EmbarcaTech
   cd ProjetoADC
   ```

2. Certifique-se de que o **Pico SDK** está configurado no VS Code.

3. Abra o projeto no **VS Code** e configure os arquivos `CMakeLists.txt` adequados para compilar o código.

4. Execute o projeto no simulador **Wokwi** ou faça o upload para o hardware físico do Raspberry Pi Pico W.


## Estrutura do Código

### Principais Arquivos
- **`ProjetoADC.c:`** Contém a lógica principal para fazer a leitura do joystick, dos botões, acionamento dos LEDs e desenhos no display.
- **`diagram.json:`** Configuração física do projeto no simulador Wokwi.
- **`inc/:`** Pasta onde fica a lógica para desenhar no display ssd1306.
- **`README.md:`** Documentação detalhada do projeto.


## Testes e Validação

1. Ao mexer o joystick no eixo X, o LED vermelho deve acender, o mesmo ocorre para o LED azul no eixo Y. Além disso, o quadrado no display deve se mover conforme o joystick.

2. Ao apertar o botão do joystick (GPIO 22), o LED verde deve alterar de estado, assim como a borda do display deve mudar.

3. Ao apertar o botão A (GPIO 5), todos os LEDs devem se apagar.

