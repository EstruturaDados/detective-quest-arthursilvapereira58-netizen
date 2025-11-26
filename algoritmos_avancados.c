#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definição do tamanho da Tabela Hash (Número de 'buckets')
#define TAMANHO_HASH 10
#define MAX_NOME 50
#define MAX_PISTA 100

// --- Estruturas de Dados ---

// 1. Estrutura para os Cômodos (Nó da Árvore Binária da Mansão)
typedef struct Comodo {
    char nome[MAX_NOME];
    char pista[MAX_PISTA]; // Pista estática associada ao cômodo
    struct Comodo *esquerda;
    struct Comodo *direita;
} Comodo;

// 2. Estrutura para as Pistas Coletadas (Nó da Árvore BST)
typedef struct PistaColetada {
    char detalhe[MAX_PISTA];
    struct PistaColetada *esquerda;
    struct PistaColetada *direita;
} PistaColetada;

// 3. Estrutura para a Tabela Hash (Lista Encadeada para colisões)
typedef struct HashItem {
    char pista[MAX_PISTA];
    char suspeito[MAX_NOME];
    struct HashItem *proximo;
} HashItem;

// Tabela Hash é um array de ponteiros para HashItem
HashItem *tabelaHash[TAMANHO_HASH];

// --- Protótipos das Funções ---

// Funções da Mansão (Árvore Binária)
Comodo* criarSala(const char *nome, const char *pista);
void explorarSalas(Comodo *atual, PistaColetada **bstPistas, Comodo *raiz);

// Funções da Pista (Árvore BST)
PistaColetada* adicionarPista(PistaColetada *raiz, const char *detalhe);
void listarPistas(PistaColetada *raiz);

// Funções da Tabela Hash
unsigned int funcaoHash(const char *chave);
void inserirNaHash(const char *pista, const char *suspeito);
char* encontrarSuspeito(const char *pista);
void inicializarHash();

// Funções do Julgamento
int contarPistasPorSuspeito(PistaColetada *bstPistas, const char *suspeitoAcusado);
void verificarSuspeitoFinal(PistaColetada *bstPistas);

// Funções Auxiliares
void liberarMansao(Comodo *raiz);
void liberarPistas(PistaColetada *raiz);
void liberarHash();

// --- Implementação das Funções ---

// ## Funções da Mansão (Árvore Binária)

/**
 * @brief Cria dinamicamente um novo cômodo.
 * * Aloca memória para um novo cômodo, inicializa seu nome e pista
 * e define seus ponteiros filhos como NULL.
 * * @param nome O nome exclusivo do cômodo.
 * @param pista A pista estática associada a este cômodo.
 * @return Um ponteiro para o novo cômodo criado.
 */
Comodo* criarSala(const char *nome, const char *pista) {
    Comodo *novo = (Comodo *)malloc(sizeof(Comodo));
    if (novo == NULL) {
        perror("Erro ao alocar memória para o cômodo");
        exit(EXIT_FAILURE);
    }
    strncpy(novo->nome, nome, MAX_NOME - 1);
    novo->nome[MAX_NOME - 1] = '\0';
    strncpy(novo->pista, pista, MAX_PISTA - 1);
    novo->pista[MAX_PISTA - 1] = '\0';
    novo->esquerda = NULL;
    novo->direita = NULL;
    return novo;
}

/**
 * @brief Navega pela árvore da mansão e ativa o sistema de pistas.
 * * Permite ao jogador escolher entre ir para a esquerda, direita ou sair,
 * ativando a coleta de pistas no cômodo atual.
 * * @param atual O cômodo atual na exploração.
 * @param bstPistas O ponteiro para a raiz da BST de pistas coletadas.
 * @param raiz A raiz da árvore da mansão (para retornar ao início).
 */
