#ifndef ARGPARSE_C_H
#define ARGPARSE_C_H

#include "global.h"

typedef enum names_tokens_argparse_c {
    token_val = 1,
    token_arg_sort, // version "corta" de los argumentos -<name>
    token_arg_long, // version larga de los argumentos --<name>
} names_tokens_argparse_c;

Token_build_t* token_analysis_argparse_c (Lexer_t *lexer);

typedef struct argparse_t {
    char        *all_arguments; // todos los argumentos en una sola cadena
    size_t  size_all_arguments; // tamaño de todos los argumentos en una sola cadena
    int                   argc; // cantidad de argumentos obtenidos desde el main
    char                **argv; // arreglo de argumentos obtenidos desde el main
    Lexer_t              lexer; // lexer para analizar los argumentos
    HashTable      *table_args; // tabla hash con los argumentos
} argparse_t;

typedef struct data_flag_t {
    char* name;                 // nombre del argumento
    char* short_flag;           // flag corta (opcional si se define long_flag )
    char* long_flag;            // flag largo (opcional si se define short_flag)
    char* description;          // descripcion del argumento
    uint8_t number_arguments;   // numero de flags asociados al argumento
    uint8_t required_arguments; // numero de argumentos requeridos por el argumento
} data_flag_t;

void free_argparse(argparse_t *self);
argparse_t init_argparse(int argc, char** argv, data_flag_t* flags, size_t size_flags);

#endif