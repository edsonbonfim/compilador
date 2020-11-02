#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "lex.h"

Lex *lex;

bool _check_token(int n, Token token, va_list args) {
    for (int i = 0; i < n; i++) {
        if (token.tipo == va_arg(args, int)) {
            va_end(args);
            return true;
        }
    }

    va_end(args);

    return false;
}

bool check_peek_token(int n, ...) {
    va_list args;
    va_start(args, n);
    return _check_token(n, peek(), args);
}

bool check_peek2_token(int n, ...) {
    va_list args;
    va_start(args, n);
    return _check_token(n, peek2(), args);
}

//bool check_token(int n, ...) {
//    va_list args;
//    va_start(args, n);
//    return _check_token(n, lex->current_token, args);
//}

Tree *new_node() {
    Tree *node = (Tree *) malloc(sizeof(Tree));
    node->token.valor = lex->current_token.valor;
    node->token.tipo = lex->current_token.tipo;
    node->child = NULL;
    node->brother = NULL;
    return node;
}

void *syntax_error() {
    printf("Syntax error: %s\n", (char *) lex->current_token.valor);
    exit(EXIT_FAILURE);
}

/**
 * [id] -> (id) Seqüência alfanumérica iniciada por char (tratado no lexico)
 */
Tree *id() {
    if (!check_peek_token(1, ID)) {
        return syntax_error();
    }

    next_token();
    Tree *node = new_node();  // [id]
    node->child = new_node(); // (id)

    return node;
}

/**
 * [number] -> (number) Seqüência numérica com a possibilidade da ocorrência de no máximo um ponto
 */
Tree *number() {
    if (!check_peek_token(2, INTEIRO, REAL)) {
        return syntax_error();
    }

    next_token();
    Tree *node = new_node();  // [number]
    node->child = new_node(); // (number)

    return node;
}

Tree *param();

/**
 * [name] -> [id]
 *        -> [id] (.) [name]
 *        -> [id] ([) [param] (])
 */
Tree *name() {
    if (!check_peek_token(1, ID)) {
        return syntax_error();
    }

    Tree *node = new_node(); // [name]
    node->child = id();      // [id]

    if (check_peek_token(1, PONTO)) {
        next_token();
        node->child->brother = new_node();      // (.)
        node->child->brother->brother = name(); // [name]
        return node;
    }

    if (check_peek_token(1, L_BRACKET)) {
        next_token();
        node->child->brother = new_node();       // ([)
        node->child->brother->brother = param(); // [param]

        if (!check_peek_token(1, R_BRACKET)) {
            return syntax_error();
        }

        next_token();
        node->child->brother->brother->brother = new_node(); // (])
        return node;
    }

    return node;
}

/**
 * [op_math] -> (+)
 *           -> (-)
 *           -> (*)
 *           -> (/)
 */
Tree *op_math() {
    if (check_peek_token(4, PLUS, MINUS, TIMES, SLASH)) {
        next_token();
        Tree *node = new_node();  // [op_math]
        node->child = new_node(); // (+|-|*|/)
        return node;
    }

    return syntax_error();
}

/**
 * [op_logic] -> (>)
 *            -> (<)
 *            -> (=)
 *            -> (!)
 */
Tree *op_logic() {
    if (check_peek_token(4, BIGGER_THAN, LESS_THAN, EQUAL, EXCLAMATION)) {
        next_token();
        Tree *node = new_node();  // [op_logic]
        node->child = new_node(); // (>|<|=|!)
        return node;
    }

    return syntax_error();
}

/**
 * [param] -> [name]
 *         -> [number]
 */
Tree *param() {
    Tree *node = new_node(); // [param]

    if (check_peek_token(1, ID)) {
        node->child = name(); // [name]
        return node;
    }

    if (check_peek_token(2, INTEIRO, REAL)) {
        node->child = number(); // [number]
        return node;
    }

    return syntax_error();
}

/**
 * [expr_math] -> [param] [op_math] [expr_math]
 *             -> [param]
 */
