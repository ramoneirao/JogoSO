# Estrutura Geral do Código
O código tem dois arquivos:

jogo_da_velha.cpp: Arquivo que contém a classe que controla o tabuleiro e a lógica do jogo.
jogo_da_velha.h: Cabeçalho que define a estrutura da classe e suas funções.

# Funções Principais do Jogo
A classe JogoDaVelha contém as seguintes funções:

Construtor (JogoDaVelha): Inicializa o tabuleiro e define os semáforos para o controle dos turnos dos jogadores.
inicializar_tabuleiro: Preenche o tabuleiro com espaços vazios.
mostrar_tabuleiro: Exibe o tabuleiro atual no console.
verificar_vencedor e verificar_empate: Verificam se algum jogador venceu ou se houve empate.
jogador1 e jogador2: Controlam as ações dos jogadores 1 e 2, respectivamente.
iniciar_jogo: Inicia as threads para cada jogador e controla o ciclo de jogo.

# Uso de Threads
Cada jogador é representado por uma thread:

std::thread t1(&JogoDaVelha::jogador1, this);: Inicia a thread para o jogador 1.

std::thread t2(&JogoDaVelha::jogador2, this);: Inicia a thread para o jogador 2.

Essas threads permitem que cada jogador jogue em paralelo. No entanto, é preciso garantir que apenas um jogador jogue de cada vez. Para isso, são usados semáforos e um mutex para controlar o acesso ao tabuleiro.

# Uso de Semáforos (sem_t)
Os semáforos (sem_jogador1 e sem_jogador2) controlam o turno dos jogadores:

sem_init: Inicializa os semáforos para que o jogador 1 comece (semáforo do jogador 1 é iniciado com valor 1 e o do jogador 2 com valor 0).

sem_wait: Suspende a thread até que o semáforo tenha um valor maior que zero. Em jogador1, sem_wait(&sem_jogador1); faz o jogador 1 aguardar seu turno; em jogador2, sem_wait(&sem_jogador2); faz o jogador 2 aguardar.

sem_post: Incrementa o semáforo e libera a thread. Em jogador1, sem_post(&sem_jogador2); permite que o jogador 2 jogue e vice-versa.

sem_destroy: Destrói os semáforos ao final do jogo, liberando os recursos do sistema.

# Controle de Acesso com std::mutex
Para garantir que apenas um jogador acesse o tabuleiro por vez, o código usa std::mutex:

std::lock_guard \<std::mutex> lock(mtx);: Protege o acesso ao tabuleiro na função mostrar_tabuleiro para evitar que múltiplas threads imprimam ao mesmo tempo.

mtx.lock() e mtx.unlock(): Usados nos métodos dos jogadores para garantir que somente um jogador faça uma jogada por vez. lock() bloqueia o mutex, e unlock() o libera.

# Lógica das Funções jogador1 e jogador2
As funções jogador1 e jogador2 são responsáveis por permitir que cada jogador faça uma jogada:

A thread do jogador chama sem_wait() para verificar se é sua vez.

A função lê a entrada do jogador, verifica se a posição é válida e atualiza o tabuleiro.
Depois, mostrar_tabuleiro exibe o tabuleiro atualizado, e são verificados se houve vitória ou empate.

Por fim, sem_post() sinaliza que o turno passa para o outro jogador.