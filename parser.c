#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "lex.h"

Lex *lex;

bool check_token(int n, ...) {
    va_list args;
    va_start(args, n);

    for (int i = 0; i < n; i++) {
        if (lex->current_token.tipo == va_arg(args, int)) {
            va_end(args);
            return true;
        }
    }

    va_end(args);

    return false;
}

Tree *new_node() {
    Tree *node = (Tree *) malloc(sizeof(Tree));
    node->value = lex->current_token.valor;
    node->child = NULL;
    node->brother = NULL;
    return node;
}

void *syntax_error() {
    exit(EXIT_FAILURE);
}

/**
 * [id] -> (id) Seqüência alfanumérica iniciada por char (tratado no lexico)
 */
Tree *id() {
    next_token();

    if (!check_token(1, ID)) {
        return syntax_error();
    }

    Tree *node = new_node();  // [id]
    node->child = new_node(); // (id)

    return node;
}

/**
 * [number] -> (number) Seqüência numérica com a possibilidade da ocorrência de no máximo um ponto
 */
Tree *number() {
    next_token();

    if (!check_token(2, INTEIRO, REAL)) {
        return syntax_error();
    }

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
    next_token();

    if (!check_token(1, ID)) {
        return syntax_error();
    }

    Tree *node = new_node(); // [name]
    node->child = id();      // [id]

    next_token();

    if (check_token(1, PONTO)) {
        node->brother = new_node();      // (.)
        node->brother->brother = name(); // [name]
        return node;
    }

    if (check_token(1, L_BRACKET)) {
        node->brother = new_node();       // ([)
        node->brother->brother = param(); // [param]

        next_token();

        if (!check_token(1, R_BRACKET)) {
            return syntax_error();
        }

        node->brother->brother->brother = new_node(); // (])
        return node;
    }

    prev_token();
    return node;
}

/**
 * [op_math] -> (+)
 *           -> (-)
 *           -> (*)
 *           -> (/)
 */