Tree *expr_math() {
    Tree *node = new_node(); // [expr_math]
    node->child = param();   // [param]

    if (check_peek_token(4, PLUS, MINUS, TIMES, SLASH)) {
        node->child->brother = op_math(); // [op_math]
        node->child->brother->brother = expr_math(); // [expr_math]
    }

    return node;
}

/**
 * [expr_logic] -> [expr_math] [op_logic] [expr_logic]
 *              -> [expr_math]
 */
Tree *expr_logic() {
    Tree *node = new_node();   // [expr_logic]
    node->child = expr_math(); // [expr_math]

    if (check_peek_token(4, BIGGER_THAN, LESS_THAN, EQUAL, EXCLAMATION)) {
        node->child->brother = op_logic();
        node->child->brother->brother = expr_logic();
    }

    return node;
}

/**
 * [list_name] -> [param] (,) [list_name]
 *             -> [param]
 *             -> Є
 */
Tree *list_name() {
    Tree *node = new_node(); // [list_name]

    if (!check_peek_token(1, ID)) {
        node->child = new_node(); // Є
        node->child->token.valor = "";
        return node;
    }

    node->child = param();   // [param]

    if (check_peek_token(1, VIRGULA)) {
        next_token();
        node->child->brother = new_node(); // (,)
        node->child->brother->brother = list_name(); // [list_name]
    }

    return node;
}

/**
 * [list_param] -> (() [list_name] ())
 */
Tree *list_param() {
    if (!check_peek_token(1, L_PAREN)) {
        return syntax_error();
    }

    next_token();
    Tree *node = new_node();  // [list_param]
    node->child = new_node(); // (()
    node->child->brother = list_name(); // [list_name]

    if (!check_peek_token(1, R_PAREN)) {
        return syntax_error();
    }

    next_token();
    node->child->brother->brother = new_node(); // ())
    return node;
}

/**
 * [value] -> [expr_math]
 *         -> [id] [list_param]
 */
Tree *value() {
    Tree *node = new_node(); // [value]

    if (check_peek2_token(1, L_PAREN)) {
        node->child = id();
        node->child->brother = list_param();
        return node;
    }

    node->child = expr_math();
    return node;
}

Tree *bloc();

/**
 * [_else] -> (else) [bloc]
 *         -> Є
 */
Tree *_else() {
    Tree *node = new_node(); // [_else]

    if (!check_peek_token(1, ELSE)) {
        node->child = new_node();
        node->child->token.valor = ""; // Є
        return node;
    }

    next_token();
    node->child = new_node();      // (else)
    node->child->brother = bloc(); // [bloc]

    return node;
}

Tree *const_value();

/**
 * [command] -> [name] (:=) [value]
 *           -> (while) [expr_logic] [bloc]
 *           -> (if) [expr_logic] (then) [bloc] [else]
 *           -> (write) [const_value]
 *           -> (read) [name]
 */
Tree *command() {
    Tree *node = new_node(); // [command]

    if (check_peek_token(1, WHILE)) {
        next_token();
        node->child = new_node(); // (while)
        node->child->brother = expr_logic(); // [expr_logic]
        node->child->brother->brother = bloc(); // [bloc]
        return node;
    }

    if (check_peek_token(1, IF)) {
        next_token();
        node->child = new_node(); // (if)
        node->child->brother = expr_logic(); // [expr_logic]

        if (!check_peek_token(1, THEN)) {
            return syntax_error();
        }

        next_token();
        node->child->brother->brother = new_node(); // (then)
        node->child->brother->brother->brother = bloc(); // [bloc]
        node->child->brother->brother->brother->brother = _else(); // [_else]
        return node;
    }

    if (check_peek_token(1, WRITE)) {
        next_token();
        node->child = new_node(); // (write)
        node->child->brother = const_value(); // [const_value]
        return node;
    }

    if (check_peek_token(1, READ)) {
        next_token();
        node->child = new_node(); // (read)
        node->child->brother = name(); // [name]
        return node;
    }

    node->child = name(); // [name]

    if (!check_peek_token(1, ATTR)) {
        return syntax_error();
    }

    next_token();
    node->child->brother = new_node(); // (:=)
    node->child->brother->brother = value(); // [value]
    return node;
}

