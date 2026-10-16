#include <stdio.h>
#include <stdlib.h>
#include "chamado.h"

static void exibirMenu(void) {
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
