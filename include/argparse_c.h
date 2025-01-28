#ifndef ARGPARSE_C_H
#define ARGPARSE_C_H

#include "global.h"

typedef enum names_tokens_argparse_c {
    token_val = 1,
    token_arg_short, // version "corta" de los argumentos -<name>
    token_arg_long, // version larga de los argumentos --<name>
} names_tokens_argparse_c;

Token_build_t* token_analysis_argparse_c (Lexer_t *lexer);

typedef struct argparse_t {
    char        *all_arguments; // todos los argumentos en una sola cadena
    size_t  size_all_arguments; // tamaño de todos los argumentos en una sola cadena
    int                   argc; // cantidad de argumentos obtenidos desde el main
    char                **argv; // arreglo de argumentos obtenidos desde el main
    Lexer_t              lexer; // lexer para analizar los argumentos
    HashTable      *table_args; // tabla hash con los argumentos parseados
    HashTable      *table_data_flag_t; // tabla hash con los datos de las flags definidas
} argparse_t;

typedef struct data_flag_t {
    char* name;                 // nombre del argumento
    char* short_flag;           // flag corta (opcional si se define long_flag )
    char* long_flag;            // flag largo (opcional si se define short_flag)
    char* description;          // descripcion del argumento
    uint8_t number_arguments;   // numero de flags asociados al argumento
    uint8_t required_arguments; // numero de argumentos requeridos por el argumento
    bool    exists_flag; 
} data_flag_t;

#define VOLATILE_DATA

#define OUT_DATA_UNION 
#define IN_DATA_UNION 
/* unión para retornar datos de las funciones de tipo f_token_process*/
typedef union data_ret_f_token_process {
    struct {
        // cantidad de flags cortas retornadas por count_number_flags_short
        OUT_DATA_UNION uint16_t number_short_flags; 
    } count_number_flags_short;
    struct {
        // cantidad de flags cortas retornadas por count_number_flags_long
        OUT_DATA_UNION uint16_t number_long_flags; 
    } count_number_flags_long;

    struct {
        IN_DATA_UNION argparse_t** arguments; // apuntador al argparse_t
    } put_flags_short_and_long;
} data_ret_f_token_process; 

typedef void (*f_token_process)(Lexer_t *, Token_build_t*, data_ret_f_token_process*);

void free_argparse(argparse_t **self);
argparse_t* init_argparse(int argc, char** argv, data_flag_t* flags, size_t size_flags);
bool check_flags_repetition(
    data_flag_t* flags,                 // flags con la informacion
    size_t size_flags,                  // cantidad de flags
    char* (*get_flag_f)(data_flag_t*)   // funcion que devuelve una flag corta o larga
);

void count_number_flags_short(
    Lexer_t * lexer,                                        // lexer del que obtener los tokens via hash_table
    Token_build_t* tok,                                     // token actual a analizar
    data_ret_f_token_process *data_ret_of_f_token_process   // datos retornados por el callback
);

void count_number_flags_long(
    Lexer_t * lexer,                                        // lexer del que obtener los tokens via hash_table
    Token_build_t* tok,                                     // token actual a analizar
    data_ret_f_token_process *data_ret_of_f_token_process   // datos retornados por el callback
);

static inline char* get_short_flag(data_flag_t  *self) {
    if (!self) return NULL;
    return self->short_flag;
}
static inline char* get_long_flag(data_flag_t  *self) {
    if (!self) return NULL;
    return self->long_flag;
}

HashTable* convert_data_flag_t_arr_to_hash_table(data_flag_t* flags, size_t size_flags);

#ifdef DEBUG_ENABLE
#define _check_flags_repetition(flags)                                                    \
    (check_flags_repetition(flags, sizeof(flags) / sizeof(data_flag_t), get_long_flag) || \
    check_flags_repetition(flags, sizeof(flags) / sizeof(data_flag_t), get_short_flag))
#else 
#define _check_flags_repetition(flags) false
#endif

#endif