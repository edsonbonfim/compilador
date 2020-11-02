#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tools.h"

/**
 * Elimina os espacos em branco que estiverem antes do primeito caracter
 * e depois do ultimo caracter de uma string
 */
char *trim(char *str) {
    char *res;

    while (isspace((unsigned char) *str)) str++;

    res = str + strlen(str) - 1;

    while (res > str && isspace((unsigned char) *res)) res--;

    res[1] = '\0';

    return str;
}

/**
 * Retorna o tamanho do arquivo
 */
int tamanho_arquivo(char *nome) {
    FILE *arq = fopen(nome, "r");

    if (!arq) {
        fprintf(stderr, "Nao foi possivel abrir o arquivo '%s'", nome);
        exit(EXIT_FAILURE);
    }

    fseek(arq, 0, SEEK_END);
    int tam = (int) ftell(arq);

    fclose(arq);

    return tam;
}

/**
 * Retorna uma string com o conteudo do arquivo
 */
char *conteudo_arquivo(char *nome) {

    nome = trim(nome);

    FILE *arq = fopen(nome, "r");

    if (!arq) {
        fprintf(stderr, "Nao foi possivel abrir o arquivo '%s'", nome);
        exit(EXIT_FAILURE);
    }

    int i, tam = tamanho_arquivo(nome);

    char *conteudo = malloc((tam + 1) * sizeof(char));

    for (i = 0; i < tam; i++)
        conteudo[i] = (char) fgetc(arq);

    conteudo[i] = '\0';

    return conteudo;
}
