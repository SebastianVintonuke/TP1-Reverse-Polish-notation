//cc -g -O2 -std=gnu18 -Wall -Werror -pedantic -Wformat=2 -Wshadow -Wpointer-arith -Wunreachable-code -Wconversion -Wno-sign-conversion -Wbad-function-cast -Wno-conversion    dc.c  -lm -o dc calc_helper.c pila.c strutil.c

//valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./dc

#include "calc_helper.h"
#include "strutil.h"

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

char **get_in() {
    int size_imput = 100;
    char imput[size_imput+1];
    int pos = 0;
    char ch;
    while ((ch = (char)getchar()) != '\n' && ch != EOF) {
        if (pos < size_imput) {
            imput[pos++] = ch;      
        }
    }
    imput[pos] = '\0';
    char **imput_split = dc_split(imput);
    return imput_split;
}

bool calc(char **entrada_split, calc_num *result) {
    pilanum_t *pila = pilanum_crear();
    if (!pila) {
        return false;
    }
    calc_num result_p = 0;
    calc_num num1;
    calc_num num2;
    calc_num num3;
    for (int i = 0; entrada_split[i] != NULL; i++) {
        struct calc_token tok;
        if (!calc_parse(entrada_split[i], &tok)) {
            printf("ERROR\n"); //CARACTER NO RECONOCIBLE
            pilanum_destruir(pila);
            return false;
        } else if (tok.type == TOK_NUM) {
            apilar_num(pila, tok.value);
        } else if (tok.type == TOK_OPER) {
            if (tok.oper.op == OP_ADD) {
                if (!desapilar_num(pila, &num1) || !desapilar_num(pila, &num2)) {
                    printf("ERROR\n"); //NO HAY SUFICIENTES OPERANDOS
                    pilanum_destruir(pila);
                    return false;
                }
                result_p = num2 + num1;
            } else if (tok.oper.op == OP_SUB) {
                if (!desapilar_num(pila, &num1) || !desapilar_num(pila, &num2)) {
                    printf("ERROR\n"); //NO HAY SUFICIENTES OPERANDOS
                    pilanum_destruir(pila);
                    return false;
                }
                result_p = num2 - num1;          
            } else if (tok.oper.op == OP_MUL) {
                if (!desapilar_num(pila, &num1) || !desapilar_num(pila, &num2)) {
                    printf("ERROR\n"); //NO HAY SUFICIENTES OPERANDOS
                    pilanum_destruir(pila);
                    return false;
                }
                result_p = num2 * num1;     
            } else if (tok.oper.op == OP_DIV) {
                if (!desapilar_num(pila, &num1) || !desapilar_num(pila, &num2)) {
                    printf("ERROR\n"); //NO HAY SUFICIENTES OPERANDOS
                    pilanum_destruir(pila);
                    return false;
                }
                if (num1 == 0) {
                    printf("ERROR\n"); //DIVISIÓN ENTRE CERO
                    pilanum_destruir(pila);
                    return false;
                }
                result_p = num2 / num1;
            } else if (tok.oper.op == (calc_num) OP_POW) {
                if (!desapilar_num(pila, &num1) || !desapilar_num(pila, &num2)) {
                    printf("ERROR\n"); //NO HAY SUFICIENTES OPERANDOS
                    pilanum_destruir(pila);
                    return false;
                }
                if (num1 < 0) {
                    printf("ERROR\n"); //POTENCIA CON EXPONENTE NEGATIVO
                    pilanum_destruir(pila);
                    return false; 
                }
                result_p = (pow((double)num2, (double)num1));
            } else if (tok.oper.op == (calc_num) OP_LOG) {
                if (!desapilar_num(pila, &num1) || !desapilar_num(pila, &num2)) {
                    printf("ERROR\n"); //NO HAY SUFICIENTES OPERANDOS
                    pilanum_destruir(pila);
                    return false;
                }
                if (num1 <= 0 || num1 == 1) {
                    printf("ERROR\n"); //BASE DE LOGARITMO NO DEFINIDA
                    pilanum_destruir(pila);
                    return false;
                }
                if (num2 < 0) {
                    printf("ERROR\n"); //ARGUMENTO DE LOGARITMO NEGATIVO
                    pilanum_destruir(pila);
                    return false;
                }
                result_p = (calc_num) (log((double)num2) / log((double)num1));
            } else if (tok.oper.op == OP_RAIZ) {
                if (!desapilar_num(pila, &num1)) {
                    printf("ERROR\n"); //NO HAY SUFICIENTES OPERANDOS
                    pilanum_destruir(pila);
                    return false;
                }
                if (num1 < 0) {
                    printf("ERROR\n"); //RAIZ DE UN NUMERO NEGATIVO
                    pilanum_destruir(pila);
                    return false; 
                }
                result_p = (sqrt((double)num1));
            } else if (tok.oper.op == OP_TERN) {
                if (!desapilar_num(pila, &num1) || !desapilar_num(pila, &num2) || !desapilar_num(pila, &num3)) {
                    printf("ERROR\n"); //NO HAY SUFICIENTES OPERANDOS
                    pilanum_destruir(pila);
                    return false;
                }
                result_p = (num3 ? num2 : num1);   
            }
            apilar_num(pila, result_p);
        } else if (tok.type == TOK_LPAREN || tok.type == TOK_RPAREN) {
            continue;
        }
    }
    desapilar_num(pila, result);
    if (desapilar_num(pila, &num1)) {
        printf("ERROR\n"); //QUEDAN VALORES EN LA PILA
        pilanum_destruir(pila);
        return false;    
    }
    pilanum_destruir(pila);
    return true;
}

int main(void) {
    char **imput_split;
    while (true) {
        imput_split = get_in();
        if (imput_split[0] == NULL) {
            break; 
        }
        calc_num result;
        if (calc(imput_split, &result)) {
            printf("%ld\n", result);
        }
        free_strv(imput_split);
    }
    free_strv(imput_split);
    return 0;
}