void explorarSalas(Comodo *atual, PistaColetada **bstPistas, Comodo *raiz) {
    char escolha;

    if (atual == NULL) {
        printf("\nVocê não pode ir para lá. Volte ao menu principal (Raiz da Mansão).\n");
        return;
    }
    
    printf("\n--- Você está no(a) **%s** ---\n", atual->nome);

    // 1. Identificar e exibir a pista
    if (strlen(atual->pista) > 0) {
        printf("🕵️‍♂️ **PISTA ENCONTRADA:** \"%s\"\n", atual->pista);
        
        // 2. Armazenar a pista na BST
        *bstPistas = adicionarPista(*bstPistas, atual->pista);
        printf("A pista foi adicionada ao seu diário de investigação (BST).\n");
        
        // 3. Associar a pista a um suspeito na Tabela Hash
        char *suspeito = encontrarSuspeito(atual->pista);
        if (suspeito != NULL) {
            printf("🔍 **Suspeito Associado:** O detetive desconfia que esta pista aponta para **%s**.\n", suspeito);
        } else {
            printf("🤔 Esta pista ainda não foi associada a um suspeito no sistema.\n");
        }
    } else {
        printf("Parece que não há nenhuma pista relevante neste cômodo.\n");
    }

    // 4. Permitir a exploração interativa
    do {
        printf("\nPara onde deseja ir? **(e)**: Esquerda | **(d)**: Direita | **(r)**: Retornar à Raiz | **(s)**: Sair e Acusar o Culpado\n");
        printf("Escolha: ");
        if (scanf(" %c", &escolha) != 1) {
            // Lidar com possíveis erros de leitura
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n'); // Limpar o buffer de entrada
        
        escolha = tolower(escolha);
        
        if (escolha == 'e') {
            if (atual->esquerda != NULL) {
                explorarSalas(atual->esquerda, bstPistas, raiz);
                return; // Retorna após a recursão
            } else {
                printf("Caminho Bloqueado: Não há cômodo à esquerda.\n");
            }
        } else if (escolha == 'd') {
            if (atual->direita != NULL) {
                explorarSalas(atual->direita, bstPistas, raiz);
                return; // Retorna após a recursão
            } else {
                printf("Caminho Bloqueado: Não há cômodo à direita.\n");
            }
        } else if (escolha == 'r') {
            if (atual != raiz) {
                explorarSalas(raiz, bstPistas, raiz); // Retorna à raiz
                return; // Retorna após a recursão
            } else {
                printf("Você já está na Raiz (Entrada Principal).\n");
            }
        } else if (escolha == 's') {
            printf("\n🚨 **Saindo da Exploração...** Hora do Julgamento!\n");
            return; // Encerra a exploração e volta ao main
        } else {
            printf("Opção inválida. Tente novamente.\n");
        }
    } while (escolha != 's');
}

// ## Funções da Pista (Árvore BST)

/**
 * @brief Insere a pista coletada na Árvore BST de forma ordenada.
 * * Utiliza recursão para manter a propriedade de Árvore de Busca Binária (BST)
 * baseada na ordem alfabética da string da pista.
 * * @param raiz A raiz (ou sub-raiz) atual da BST.
 * @param detalhe A string da pista a ser inserida.
 * @return O ponteiro para a nova raiz (ou sub-raiz) após a inserção.
 */
PistaColetada* adicionarPista(PistaColetada *raiz, const char *detalhe) {
    // 1. Caso base: Se a árvore estiver vazia, cria um novo nó
    if (raiz == NULL) {
        PistaColetada *novaPista = (PistaColetada *)malloc(sizeof(PistaColetada));
        if (novaPista == NULL) {
            perror("Erro ao alocar memória para a pista");
            exit(EXIT_FAILURE);
        }
        strncpy(novaPista->detalhe, detalhe, MAX_PISTA - 1);
        novaPista->detalhe[MAX_PISTA - 1] = '\0';
        novaPista->esquerda = NULL;
        novaPista->direita = NULL;
        return novaPista;
    }

    // 2. Compara a nova pista com o nó atual
    int comparacao = strcmp(detalhe, raiz->detalhe);

    if (comparacao < 0) {
        // Nova pista é "menor" (vem antes no alfabeto): vai para a sub-árvore esquerda
        raiz->esquerda = adicionarPista(raiz->esquerda, detalhe);
    } else if (comparacao > 0) {
        // Nova pista é "maior" (vem depois no alfabeto): vai para a sub-árvore direita
        raiz->direita = adicionarPista(raiz->direita, detalhe);
    }
    // Se comparacao == 0, a pista já existe. Não faz nada (evita duplicatas).

    return raiz;
}

/**
 * @brief Lista todas as pistas coletadas em ordem alfabética (percurso In-ordem).
 * * @param raiz A raiz da BST de pistas coletadas.
 */
void listarPistas(PistaColetada *raiz) {
    if (raiz != NULL) {
        listarPistas(raiz->esquerda); // Visita a sub-árvore esquerda (menores)
        printf("- %s\n", raiz->detalhe); // Visita o nó atual
        listarPistas(raiz->direita); // Visita a sub-árvore direita (maiores)
    }
}

// ## Funções da Tabela Hash

/**
 * @brief Inicializa todos os 'buckets' da tabela hash para NULL.
 */
void inicializarHash() {
    for (int i = 0; i < TAMANHO_HASH; i++) {
        tabelaHash[i] = NULL;
    }
}

/**
 * @brief Função Hash simples para calcular o índice (bucket) no array.
 * * Soma os valores ASCII dos caracteres da chave (pista) e aplica o módulo
 * com o tamanho da tabela para obter um índice válido.
 * * @param chave A string da pista.
 * @return O índice (bucket) na tabela hash.
 */
unsigned int funcaoHash(const char *chave) {
    unsigned int hash = 0;
    for (int i = 0; chave[i] != '\0'; i++) {
        hash = hash + chave[i];
    }
    return hash % TAMANHO_HASH;
}

/**
 * @brief Insere a associação pista/suspeito na tabela hash.
 * * Calcula o índice, aloca um novo item, preenche os dados e insere no
 * início da lista encadeada no índice (resolução de colisão por encadeamento).
 * * @param pista A string da pista (chave).
 * @param suspeito O nome do suspeito (valor).
 */
void inserirNaHash(const char *pista, const char *suspeito) {
    unsigned int indice = funcaoHash(pista);

    // Cria o novo item
    HashItem *novoItem = (HashItem *)malloc(sizeof(HashItem));
    if (novoItem == NULL) {
        perror("Erro ao alocar memória para o item Hash");
        exit(EXIT_FAILURE);
    }
    strncpy(novoItem->pista, pista, MAX_PISTA - 1);
    novoItem->pista[MAX_PISTA - 1] = '\0';
    strncpy(novoItem->suspeito, suspeito, MAX_NOME - 1);
    novoItem->suspeito[MAX_NOME - 1] = '\0';

    // Insere no início da lista encadeada
    novoItem->proximo = tabelaHash[indice];
    tabelaHash[indice] = novoItem;
}

/**
 * @brief Consulta o suspeito correspondente a uma pista.
 * * Calcula o índice e percorre a lista encadeada no 'bucket' até
 * encontrar a pista correspondente.
 * * @param pista A string da pista (chave) a ser procurada.
 * @return O nome do suspeito, ou NULL se a pista não for encontrada.
 */
char* encontrarSuspeito(const char *pista) {
    unsigned int indice = funcaoHash(pista);
    HashItem *atual = tabelaHash[indice];

    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito;
        }
        atual = atual->proximo;
    }
    return NULL; // Não encontrado
}

