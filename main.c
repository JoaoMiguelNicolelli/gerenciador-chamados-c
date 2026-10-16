#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

#define TAM_DESCRICAO 200
#define CAPACIDADE_INICIAL 2
#define TAM_LINHA_NUMERO 32

#define LEITURA_EOF -1
#define LEITURA_INVALIDA 0
#define LEITURA_OK 1

#define STATUS_ABERTO 0
#define STATUS_ENCERRADO 1

#define PRIORIDADE_BAIXA 1
#define PRIORIDADE_MEDIA 2
#define PRIORIDADE_ALTA 3

typedef struct {
    int id;
    char descricao[TAM_DESCRICAO];
    int prioridade;
    int status;
} Chamado;

// Descarta o resto da linha que ficou no buffer de entrada.
// Devolve 0 se a entrada terminou (EOF), senao 1.
// Comparar apenas com '\n' daria laco infinito, porque no EOF o getchar devolve EOF para sempre
int limparEntrada(void) {
    int c;

    while ((c = getchar()) != '\n') {
        if (c == EOF) {
            return 0;
        }
    }

    return 1;
}

// Le uma linha inteira e converte para int. Aceita espacos antes e depois do numero.
// Devolve LEITURA_OK, LEITURA_INVALIDA ou LEITURA_EOF
// Como sempre consome a linha toda, nenhuma sobra fica no buffer para a proxima leitura
int lerInteiro(int *valor) {
    char linha[TAM_LINHA_NUMERO];
    char *fim;
    long numero;
    int c;
    int terminouEntrada = 0;
    int semDigitos;

    if (fgets(linha, TAM_LINHA_NUMERO, stdin) == NULL) {
        return LEITURA_EOF;
    }

    // Sem '\n' no buffer: a linha acabou no EOF, coube exatamente ou e longa demais
    if (strchr(linha, '\n') == NULL) {
        c = getchar();
        if (c == EOF) {
            terminouEntrada = 1;
        } else if (c != '\n') {
            // Linha longa demais: descarta o excedente para ele nao virar uma nova resposta
            if (!limparEntrada()) {
                return LEITURA_EOF;
            }
            return LEITURA_INVALIDA;
        }
    }

    errno = 0;
    numero = strtol(linha, &fim, 10);

    // fim == linha: nenhum digito (linha vazia, so espacos, "abc").
    // Precisa ser guardado antes do laco abaixo, que move fim
    semDigitos = (fim == linha);

    // Pula os espacos depois do numero (o '\n' tambem conta como espaco)
    while (isspace((unsigned char)*fim)) {
        fim++;
    }

    // *fim != '\0': sobrou texto, como em "12abc"
    // A comparacao com INT_MAX/INT_MIN e necessaria onde long tem 64 bits
    if (semDigitos || *fim != '\0' || errno == ERANGE || numero > INT_MAX || numero < INT_MIN) {
        // Se a entrada ja acabou, nao adianta pedir de novo
        if (terminouEntrada) {
            return LEITURA_EOF;
        }
        return LEITURA_INVALIDA;
    }

    *valor = (int)numero;
    return LEITURA_OK;
}

int aumentarCapacidade(Chamado **chamados, int *capacidade) {
    int novaCapacidade;
    Chamado *temp;

    if (*capacidade == 0) {
        novaCapacidade = CAPACIDADE_INICIAL;
    } else {
        novaCapacidade = *capacidade * 2;
    }

    // realloc com ponteiro NULL funciona como malloc, entao serve tanto para a primeira alocacao quanto para as proximas
    // O resultado vai para temp: se falhar, chamados continua apontando para o vetor original
    temp = realloc(*chamados, novaCapacidade * sizeof(Chamado));
    if (temp == NULL) {
        return 0;
    }

    *chamados = temp;
    *capacidade = novaCapacidade;
    return 1;
}

