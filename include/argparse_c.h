#ifndef ARGPARSE_C_H
#define ARGPARSE_C_H

#include "global.h"

/*
 * Tokens basicos y no predefinidos para analizar los argumentos utilizando un lexer/parser.
 */
typedef enum names_tokens_argparse_c {
    token_val = 1,
    token_arg_short,    // version "corta" de los argumentos -<name>
    token_arg_long,     // version larga de los argumentos --<name>
} names_tokens_argparse_c;

/*
 * funcion que analiza ek miembro 'all_arguments' en busca de los argumentos y flags, tokenizandolos.
 */
Token_build_t* token_analysis_argparse_c (Lexer_t *lexer);

/*
 * Estructura general para parsear los argumentos utilizando un lexer/parser.
 */
typedef struct argparse_t {
    /* 
     * todos los argumentos de 'argv', en una sola 
     * cadena y entre comillas doble cada uno, esto es necesario para 
     * que el lexer pueda analizar los argumentos
     */
    char        *all_arguments;

    /* tamaño de la cadena 'all_arguments' */
    size_t  size_all_arguments;

    // cantidad de argumentos obtenidos desde el main
    int                   argc;

    // arreglo de argumentos obtenidos desde el main
    char                **argv;

    // lexer para analizar los argumentos
    Lexer_t              lexer;

    /* 
     * tabla hash con los argumentos parseados, 
     * se accede usando get(table_args, "nombre_flag"),
     * El valor devuelveto es un ArrayList* que contiene 
     * los datos pasados al argumento, los datos del array
     * son de tipo Token_build_t*, donde el miembro 'value_process'
     * contiene el valor del argumento.
     */
    HashTable      *table_args;        

    /* 
     * tabla hash con los datos de las flags definidas por el programador, 
     * este hash table se usa para procesar los datos de una mejor manera, 
     * no es necesario liberar sus elementos internos siempre y cuando no 
     * hayan sido reservados dinamicamente, solo sera necesario liberar la
     * tabla hash con sus entradas mediante la funcion 'freeHashTable', que
     * se usa en 'free_argparse'.
     * Este campo se puede usar para acceder a los datos de una flag ya 
     * definida, y obtener su informacion, aquella que el programador definio 
     * en primera instancia.
     */
    HashTable      *table_data_flag_t; 
} argparse_t;

/*
 * Estructura que representa un argumento y sus datos asociados.
 * Los datos asociados pueden ser enteros, cadenas, etc.
 * Los argumentos pueden tener flags asociados y estos flags pueden tener argumentos adicionales.
 *
 * @note Los argumentos pueden tener flags asociados y estos flags pueden tener argumentos adicionales.
 *       Estos argumentos adicionales pueden ser enteros, cadenas, etc.
 */
typedef struct data_flag_t {
    char* name;                 // nombre del argumento
    char* short_flag;           // flag corta (opcional si se define long_flag )
    char* long_flag;            // flag largo (opcional si se define short_flag)
    char* description;          // descripcion del argumento
    uint8_t number_arguments;   // numero de flags asociados al argumento
    uint8_t required_arguments; // numero de argumentos requeridos por el argumento

    /*
     * indica si la flag fue usada, sea su version long o short,
     * en caso de ser true, la flag fue usada, en caso contrario, es false.
     * Se puede averiguar que version de la flag fue usada(si la long o la short)
     * usando el metodo get(table_args, "nombre_flag"), donde 'nombre_flag' es aquella 
     * por la que se quiere preguntar, en caso de que la funcion devuelva NULL, querra indicar
     * que la flag no fue usada, o al menos, no esa version, existe la posibilidad de que su omologa si.
     */
    bool exists_flag;                       

    void (*func_flag_exec)(argparse_t *) ;  /* 
                                             * función a ejecutar para este argumento si se da*/
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

/*
 * Permite saber si una flag fue usada en los argumentos de linea de comandos.
 */
/*static inline bool is_flag_exist(argparse_t* args, data_flag_t* flag) {
    if (args == NULL || flag == NULL)       return false; // si la flag o los argumentos es null
    if (args->table_data_flag_t == NULL)    return false; // si la tabla hash de los argumentos definidos es null
    if (args->table_args == NULL)           return false; // si la tabla hash de los argumentos formateados es null

    data_flag_t*flag_actual = NULL;
    const char* flag_string = flag->long_flag; // probar con la version larga de la flag primero    

    // obtiene la flag y retorna si la flag fue usada:
    if (flag_string != NULL && (flag_actual = get(args->table_data_flag_t, flag_string)) != NULL) return flag_actual->exists_flag;
    else {
        // si fallo la version larga, comprobar usando la version corta
        if (flag->short_flag != NULL && (flag_actual = get(args->table_data_flag_t, flag->short_flag)) != NULL) return flag_actual->exists_flag;
        else return false;
    }
}*/

#ifdef DEBUG_ENABLE
#define _check_flags_repetition(flags)                                                    \
    (check_flags_repetition(flags, sizeof(flags) / sizeof(data_flag_t), get_long_flag) || \
    check_flags_repetition(flags, sizeof(flags) / sizeof(data_flag_t), get_short_flag))
#else 
#define _check_flags_repetition(flags) false
#endif

#define arg_val(name) arg_val_ ## name

#endif