// ## Funções do Julgamento

/**
 * @brief Função recursiva para contar quantas pistas na BST apontam para um suspeito.
 * * Percorre a BST, consulta o suspeito na tabela Hash para cada pista
 * e incrementa o contador se o suspeito corresponder ao acusado.
 * * @param bstPistas A raiz (ou sub-raiz) da BST de pistas.
 * @param suspeitoAcusado O nome do suspeito a ser verificado.
 * @return O número de pistas que apontam para o suspeito.
 */
int contarPistasPorSuspeito(PistaColetada *bstPistas, const char *suspeitoAcusado) {
    if (bstPistas == NULL) {
        return 0;
    }

    int contador = 0;
    
    // Contagem na sub-árvore esquerda
    contador += contarPistasPorSuspeito(bstPistas->esquerda, suspeitoAcusado);
    
    // Contagem no nó atual
    char *suspeitoDaPista = encontrarSuspeito(bstPistas->detalhe);
    if (suspeitoDaPista != NULL && strcmp(suspeitoDaPista, suspeitoAcusado) == 0) {
        contador++;
    }

    // Contagem na sub-árvore direita
    contador += contarPistasPorSuspeito(bstPistas->direita, suspeitoAcusado);

    return contador;
}

/**
 * @brief Conduz à fase de julgamento final.
 * * Lista as pistas, solicita a acusação do jogador, e verifica se há
 * pelo menos duas pistas para sustentar a acusação.
 * * @param bstPistas A raiz da BST de pistas coletadas.
 */
