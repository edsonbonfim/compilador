#include <stdio.h>
#include <stdlib.h>

#include "tools.h"
#include "lex.h"
#include "parser.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("COMP\n");
        printf("Uso: COMP [programa_fonte]\n");
        exit(EXIT_FAILURE);
    }

    char *codigo_fonte = conteudo_arquivo(argv[1]);

    // Inicializa o analisador lexico
    Lex *lex = iniciar_lex(codigo_fonte);

    Tree *tree = parse_tree(lex);

    print_parse_tree(tree);

    tree = dispose_parse_tree(tree);

    return EXIT_SUCCESS;
}
