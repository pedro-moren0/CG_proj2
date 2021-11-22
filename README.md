# Projeto 2 - Computação Gráfica

[Link para o Github pages com a aplicação](https://pedro-moren0.github.io/CG_proj2/proj2/)
AVISO: Há um bug no webassembly que torna os inimigos invisíveis. Fora isso tudo funciona normalmente.

## Introdução

O objetivo desse trabalho era a realização de uma aplicação envolvendo os conceitos de computação gráfica e as ferramentas do OpenGL, agora utilizando o espaço 3D.

Nesse projeto nos baseamos no conceito dos jogos de espionagem, notavelmente os da série Metal Gear, para desenvolver um jogo usando o espaço tridimensional do OpenGL.

## Overview do projeto

Assim como nos projetos anteriores, grande parte do código fonte teve como base os códigos disponibilizados pelos professores. No caso desse projeto, valemo-nos em grande parte do código do capítulo de câmera Look At [1].
Os arquivos ground.cpp (e .hpp), camera.cpp (e.hpp) e main.cpp apresentam essencialmente as mesmas funcionalidades explicitadas no site da disciplina.
Basta-nos dizer que camera.cpp recebe as matrizes MVP (Model, View, Projection) e transforma as coordenadas dos objetos no mundo em uma visão perspectiva (em primeira pessoa). As únicas modificações feitas nesses três arquivos foram que: em ground, alteramos a coloração do chão para ciano ao invés de cinza, e pintamos o quadrado central em vermelho; em camera, diminuímos o ângulo do FOV; e em main, desabilitamos o contador de FPS e o botão de tela inteira, e ativamos o vsync.

Em bunny.cpp, agregamos a função loadModel para carregar modelos tridimensionais em malha. Especificamente nesse caso, carregamos o modelo do Stanford Bunny. Definimos uma posição inicial para o coelho, e o transladamos para tal posição usando a função translate, do GLM. Além disso, bunny.cpp é capaz de calcular se o jogador o "salvou" (isto é, quando a posição de m_eye de camera.cpp sobrevoa a área do quadrado em que o coelho está), e caso tal propriedade seja verdadeira, não mais desenhar o coelho na cena, como se o jogador tivesse de fato o pegado.

Em enemy.cpp, utilizamos o modelo de um triângulo bidimensional para representar os inimigos. A escolha por um modelo bidimensional nesse caso foi proposital por conta do fragment shader, onde caso a face desenhada seja interna, ela é desenhada mais escura. Usamos essa propriedade para simular como o caso do inimigo ficar de costas. Esse efeito não seria tão simples de se fazer utilizando um modelo 3D.

Também vale ressaltar que a escolha de um triângulo é pura conveniência. Qualquer modelo bidimensional pode ser utilizado fazendo as devidas alterações em enemy.cpp.

paintGL de enemy recebe parâmetros como posição e fase (ângulo inicial), sendo capaz de desenhar qualquer inimigo em qualquer posição, com qualquer ângulo de rotação. Além disso, se fase é um ângulo que varia com o tempo, os modelos dos inimigos também o fazem. Além disso, a classe consegue definir se o jogador está dentro do campo de visão do inimigo, por meio da função isDetected. O funcionamento dessa função é muito simples: definimos qualquer ponto de referência, no caso o ponto onde o triângulo toca o chão, e definimos também um vetor de referência, geralmente o versor k, e por meio da propriedade de que arc cos (<u|v>) = angulo entre u e v, (se e somente se u e v forem normalizados), fica fácil dizer se o jogador está ou não dentro do campo de visão.

Finalmente, criamos um controlador de jogo, que instancia enemy e bunny e controla os estados do jogo (i.e, ganhou ou perdeu), e usamos a função update de openglwindow para alterar entre os estados do jogo. paintUI de openglwindow se responsabilizou por criar os pop-ups de ganhou ou perdeu, bem como informar o jogador sobre sua vida e posição no tabeleiro, além dos controles e regras do jogo.

## Futuro do Projeto

Algumas alterações que poderiam ocorrer para o melhoramento do projeto são apresentadas aqui.

Primeiramente, o comportamento dos inimigos poderia ser melhorado. Adicionar translações e rotações no sentido anti-horário adicionaria a dificuldade do jogo. Além disso, poderíamos incluir e funcionalidade de que quando o jogador é visto, a sua câmera rotaciona para mostrar qual inimigo o viu. Além de que poderíamos adicionar paredes ao jogo, bem como limitar o movimento do jogador nessas áreas.

## Conclusão

Apesar de não ser um grande jogo, podemos mostrar certo domínio do espaço tridimensonal do OpenGL, por meio das funções de detecção dos inimigos e do uso das matrizes de rotação e translação, o que era o objetivo da atividade.

## Referências

[1] [Notas de aula sobre câmera Look At](https://hbatagelo.github.io/cg/lookatproject.html)