void verificarSuspeitoFinal(PistaColetada *bstPistas) {
    char suspeitoAcusado[MAX_NOME];
    int numPistas;

    printf("\n==================================\n");
    printf("    **JULGAMENTO FINAL** 👨‍⚖️\n");
    printf("==================================\n");
    
    if (bstPistas == NULL) {
        printf("Você não coletou nenhuma pista. O caso é arquivado por falta de evidências.\n");
        return;
    }

    // 1. Listar Pistas Coletadas
    printf("\n📋 **PISTAS COLETADAS (Diário de Investigação):**\n");
    listarPistas(bstPistas);
    printf("\n");

    // 2. Solicitar Acusação
    printf("Quem você acusa de ser o culpado? (Ex: 'Mordomo', 'Cozinheira', 'Jardineiro', etc.)\n");
    printf("Acusado: ");
    if (scanf("%49[^\n]", suspeitoAcusado) != 1) {
        printf("Entrada inválida.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n'); // Limpar o buffer

    // 3. Verificar Pistas Suficientes
    numPistas = contarPistasPorSuspeito(bstPistas, suspeitoAcusado);

    printf("\n--- ANÁLISE DE EVIDÊNCIAS ---\n");
    printf("O sistema encontrou **%d** pista(s) que apontam para **%s**.\n", numPistas, suspeitoAcusado);

    // 4. Exibir o Desfecho
    if (numPistas >= 2) {
        printf("\n🎉 **VEREDICTO: CULPADO!**\n");
        printf("Com **%d pistas** sólidas, sua acusação contra %s é inegável! O mistério foi resolvido.\n", numPistas, suspeitoAcusado);
    } else {
        printf("\n❌ **VEREDICTO: INOCENTE!**\n");
        printf("Apenas **%d pista(s)** não é(são) suficiente(s). A lei exige pelo menos duas evidências concretas para sustentar a prisão de %s. O culpado escapou!\n", numPistas, suspeitoAcusado);
    }
    printf("\n==================================\n");
}

// ## Funções de Liberação de Memória

void liberarMansao(Comodo *raiz) {
    if (raiz != NULL) {
        liberarMansao(raiz->esquerda);
        liberarMansao(raiz->direita);
        free(raiz);
    }
}

void liberarPistas(PistaColetada *raiz) {
    if (raiz != NULL) {
        liberarPistas(raiz->esquerda);
        liberarPistas(raiz->direita);
        free(raiz);
    }
}

void liberarHash() {
    for (int i = 0; i < TAMANHO_HASH; i++) {
        HashItem *atual = tabelaHash[i];
        while (atual != NULL) {
            HashItem *proximo = atual->proximo;
            free(atual);
            atual = proximo;
        }
        tabelaHash[i] = NULL;
    }
}


// --- Função Principal ---

int main() {
    // Inicializa a Tabela Hash
    inicializarHash();

    // --- 1. Montagem da Mansão (Árvore Binária Fixa) ---
    // Estrutura:
    //                   Entrada Principal (Raiz)
    //                     /              \
    //              Sala de Estar      Cozinha
    //                /     \             /     \
    //         Biblioteca   Quarto M.   Despensa  Jardim
    
    // Nível 0 (Raiz)
    Comodo *raiz = criarSala("Entrada Principal", "Um casaco de lã de ovelha foi deixado na entrada.");

    // Nível 1
    raiz->esquerda = criarSala("Sala de Estar", "O cinzeiro está cheio de pontas de cigarro artesanais.");
    raiz->direita = criarSala("Cozinha", "A faca de chef mais cara sumiu do bloco de facas.");

    // Nível 2 - Esquerda
    raiz->esquerda->esquerda = criarSala("Biblioteca", "O livro 'Como Falsificar Identidades' está aberto na página 32.");
    raiz->esquerda->direita = criarSala("Quarto do Mordomo", "Uma nota de amor secreta para a Cozinheira.");

    // Nível 2 - Direita
    raiz->direita->esquerda = criarSala("Despensa", "Um par de luvas de borracha com terra foi jogado aqui.");
    raiz->direita->direita = criarSala("Jardim", "Um bilhete rasgado menciona 'encontre-me na biblioteca'.");


    // --- 2. Montagem da Tabela Hash (Associações Pista/Suspeito) ---
    // (As pistas devem ser as mesmas definidas nos cômodos)
    
    // Suspeitos: Mordomo, Cozinheira, Jardineiro.

    inserirNaHash("Um casaco de lã de ovelha foi deixado na entrada.", "Jardineiro");
    inserirNaHash("O cinzeiro está cheio de pontas de cigarro artesanais.", "Cozinheira");
    inserirNaHash("A faca de chef mais cara sumiu do bloco de facas.", "Cozinheira");
    inserirNaHash("O livro 'Como Falsificar Identidades' está aberto na página 32.", "Mordomo");
    inserirNaHash("Uma nota de amor secreta para a Cozinheira.", "Mordomo");
    inserirNaHash("Um par de luvas de borracha com terra foi jogado aqui.", "Jardineiro");
    inserirNaHash("Um bilhete rasgado menciona 'encontre-me na biblioteca'.", "Mordomo");
    
    
    // --- 3. Início do Jogo ---
    PistaColetada *bstPistas = NULL;

    printf("==================================================\n");
    printf("          **DETECTIVE QUEST: O CÓDIGO FINAL**\n");
    printf("==================================================\n");
    printf("Bem-vindo(a) à Mansão Enigma. Seu objetivo: Coletar Pistas e Acusar o Culpado.\n");
    printf("Você precisa de pelo menos **duas** pistas para sustentar uma acusação.\n");

    // Inicia a exploração recursiva
    explorarSalas(raiz, &bstPistas, raiz);

    // O jogador saiu (s) da exploração. Inicia o julgamento.
    if (bstPistas != NULL) {
        verificarSuspeitoFinal(bstPistas);
    } else {
        printf("\nO caso foi encerrado antes de começar, sem exploração.\n");
    }

    // --- 4. Limpeza de Memória ---
    liberarMansao(raiz);
    liberarPistas(bstPistas);
    liberarHash();
    
    printf("\nMemória liberada. Fim do Jogo.\n");

    return 0;
}
