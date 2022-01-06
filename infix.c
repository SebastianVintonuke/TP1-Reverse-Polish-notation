//cc -g -O2 -std=gnu18 -Wall -Werror -pedantic -Wformat=2 -Wshadow -Wpointer-arith -Wunreachable-code -Wconversion -Wno-sign-conversion -Wbad-function-cast -Wno-conversion    infix.c  -lm -o infix calc_helper.c pila.c strutil.c

//valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./infix

#include "calc_helper.h"
#include "strutil.h"

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *get_ope(calc_num ope) {
    if (ope == 0) {
        return "+";
    } else if (ope == 1) {
        return "-";
    } else if (ope == 2) {
        return "*";
    } else if (ope == 3) {
        return "/";
    } else {
        return "^";
    }
} 

char **get_in() {
    int size_imput = 100000;
    char imput[size_imput+1];
    int pos = 0;
    char ch;
    while ((ch = (char)getchar()) != '\n' && ch != EOF) {
        if (pos < size_imput) {
            imput[pos++] = ch;      
        }
    }
    imput[pos] = '\0';
    char **imput_split = infix_split(imput);
    return imput_split;
}

char *infix(char **imput_split) {
    size_t tam = 0;
    for (int i = 0; imput_split[i] != NULL; i++) {
        tam += strlen(imput_split[i]);
    }
    char *result = calloc((tam+1)*2, sizeof(char));
    char *final_p = result;
    if (!result) {
        return false;
    }
    pilanum_t *pila = pila_crear();
    if (!pila) {
        return false;
    }
    calc_num ope;
    for (int i = 0; imput_split[i] != NULL; i++) {
        struct calc_token tok;
        if (!calc_parse(imput_split[i], &tok)) {
            free(result);
            pilanum_destruir(pila);
            return false;
        } else if (tok.type == TOK_NUM) {
            char num_c[strlen(imput_split[i])];
            sprintf(num_c, "%ld", tok.value);
            final_p = stpcpy(final_p, num_c);
            final_p = stpcpy(final_p, " ");
        } else if (tok.type == TOK_OPER) {
            if (tok.oper.op == OP_ADD) {
                if (!pilanum_esta_vacia(pila)) {
                    while ((!pilanum_esta_vacia(pila)) && (pilanum_ver_tope(pila) == OP_ADD || pilanum_ver_tope(pila) == OP_SUB || pilanum_ver_tope(pila) == OP_POW || pilanum_ver_tope(pila) == OP_MUL || pilanum_ver_tope(pila) == OP_DIV)) {
                        desapilar_num(pila, &ope);
                        final_p = stpcpy(final_p, get_ope(ope));
                        final_p = stpcpy(final_p, " "); 
                    }
                }
                apilar_num(pila, (calc_num) OP_ADD);   
            } else if (tok.oper.op == OP_SUB) {
                if (!pilanum_esta_vacia(pila)) {
                    while ((!pilanum_esta_vacia(pila)) && (pilanum_ver_tope(pila) == OP_ADD || pilanum_ver_tope(pila) == OP_SUB || pilanum_ver_tope(pila) == OP_POW || pilanum_ver_tope(pila) == OP_MUL || pilanum_ver_tope(pila) == OP_DIV)) {
                        desapilar_num(pila, &ope);
                        final_p = stpcpy(final_p, get_ope(ope));
                        final_p = stpcpy(final_p, " ");
                    }
                }
                apilar_num(pila, (calc_num) OP_SUB);
            } else if (tok.oper.op == OP_MUL) {
                if (!pilanum_esta_vacia(pila)) {
                    while ((!pilanum_esta_vacia(pila)) && (pilanum_ver_tope(pila) == OP_POW || pilanum_ver_tope(pila) == OP_MUL || pilanum_ver_tope(pila) == OP_DIV)) {
                        desapilar_num(pila, &ope);
                        final_p = stpcpy(final_p, get_ope(ope));
                        final_p = stpcpy(final_p, " "); 
                    }
                }
                apilar_num(pila, (calc_num) OP_MUL);
            } else if (tok.oper.op == OP_DIV) {
                if (!pilanum_esta_vacia(pila)) {
                    while ((!pilanum_esta_vacia(pila)) && (pilanum_ver_tope(pila) == OP_POW || pilanum_ver_tope(pila) == OP_MUL || pilanum_ver_tope(pila) == OP_DIV)) {
                        desapilar_num(pila, &ope);
                        final_p = stpcpy(final_p, get_ope(ope));
                        final_p = stpcpy(final_p, " ");  
                    }
                }
                apilar_num(pila, (calc_num) OP_DIV); 
            } else if (tok.oper.op == OP_POW) {
                apilar_num(pila, (calc_num) OP_POW);
            }
        } else if (tok.type == TOK_LPAREN) {
            apilar_num(pila, (calc_num) 5);
        } else if (tok.type == TOK_RPAREN) {
            if (!pilanum_esta_vacia(pila)) {
                while ((!pilanum_esta_vacia(pila)) && (pilanum_ver_tope(pila) != 5)) {
                    desapilar_num(pila, &ope);
                    final_p = stpcpy(final_p, get_ope(ope));
                    final_p = stpcpy(final_p, " "); 
                }
            }
            desapilar_num(pila, &ope);
        }
    }
    while (desapilar_num(pila, &ope)) {
        if (ope != 5) {
            final_p = stpcpy(final_p, get_ope(ope));
            final_p = stpcpy(final_p, " ");
        }
    }
    pilanum_destruir(pila);
    return result;
}

int main(void) {
    char **imput_split;
    while (true) {
        imput_split = get_in();
        if (imput_split[0] == NULL) {
            break;
        }
        char *result = infix(imput_split);
        printf("%s \n", result);
        free_strv(imput_split);
        free(result);
    }
    free_strv(imput_split);
    return 0;
}