Tree *op_math() {
    next_token();

    if (check_token(4, PLUS, MINUS, TIMES, SLASH)) {
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
    next_token();

    if (check_token(4, BIGGER_THAN, LESS_THAN, EQUAL, EXCLAMATION)) {
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

    next_token();

    if (check_token(1, ID)) {
        prev_token();
        node->child = name(); // [name]
        return node;
    }

    if (check_token(2, INTEIRO, REAL)) {
        prev_token();
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

    next_token();

    if (check_token(4, PLUS, MINUS, TIMES, SLASH)) {
        prev_token();
        node->child->brother = op_math();
        node->child->brother->brother = expr_math();
    } else {
        prev_token();
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

    next_token();

    if (check_token(4, BIGGER_THAN, LESS_THAN, EQUAL, EXCLAMATION)) {
        prev_token();
        node->child->brother = op_logic();
        node->child->brother->brother = expr_logic();
    } else {
        prev_token();
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
    node->child = param();   // [param]

    next_token();

    if (check_token(1, VIRGULA)) {
        node->child->brother = new_node(); // (,)
        node->child->brother->brother = list_name(); // [list_name]
        return node;
    }

    prev_token();

    return node;
}

/**
 * [list_param] -> (() [list_name] ())
 */
Tree *list_param() {
    next_token();

    if (!check_token(1, L_PAREN)) {
        return syntax_error();
    }

    Tree *node = new_node();  // [list_param]
    node->child = new_node(); // (()
    node->child->brother = list_name(); // [list_name]

    next_token();

    if (!check_token(1, R_PAREN)) {
        return syntax_error();
    }

    node->child->brother->brother = new_node(); // ())

    return node;
}

/**
 * [value] -> [expr_math]
 *         -> [id] [list_param]
 */
Tree *value() {
    Tree *node = new_node(); // [value]

    next_token();

    if (check_token(1, ID)) {
        prev_token();
        node->child = id();
        node->child->brother = list_param();
        return node;
    }

    prev_token();
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

    next_token();

    if (!check_token(1, ELSE)) {
        prev_token();
        node->value = ""; // Є
        return node;
    }

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

    next_token();

    if (check_token(1, WHILE)) {
        node->child = new_node(); // (while)
        node->child->brother = expr_logic(); // [expr_logic]
        node->child->brother->brother = bloc(); // [bloc]
        return node;
    }

    if (check_token(1, IF)) {
        node->child = new_node(); // (if)
        node->child->brother = expr_logic(); // [expr_logic]

        next_token();

        if (!check_token(1, THEN)) {
            return syntax_error();
        }

        node->child->brother->brother = new_node(); // (then)
        node->child->brother->brother->brother = bloc(); // [bloc]
        node->child->brother->brother->brother->brother = _else(); // [_else]
    }

    if (check_token(1, WRITE)) {
        node->child = new_node(); // (write)
        node->child->brother = const_value(); // [const_value]
    }

    if (check_token(1, READ)) {
        node->child = new_node(); // (read)
        node->child->brother = name(); // [name]
    }

    node->child = name(); // [name]

    if (!check_token(1, ATTR)) {
        return syntax_error();
    }

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

    next_token();

    if (!check_token(5, WHILE, IF, WRITE, READ, ID)) {
        prev_token();
        node->child = new_node(); // Є
        node->child->value = "";
        return node;
    }

    prev_token();

    node->child = command(); // [command]

    next_token();

    if (!check_token(1, PT_VIRGULA)) {
        return syntax_error();
    }

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

    next_token();

    if (check_token(1, INICIO)) {
        node->child = new_node(); // (begin)
        node->child->brother = list_command(); // [list_command]

        if (!check_token(1, FIM)) {
            return syntax_error();
        }

        node->child->brother->brother = new_node(); // (end);
        return node;
    }

    prev_token();
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

    next_token();

    if (check_token(6, INICIO, WHILE, IF, WRITE, READ, ID)) {
        prev_token();
        node->child = bloc(); // [bloc]
        return node;
    }

    prev_token();

    node->child = def_variable();  // [def_variable]
    node->child->brother = bloc(); // [bloc]

    return node;
}

Tree *list_variable();

Tree *data_type();

/**
 * [name_function] -> (function) [id] (() [list_variable] ()) (:) [data_type]
 */
Tree *name_function() {
    next_token();

    if (!check_token(1, FUNCTION)) {
        return syntax_error();
    }

    Tree *node = new_node(); // [name_function]
    node->child = id(); // [id]

    next_token();

    if (!check_token(1, L_PAREN)) {
        return syntax_error();
    }

    node->child->brother = new_node(); // (()
    node->child->brother->brother = list_variable(); // [list_variable]

    next_token();

    if (!check_token(1, R_PAREN)) {
        return syntax_error();
    }

    node->child->brother->brother->brother = new_node(); // ())

    next_token();

    if (!check_token(1, DOIS_PONTOS)) {
        return syntax_error();
    }

    node->child->brother->brother->brother->brother = new_node(); // (:)
    node->child->brother->brother->brother->brother->brother = data_type(); // [data_type]

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

    next_token();

    if (!check_token(1, FUNCTION)) {
        prev_token();
        node->child = new_node(); // Є
        node->child->value = "";
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

    next_token();

    if (check_token(2, INTEIRO, REAL)) {
        node->child = new_node(); // (integer|real)
        return node;
    }

    if (check_token(1, VETOR)) {
        node->child = new_node(); // (array)

        next_token();

        if (!check_token(1, L_BRACKET)) {
            return syntax_error();
        }

        node->child->brother = new_node(); // ([)
        node->child->brother = number(); // [number]

        next_token();

        if (!check_token(1, R_BRACKET)) {
            return syntax_error();
        }

        node->child->brother = new_node(); // (])
        node->child->brother = data_type(); // [data_type]

        return node;
    }

    if (check_token(1, RECORD)) {
        node->child = new_node(); // (record)
        node->child->brother = list_variable(); // [list_variable]

        next_token();

        if (!check_token(1, FIM)) {
            return syntax_error();
        }

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

    next_token();

    if (!check_token(1, VIRGULA)) {
        prev_token();
        return node;
    }

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

    next_token();

    if (!check_token(1, DOIS_PONTOS)) {
        return syntax_error();
    }

    node->child->brother = new_node(); // (:)
    node->child->brother->brother = data_type(); // [data_type]

    return node;
}

/**
 * [list_variable] -> [variable] (;) [list_variable]
 *                 -> [variable]
 */
Tree *list_variable() {
    Tree *node = new_node(); // [list_variable]
    node->child = variable();

    next_token();

    if (!check_token(1, PT_VIRGULA)) {
        prev_token();
        return node;
    }

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

    if (!check_token(1, EQUAL)) {
        return syntax_error();
    }

    node->child->brother = new_node(); // (=)
    node->child->brother->brother = data_type(); // [data_type]

    return node;
}

/**
 * [list_type] -> [type] (;) [list_type]
 *             -> [type]
 */
Tree *list_type() {
    Tree *node = new_node(); // [list_type]
    node->child = type();

    next_token();

    if (!check_token(1, PT_VIRGULA)) {
        prev_token();
        return node;
    }

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

    next_token();

    if (check_token(1, STRING)) {
        node->child = new_node();
        return node;
    }

    prev_token();
    node->child = expr_math(); // [expr_math]

    return node;
}

/**
 * [constant] -> [id] (=) [const_value] (;)
 */
Tree *constant() {
    Tree *node = new_node(); // [constant]
    node->child = id(); // [id]

    next_token();

    if (!check_token(1, EQUAL)) {
        return syntax_error();
    }

    node->child->brother = new_node(); // (=)
    node->child->brother->brother = const_value(); // [const_value]

    next_token();

    if (!check_token(1, PT_VIRGULA)) {
        return syntax_error();
    }

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

    next_token();

    if (!check_token(1, VAR)) {
        prev_token();
        node->child = new_node(); // Є
        node->child->value = "";
        return node;
    }

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

    next_token();

    if (!check_token(1, TIPO)) {
        prev_token();
        node->child = new_node(); // Є
        node->child->value = "";
        return node;
    }

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

    next_token();

    if (!check_token(1, CONSTANTE)) {
        prev_token();
        node->child = new_node(); // Є
        node->child->value = "";
        return node;
    }

    node->child = new_node(); // (const)
    node->child->brother = list_const();

    return node;
}

/**
 * [declarations] -> [def_const] [def_type] [def_variable] [list_function]
 *                -> Є
 */
Tree *declarations() {
    Tree *node = new_node(); // [declarations]

    next_token();

    if (!check_token(4, CONSTANTE, TIPO, VAR, FUNCTION)) {
        prev_token();
        node->child = new_node(); // Є
        node->child->value = "";
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

    next_token();

    if (!check_token(1, INICIO)) {
        prev_token();
        node->child = declarations(); // [declarations]
        node->child->brother = body();
        return node;
    }

    node->child = new_node(); // (begin)
    node->child->brother = list_command(); // [list_command]

    next_token();

    if (!check_token(1, FIM)) {
        return syntax_error();
    }

    node->child->brother->brother = new_node(); // (end)

    return node;
}

/**
 * [program] -> (program) [id] (;) [body]
 */
Tree *program() {
    next_token();

    if (!check_token(1, PROGRAMA)) {
        return syntax_error();
    }

    Tree *node = new_node();     // [program]
    node->child = new_node();    // (program)
    node->child->brother = id(); // [id]

    next_token();

    if (!check_token(1, PT_VIRGULA)) {
        return syntax_error();
    }

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

    if (tree->child == NULL) {
        printf("%s ", (char *) tree->value);
    }

    print_parse_tree(tree->child);
    print_parse_tree(tree->brother);
}
