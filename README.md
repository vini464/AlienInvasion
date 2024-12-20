# Alien Invasion
Este projeto trata-se de um jogo 1v1 onde um dos jogadores controla uma nave espacial invasora e o outro controla um tanque de guerra. O objetivo de cada um é destruir seu inimigo, afinal estamos em guerra! Para balancear o jogo, a nave tem menos vida que o tanque, em compensação o tanque tem seu movimento limitado. Chame seu amigo pegue sua fpga e vamos jogar!

## Instalação:
Primeiramete baixe os arquivos do jogo disponíveis [aqui](https://github.com/vini464/AlienInvasion/releases/tag/v1.0.1) e os extraia, ou clone este repositório.
Agora, entre na pasta code dentro da pasta do projeto. 
Por fim, para compilar e rodar o jogo basta executar esse comando: ```make game```

*Obs: Você precisa de previlégios de administrador para rodar o jogo.

## Como jogar Alien Invasion
Antes de mais nada, vamos conhecer nossos controles:
Na placa, temos o acelerômetro (com ele que movimentamos o tanque), e 4 botões:
--[[mostrar aqui foto da placa (colocar numero nos botões)]]
- O botão 1 atira;
- O botão 2 pausa e retoma o jogo;
- O botão 3 reinicia o jogo (somente quando pausado ou quando um dos jogadores venceu);
- O botão 4 sai do jogo (apenas quando pausado ou quando um dos jogadores venceu).

Nosso outro controle é um mouse:
-- [[mostrar aqui uma foto do mouse]]
- Arrastá-lo movimenta a nave;
- O botão esquerdo atira.

Todo o controle do fluxo do jogo é realizado por meio dos botões da placa.
Quando o jogo inicia a seguinte tela é mostrada 
--[[mostrar aqui a tela inicial]]
Esta tela só é trocada quando um dos botões da placa é pressionado.
E assim começa o jogo. Derrote seu inimigo! Vamos supor que você controla a nave, se você vencer a seguinte tela é mostrada:
--[[mostrar tela alien wins]]
Agora, se você perder esta outra tela aparece:
--[[mostrar a tela human wins]]

Em qualquer uma dessas duas telas você pode precionar o botão 3 para reiniciar o jogo, ou o botão 4 para sair.

Lembrando que: A qualquer momento em que os jogadores estejam vivos, o botão dois pode ser pressionado para pausar o jogo e então poder reiniciar ou sair do jogo (contamos com a integridade do jogador que segura a placa para não reiniciar o jogo "sem querer" quando estiver perdendo).

Divirtam-se!!
