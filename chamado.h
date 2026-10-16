#ifndef CHAMADO_H
#define CHAMADO_H

#define TAM_DESCRICAO 200

#define LEITURA_EOF -1
#define LEITURA_INVALIDA 0
#define LEITURA_OK 1

typedef struct {
    int id;
    char descricao[TAM_DESCRICAO];
    int prioridade;
    int status;
} Chamado;

int lerInteiro(int *valor);
int abrirChamado(Chamado **chamados, int *quantidadeChamados, int *capacidade, int *proximoId);
int encerrarChamado(Chamado *chamados, int quantidadeChamados);
int excluirChamado(Chamado *chamados, int *quantidadeChamados);
void listarChamados(Chamado *chamados, int quantidadeChamados);

#endif