/**
 * [list_command] -> [command] (;) [list_command]
 *                -> Є
 */
Tree *list_command() {
    Tree *node = new_node(); // [list_command]

    if (!check_peek_token(5, WHILE, IF, WRITE, READ, ID)) {
        node->child = new_node(); // Є
        node->child->token.valor = "";
        return node;
    }

    node->child = command(); // [command]

    if (!check_peek_token(1, PT_VIRGULA)) {
        return syntax_error();
    }

    next_token();
    node->child->brother = new_node(); // (;)
    node->child->brother->brother = list_command(); // [list_command]
    return node;
}

/**
 * [bloc] -> (begin) [list_command] (end)
 *        -> [command]
 */
Tree *bloc() {
    Tree *node = new_node(); // [bloc]

    if (check_peek_token(1, INICIO)) {
        next_token();
        node->child = new_node(); // (begin)
        node->child->brother = list_command(); // [list_command]

        if (!check_peek_token(1, FIM)) {
            return syntax_error();
        }

        next_token();
        node->child->brother->brother = new_node(); // (end);
        return node;
    }

    node->child = command(); // [command]

    return node;
}

Tree *def_variable();

/**
 * [bloc_function] -> [def_variable] [bloc]
 *                 -> [bloc]
 */
Tree *bloc_function() {
    Tree *node = new_node(); // [bloc_function]

    if (check_peek_token(1, VAR)) {
        node->child = def_variable();  // [def_variable]
        node->child->brother = bloc(); // [bloc]
        return node;
    }

    node->child = bloc();
    return node;
}

Tree *list_variable();

Tree *data_type();

/**
 * [name_function] -> (function) [id] (() [list_variable] ()) (:) [data_type]
 */
Tree *name_function() {
    if (!check_peek_token(1, FUNCTION)) {
        return syntax_error();
    }

    next_token();
    Tree *node = new_node(); // [name_function]
    node->child = new_node(); // (function)
    node->child->brother = id(); // [id]

    if (!check_peek_token(1, L_PAREN)) {
        return syntax_error();
    }

    next_token();
    node->child->brother->brother = new_node(); // (()
    node->child->brother->brother->brother = list_variable(); // [list_variable]

    if (!check_peek_token(1, R_PAREN)) {
        return syntax_error();
    }

    next_token();
    node->child->brother->brother->brother->brother = new_node(); // ())

    if (!check_peek_token(1, DOIS_PONTOS)) {
        return syntax_error();
    }

    next_token();
    node->child->brother->brother->brother->brother->brother = new_node(); // (:)
    node->child->brother->brother->brother->brother->brother->brother = data_type(); // [data_type]

    return node;
}

/**
 * [function] -> [name_function] [bloc_function]
 */
Tree *function() {
    Tree *node = new_node(); // [function]
    node->child = name_function(); // [name_function]
    node->child->brother = bloc_function(); // [bloc_function]
    return node;
}

/**
 * [list_function] -> [function] [list_function]
 *                 -> Є
 */
Tree *list_function() {
    Tree *node = new_node(); // [list_function]

    if (!check_peek_token(1, FUNCTION)) {
        node->child = new_node(); // Є
        node->child->token.valor = "";
        return node;
    }

    node->child = function();
    node->child->brother = list_function();

    return node;
}

/**
 * [data_type] -> (integer)
 *             -> (real)
 *             -> (array) ([) [number] (]) (of) [data_type]
 *             -> (record) [list_variable] (end)
 *             -> [id]
 */