// Recebe Chamado ** porque aumentarCapacidade pode mover o vetor de endereco
// proximoId vem por ponteiro para avancar sempre, mesmo depois de exclusoes: assim nenhum ID e reaproveitado
// Devolve 0 se a entrada terminou (EOF) e 1 nos demais casos, inclusive falta de memoria
int abrirChamado(Chamado **chamados, int *quantidadeChamados, int *capacidade, int *proximoId) {
    Chamado *novo;
    int tamanho;
    int resultado;

    if (*quantidadeChamados == *capacidade) {
        if (!aumentarCapacidade(chamados, capacidade)) {
            printf("Erro: memoria insuficiente. Chamado nao registrado.\n");
            return 1;
        }
    }

    // Aponta para a posicao livre depois do realloc, pois o vetor pode ter mudado de endereco
    novo = &(*chamados)[*quantidadeChamados];

    // Nao e preciso limpar o buffer aqui: lerInteiro ja consumiu a linha inteira da opcao
    do {
        printf("Descricao do chamado: ");

        // No EOF o fgets devolve NULL e nao escreve nada, entao a descricao nao pode ser lida
        if (fgets(novo->descricao, TAM_DESCRICAO, stdin) == NULL) {
            printf("Entrada encerrada. Chamado nao registrado.\n");
            return 0;
        }

        tamanho = strcspn(novo->descricao, "\n");
        if (novo->descricao[tamanho] == '\n') {
            novo->descricao[tamanho] = '\0';
        } else {
            // Texto maior que o limite: o restante da linha e descartado
            if (!limparEntrada()) {
                printf("Entrada encerrada. Chamado nao registrado.\n");
                return 0;
            }
        }

        if (tamanho == 0) {
            printf("A descricao nao pode ficar vazia.\n");
        }
    } while (tamanho == 0);

    do {
        printf("Prioridade (1 - Baixa, 2 - Media, 3 - Alta): ");
        resultado = lerInteiro(&novo->prioridade);
        if (resultado == LEITURA_EOF) {
            printf("Entrada encerrada. Chamado nao registrado.\n");
            return 0;
        }
        if (resultado == LEITURA_INVALIDA) {
            novo->prioridade = 0;
        }

        if (novo->prioridade < PRIORIDADE_BAIXA || novo->prioridade > PRIORIDADE_ALTA) {
            printf("Prioridade invalida! Digite 1, 2 ou 3.\n");
        }
    } while (novo->prioridade < PRIORIDADE_BAIXA || novo->prioridade > PRIORIDADE_ALTA);

    // O ID vem do contador, nao da quantidade: assim uma exclusao nao faz o proximo chamado repetir um ID
    novo->id = *proximoId;
    novo->status = STATUS_ABERTO;
    (*quantidadeChamados)++;
    (*proximoId)++;

    printf("Chamado %d aberto com sucesso!\n", novo->id);
    return 1;
}

// Chamado * basta: altera o status de um chamado, mas nao realoca o vetor nem mexe nos contadores
// Devolve 0 se a entrada terminou (EOF) e 1 nos demais casos, igual a excluirChamado
int encerrarChamado(Chamado *chamados, int quantidadeChamados) {
    int idBusca;
    int encontrado;
    int i;
    int resultado;

    printf("Informe o ID do chamado: ");
    resultado = lerInteiro(&idBusca);
    if (resultado == LEITURA_EOF) {
        printf("Entrada encerrada. Operacao cancelada.\n");
        return 0;
    }
    if (resultado == LEITURA_INVALIDA) {
        printf("ID invalido! Digite apenas numeros.\n");
        return 1;
    }

    encontrado = -1;
    for (i = 0; i < quantidadeChamados; i++) {
        if (chamados[i].id == idBusca) {
            encontrado = i;
            break;
        }
    }

    if (encontrado == -1) {
        printf("Chamado nao encontrado.\n");
    } else if (chamados[encontrado].status != STATUS_ABERTO) {
        printf("O chamado %d ja esta encerrado.\n", chamados[encontrado].id);
    } else {
        chamados[encontrado].status = STATUS_ENCERRADO;
        printf("Chamado %d encerrado com sucesso!\n", chamados[encontrado].id);
    }

    return 1;
}

// Chamado * basta: a exclusao desloca elementos dentro do vetor, mas nao realoca nem libera nada
// quantidadeChamados vem por ponteiro porque a exclusao diminui a contagem
// Devolve 0 se a entrada terminou (EOF) e 1 nos demais casos, igual a limparEntrada e aumentarCapacidade
int excluirChamado(Chamado *chamados, int *quantidadeChamados) {
    int idBusca;
    int encontrado;
    int i;
    int resultado;

    printf("Informe o ID do chamado: ");
    resultado = lerInteiro(&idBusca);
    // Nao ha mais entrada para ler: avisa o main para encerrar em vez de voltar ao menu
    if (resultado == LEITURA_EOF) {
        printf("Entrada encerrada. Operacao cancelada.\n");
        return 0;
    }
    if (resultado == LEITURA_INVALIDA) {
        printf("ID invalido! Digite apenas numeros.\n");
        return 1;
    }

    encontrado = -1;
    for (i = 0; i < *quantidadeChamados; i++) {
        if (chamados[i].id == idBusca) {
            encontrado = i;
            break;
        }
    }

    if (encontrado == -1) {
        printf("Chamado nao encontrado.\n");
        return 1;
    }

    // Desloca os posteriores uma posicao para a esquerda, cobrindo o excluido.
    // A atribuicao de struct copia o registro inteiro, inclusive a descricao
    // Ao excluir o ultimo o laco nao executa: basta diminuir a contagem
    for (i = encontrado; i < *quantidadeChamados - 1; i++) {
        chamados[i] = chamados[i + 1];
    }

    (*quantidadeChamados)--;

    // Usa idBusca porque a posicao do excluido ja foi sobrescrita pelo deslocamento
    printf("Chamado %d excluido com sucesso!\n", idBusca);
    return 1;
}

