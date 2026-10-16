# Gerenciador de Chamados em C

Sistema de linha de comando para registrar e acompanhar chamados de suporte técnico. Permite abrir, listar, encerrar e excluir chamados, com a listagem ordenada por prioridade.

Projeto desenvolvido para praticar a linguagem C no curso de Ciência de Dados e Inteligência Artificial da UEL.

Os chamados ficam apenas em memória: nada é salvo em arquivo, e os dados são perdidos quando o programa é encerrado.

## Funcionalidades

- **Abrir chamado:** registra uma descrição e uma prioridade (Baixa, Média ou Alta). Cada chamado recebe um ID sequencial.
- **Listar chamados:** exibe os chamados da prioridade mais alta para a mais baixa. Chamados com a mesma prioridade mantêm a ordem de cadastro.
- **Encerrar chamado:** marca um chamado aberto como encerrado, a partir do ID.
- **Excluir chamado:** remove um chamado, aberto ou encerrado, a partir do ID. IDs de chamados excluídos não são reutilizados.
- **Validação de entradas:** opções do menu, prioridades e IDs só são aceitos quando são números inteiros válidos. Entradas como `abc`, `12abc` ou uma linha vazia são recusadas com uma mensagem de erro, e o programa encerra normalmente se a entrada terminar (EOF).

## Conceitos de C aplicados

- **`struct`:** cada chamado é representado pela struct `Chamado`, com ID, descrição, prioridade e status.
- **Ponteiros:** as funções recebem ponteiros para alterar os contadores do `main`, como a quantidade de chamados e o próximo ID.
- **Ponteiro para ponteiro:** `abrirChamado` recebe `Chamado **` porque o `realloc` pode mudar o endereço do vetor de chamados.
- **Alocação dinâmica:** o vetor de chamados cresce conforme novos chamados são abertos.
  - `malloc` cria o vetor auxiliar de índices usado na listagem.
  - `realloc` aumenta a capacidade do vetor de chamados. Se a alocação falhar, os chamados já cadastrados são preservados.
  - `free` libera a memória ao final da listagem e ao sair do programa.
- **Ordenação estável:** a listagem usa insertion sort sobre um vetor de índices, sem reordenar os dados originais e mantendo a ordem de cadastro entre chamados de mesma prioridade.
- **Modularização:** o código é dividido em `.c` e `.h`. As funções auxiliares são `static` e ficam visíveis apenas dentro de `chamado.c`.

## Estrutura do projeto

```text
.
├── main.c       # Menu principal e controle do fluxo do programa
├── chamado.c    # Operações sobre os chamados e leitura das entradas
├── chamado.h    # Struct Chamado, constantes e funções compartilhadas
├── .gitignore   # Ignora executáveis e configurações locais do VS Code
└── README.md    # Documentação do projeto
```

## Compilação e execução

A compilação foi verificada no Windows com o GCC 14.2, instalado via MSYS2.

No terminal **MSYS2 UCRT64**, dentro da pasta do projeto:

```bash
gcc -Wall -Wextra -o chamados main.c chamado.c
./chamados
```

No **PowerShell**, com o GCC do MSYS2 no `PATH`:

```powershell
gcc -Wall -Wextra -o chamados main.c chamado.c
.\chamados.exe
```

## Exemplo de uso

A execução abaixo abre dois chamados (com uma prioridade inválida recusada), lista os chamados por prioridade, encerra o chamado 2, exclui o chamado 1 e sai do programa.

```text
===== SISTEMA DE CHAMADOS =====
1 - Abrir chamado
2 - Atender/Encerrar chamado
3 - Listar chamados
4 - Excluir chamado
0 - Sair
Escolha uma opcao: 1
Descricao do chamado: Impressora nao imprime
Prioridade (1 - Baixa, 2 - Media, 3 - Alta): 2
Chamado 1 aberto com sucesso!

===== SISTEMA DE CHAMADOS =====
1 - Abrir chamado
2 - Atender/Encerrar chamado
3 - Listar chamados
4 - Excluir chamado
0 - Sair
Escolha uma opcao: 1
Descricao do chamado: Sistema de vendas parado
Prioridade (1 - Baixa, 2 - Media, 3 - Alta): alta
Prioridade invalida! Digite 1, 2 ou 3.
Prioridade (1 - Baixa, 2 - Media, 3 - Alta): 3
Chamado 2 aberto com sucesso!

===== SISTEMA DE CHAMADOS =====
1 - Abrir chamado
2 - Atender/Encerrar chamado
3 - Listar chamados
4 - Excluir chamado
0 - Sair
Escolha uma opcao: 3

===== LISTA DE CHAMADOS =====
ID: 2
Descricao: Sistema de vendas parado
Prioridade: Alta
Status: Aberto
------------------------------
ID: 1
Descricao: Impressora nao imprime
Prioridade: Media
Status: Aberto
------------------------------
Total de chamados: 2

===== SISTEMA DE CHAMADOS =====
1 - Abrir chamado
2 - Atender/Encerrar chamado
3 - Listar chamados
4 - Excluir chamado
0 - Sair
Escolha uma opcao: 2
Informe o ID do chamado: 2
Chamado 2 encerrado com sucesso!

===== SISTEMA DE CHAMADOS =====
1 - Abrir chamado
2 - Atender/Encerrar chamado
3 - Listar chamados
4 - Excluir chamado
0 - Sair
Escolha uma opcao: 4
Informe o ID do chamado: 1
Chamado 1 excluido com sucesso!

===== SISTEMA DE CHAMADOS =====
1 - Abrir chamado
2 - Atender/Encerrar chamado
3 - Listar chamados
4 - Excluir chamado
0 - Sair
Escolha uma opcao: 0
Saindo do sistema...
```