Tree *data_type() {
    Tree *node = new_node(); // [data_type]

    if (check_peek_token(2, TIPO_DADO)) {
        next_token();
        node->child = new_node(); // (integer|real)
        return node;
    }

    if (check_peek_token(1, VETOR)) {
        next_token();
        node->child = new_node(); // (array)

        if (!check_peek_token(1, L_BRACKET)) {
            return syntax_error();
        }

        next_token();
        node->child->brother = new_node(); // ([)
        node->child->brother = number(); // [number]

        if (!check_peek_token(1, R_BRACKET)) {
            return syntax_error();
        }

        next_token();
        node->child->brother = new_node(); // (])

        if (!check_peek_token(1, OF)) {
            return syntax_error();
        }

        next_token();
        node->child->brother = new_node(); // (of)
        node->child->brother->brother = data_type(); // [data_type]

        return node;
    }

    if (check_peek_token(1, RECORD)) {
        next_token();
        node->child = new_node(); // (record)
        node->child->brother = list_variable(); // [list_variable]

        if (!check_peek_token(1, FIM)) {
            return syntax_error();
        }

        next_token();
        node->child->brother->brother = new_node(); // (end)

        return node;
    }

    node->child = id();

    return node;
}

/**
 * [list_id] -> [id] (,) [list_id]
 *           -> [id]
 */
Tree *list_id() {
    Tree *node = new_node(); // [list_id]
    node->child = id(); // [id]

    if (!check_peek_token(1, VIRGULA)) {
        return node;
    }

    next_token();
    node->child->brother = new_node(); // (,)
    node->child->brother->brother = list_id(); // [list_id]

    return node;
}

/**
 * [variable] -> [list_id] (:) [data_type]
 */
Tree *variable() {
    Tree *node = new_node(); // [variable]
    node->child = list_id();

    if (!check_peek_token(1, DOIS_PONTOS)) {
        return syntax_error();
    }

    next_token();
    node->child->brother = new_node(); // (:)
    node->child->brother->brother = data_type(); // [data_type]
    return node;
}

/**
 * [list_variable] -> [variable] (;) [list_variable]
 *                 -> [variable]
 *                 -> Є !important
 */
Tree *list_variable() {
    Tree *node = new_node(); // [list_variable]

    if (!check_peek_token(1, ID)) {
        node->child = new_node(); // Є
        node->child->token.valor = "";
        return node;
    }

    node->child = variable();

    if (!check_peek_token(1, PT_VIRGULA)) {
        return node;
    }

    next_token();
    node->child->brother = new_node(); // (;)
    node->child->brother->brother = list_variable(); // [list_variable]

    return node;
}

/**
 * [type] -> [id] (=) [data_type]
 */
Tree *type() {
    Tree *node = new_node(); // [type]
    node->child = id(); // [id]

    if (!check_peek_token(1, EQUAL)) {
        return syntax_error();
    }

    next_token();
    node->child->brother = new_node(); // (=)
    node->child->brother->brother = data_type(); // [data_type]

    return node;
}

/**
 * [list_type] -> [type] (;) [list_type]
 *             -> [type]
 *             -> Є !important
 */
Tree *list_type() {
    Tree *node = new_node(); // [list_type]

    if (!check_peek_token(1, ID)) {
        node->child = new_node(); // Є
        node->child->token.valor = "";
        return node;
    }

    node->child = type();

    if (!check_peek_token(1, PT_VIRGULA)) {
        return node;
    }

    next_token();
    node->child->brother = new_node(); // (;)
    node->child->brother->brother = list_type(); // [list_type]

    return node;
}

/**
 * [const_value] -> (") Sequência alfanumérica (")
 *               -> [expr_math]
 */
Tree *const_value() {
    Tree *node = new_node(); // [const_value]

    if (check_peek_token(1, STRING)) {
        next_token();
        node->child = new_node(); // string
        return node;
    }

    node->child = expr_math(); // [expr_math]
    return node;
}

/**
 * [constant] -> [id] (=) [const_value] (;)
 */
Tree *constant() {
    Tree *node = new_node(); // [constant]
    node->child = id(); // [id]

    if (!check_peek_token(1, EQUAL)) {
        return syntax_error();
    }

    next_token();
    node->child->brother = new_node(); // (=)
    node->child->brother->brother = const_value(); // [const_value]

    if (!check_peek_token(1, PT_VIRGULA)) {
        return syntax_error();
    }

    next_token();
    node->child->brother->brother->brother = new_node(); // (;)

    return node;
}