// Preenche ordem[] com os indices dos chamados, da maior prioridade para a menor.
// O vetor de chamados nao e alterado: so o vetor de indices e reorganizado
// Insertion sort: no empate a condicao do while e falsa, o elemento novo fica depois
// dos iguais e a ordem de cadastro se mantem (ordenacao estavel)
void ordenarIndicesPorPrioridade(Chamado *chamados, int quantidadeChamados, int *ordem) {
    int indiceAtual;
    int i;
    int j;

    for (i = 0; i < quantidadeChamados; i++) {
        ordem[i] = i;
    }

    for (i = 1; i < quantidadeChamados; i++) {
        indiceAtual = ordem[i];
        j = i - 1;

        // '<' e nao '<=', senao a ordenacao deixaria de ser estavel
        while (j >= 0 && chamados[ordem[j]].prioridade < chamados[indiceAtual].prioridade) {
            ordem[j + 1] = ordem[j];
            j--;
        }

        ordem[j + 1] = indiceAtual;
    }
}

void listarChamados(Chamado *chamados, int quantidadeChamados) {
    Chamado *atual;
    int *ordem;
    int i;

    // Vetor auxiliar apenas com as posicoes: os chamados nao sao copiados nem reordenados
    ordem = malloc(quantidadeChamados * sizeof(int));
    if (ordem == NULL) {
        printf("Erro: memoria insuficiente para ordenar a lista.\n");
        return;
    }

    ordenarIndicesPorPrioridade(chamados, quantidadeChamados, ordem);

    printf("\n===== LISTA DE CHAMADOS =====\n");
    for (i = 0; i < quantidadeChamados; i++) {
        // Segue a ordem de prioridade, nao a ordem de cadastro
        atual = &chamados[ordem[i]];

        printf("ID: %d\n", atual->id);
        printf("Descricao: %s\n", atual->descricao);

        if (atual->prioridade == PRIORIDADE_BAIXA) {
            printf("Prioridade: Baixa\n");
        } else if (atual->prioridade == PRIORIDADE_MEDIA) {
            printf("Prioridade: Media\n");
        } else {
            printf("Prioridade: Alta\n");
        }

        if (atual->status == STATUS_ABERTO) {
            printf("Status: Aberto\n");
        } else {
            printf("Status: Encerrado\n");
        }

        printf("------------------------------\n");
    }

    free(ordem);
    printf("Total de chamados: %d\n", quantidadeChamados);
}

void exibirMenu(void) {
    printf("\n===== SISTEMA DE CHAMADOS =====\n");
    printf("1 - Abrir chamado\n");
    printf("2 - Atender/Encerrar chamado\n");
    printf("3 - Listar chamados\n");
    printf("4 - Excluir chamado\n");
    printf("0 - Sair\n");
    printf("Escolha uma opcao: ");
}

int main() {
    Chamado *chamados = NULL;
    int quantidadeChamados = 0;
    int capacidade = 0;
    int proximoId = 1;
    int opcao;
    int resultado;

    do {
        exibirMenu();

        resultado = lerInteiro(&opcao);
        if (resultado == LEITURA_EOF) {
            // A entrada terminou (EOF): encerra em vez de ficar preso no menu
            printf("\nEntrada encerrada.\n");
            opcao = 0;
        } else if (resultado == LEITURA_INVALIDA) {
            opcao = -1;
        }

        // Nas operacoes, um retorno 0 indica EOF: opcao = 0 encerra o laco sem exibir o menu
        // de novo e sem pular o free do final
        switch (opcao) {
            case 1:
                if (!abrirChamado(&chamados, &quantidadeChamados, &capacidade, &proximoId)) {
                    printf("Saindo do sistema...\n");
                    opcao = 0;
                }
                break;
            case 2:
                if (quantidadeChamados == 0) {
                    printf("Nao existem chamados para atender.\n");
                } else if (!encerrarChamado(chamados, quantidadeChamados)) {
                    printf("Saindo do sistema...\n");
                    opcao = 0;
                }
                break;
            case 3:
                if (quantidadeChamados == 0) {
                    printf("Nenhum chamado cadastrado.\n");
                } else {
                    listarChamados(chamados, quantidadeChamados);
                }
                break;
            case 4:
                if (quantidadeChamados == 0) {
                    printf("Nao existem chamados para excluir.\n");
                } else if (!excluirChamado(chamados, &quantidadeChamados)) {
                    printf("Saindo do sistema...\n");
                    opcao = 0;
                }
                break;
            case 0:
                printf("Saindo do sistema...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 0);

    // Libera o vetor alocado; se nada foi alocado, free(NULL) nao faz nada
    free(chamados);

    return 0;
}