/**
 * [list_const] -> [constant] [list_const]
 *              -> [constant]
 */
Tree *list_const() {
    Tree *node = new_node(); // [list_const]
    node->child = constant(); // [constant]
    return node;
}

/**
 * [def_variable] -> (var) [list_variable]
 *                -> Є
 */
Tree *def_variable() {
    Tree *node = new_node(); // [def_variable]

    if (!check_peek_token(1, VAR)) {
        node->child = new_node(); // Є
        node->child->token.valor = "";
        return node;
    }

    next_token();
    node->child = new_node(); // (var)
    node->child->brother = list_variable();

    return node;
}

/**
 * [def_type] -> (type) [list_type]
 *            -> Є
 */
Tree *def_type() {
    Tree *node = new_node(); // [def_type]

    if (!check_peek_token(1, TIPO)) {
        node->child = new_node(); // Є
        node->child->token.valor = "";
        return node;
    }

    next_token();
    node->child = new_node(); // (type)
    node->child->brother = list_type();

    return node;
}

/**
 * [def_const] -> (const) [list_const]
 *             -> Є
 */
Tree *def_const() {
    Tree *node = new_node(); // [def_const]

    if (!check_peek_token(1, CONSTANTE)) {
        node->child = new_node(); // Є
        node->child->token.valor = "";
        return node;
    }

    next_token();
    node->child = new_node(); // (const)
    node->child->brother = list_const(); // [list_const]

    return node;
}

/**
 * [declarations] -> [def_const] [def_type] [def_variable] [list_function]
 *                -> Є
 */
Tree *declarations() {
    Tree *node = new_node(); // [declarations]

    if (!check_peek_token(4, CONSTANTE, TIPO, VAR, FUNCTION)) {
        node->child = new_node(); // Є
        node->child->token.valor = "";
        return node;
    }

    node->child = def_const();
    node->child->brother = def_type();
    node->child->brother->brother = def_variable();
    node->child->brother->brother->brother = list_function();

    return node;
}

/**
 *  [body] -> [declarations] [body]
 *         -> (begin) [list_command] (end)
 */
Tree *body() {
    Tree *node = new_node();  // [body]

    if (!check_peek_token(1, INICIO)) {
        node->child = declarations(); // [declarations]
        node->child->brother = body(); // [body]
        return node;
    }

    next_token();
    node->child = new_node(); // (begin)
    node->child->brother = list_command(); // [list_command]

    if (!check_peek_token(1, FIM)) {
        return syntax_error();
    }

    next_token();
    node->child->brother->brother = new_node(); // (end)

    return node;
}

/**
 * [program] -> (program) [id] (;) [body]
 */
Tree *program() {
    if (!check_peek_token(1, PROGRAMA)) {
        return syntax_error();
    }

    next_token();
    Tree *node = new_node();     // [program]
    node->child = new_node();    // (program)
    node->child->brother = id(); // [id]

    if (!check_peek_token(1, PT_VIRGULA)) {
        return syntax_error();
    }

    next_token();
    node->child->brother->brother = new_node();      // (;)
    node->child->brother->brother->brother = body(); // [body]

    return node;
}

Tree *parse_tree(Lex *_lex) {
    lex = _lex;
    return program();
}

void print_parse_tree(Tree *tree) {
    if (tree == NULL) {
        return;
    }

    if (tree->child == NULL && strcmp(tree->token.valor, "") != 0) {
        if (tree->token.tipo == INTEIRO) {
            printf("%d ", *(int *) tree->token.valor);
        } else if (tree->token.tipo == REAL) {
            printf("%g ", *(double *) tree->token.valor);
        } else {
            printf("%s ", (char *) tree->token.valor);
        }
    }

    print_parse_tree(tree->child);
    print_parse_tree(tree->brother);
}
