#ifndef ARGPARSE_C_C
#define ARGPARSE_C_C

#include "argparse_c.h"

static inline Token_build_t* lexer_parser_arg_long (Lexer_t *lexer) {
    return lexer_parser_id(lexer);
}

static inline Token_build_t* lexer_parser_arg_short (Lexer_t *lexer) {
    return lexer_parser_id(lexer);
}

Token_build_t* token_analysis_argparse_c (Lexer_t *lexer) {
    /*
        Los elementos de tipo `Token_build_t*` devueltos por esta funcion deben ser liberados
        con free por parte del programador.
     */
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(Token_build_t*  , token_analysis_argparse_c)
            TYPE_DATA_DBG(Lexer_t*, "lexer = %p")
        END_TYPE_FUNC_DBG,
        lexer);
    Token_build_t* self;

    repeat_switch:
        if (isdigit(lexer->chartter)) return lexer_parser_number(lexer);
        else if (lexer->chartter == '-' && isdigit(lexer->data[lexer->index + 1])) {
            /* 
             *  se trata de un numero negativo. En el caso de que el programador intente usar un numero negativo,
             *  como flag, no se podra, pues para el lexer & parser son numeros negativos.
             */
            return lexer_parser_number(lexer);
        }

        repear_switch:
        switch (lexer->chartter)
        {
            case '-': // se encontro un posible argumento corto o largo
                lexer_advance(lexer);
                if (lexer->chartter == '-') {
                    lexer_advance(lexer); // si se encontro esto, es un argumento largo
                    self =  lexer_parser_arg_long(lexer);
                    self->token = ((Token_t*)get(lexer->hash_table, "--"));
                } else {
                    self = lexer_parser_arg_short(lexer);
                    self->token = ((Token_t*)get(lexer->hash_table, "-"));
                }
                lexer_advance(lexer); 
                return self;
            case '"':
                lexer_advance(lexer);
                if (lexer->chartter != '"' ) goto repear_switch;
            case '\'':
                return lexer_parser_string(lexer);
            case ' ':
                lexer_advance(lexer);
                goto repeat_switch;
            case '\0':
                self = init_token_build(NULL);
                self->token = token_eof;
                return self;
            default:
                self = lexer_parser_id(lexer);
                lexer_advance(lexer); 
                self->token = get(lexer->hash_table, build_token_special(TOKEN_VAL));
                return self;
        }

}


bool check_flags_repetition(
        data_flag_t* flags,                 // flags con la informacion
        size_t size_flags,                  // cantidad de flags
        char* (*get_flag_f)(data_flag_t*)   // funcion que devuelve una flag corta o larga
    ) {
        DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(bool, check_flags_repetition)
            TYPE_DATA_DBG(data_flag_t*, "flags = %p")
            TYPE_DATA_DBG(size_t, "size_flags = %zu")
            TYPE_DATA_DBG(char* (*)(data_flag_t*), "get_flag_f = %p")
        END_TYPE_FUNC_DBG,
        flags, size_flags, get_flag_f);
    // Coste O(n^2) por la comprobacion de repeticion
    char* flags_string1 = NULL, *flags_string2 = NULL;
    uint8_t repetitions = 0;
    /* 
     * si una flag se repite en el array de flags, por ejemplo una cantidad de dos veces,
     * la cantidad de repeticiones sera size_flags * 2, en caso de que se repite una sola vez,
     * repeticiones sera == size_flags.
     */

    for (uint32_t i = 0; i < size_flags; i++) {
        flags_string1 = get_flag_f(&(flags[i])); // para cada flag i
        for (uint32_t j = 0; j < size_flags; j++) {
            // se comprueba si el flag j es igual al flag i
            flags_string2 = get_flag_f(&(flags[j]));
            if (strcmp(flags_string1, flags_string2) == 0) {
                repetitions++; // se ha encontrado un flag repetido
            }
            if (repetitions > size_flags) {
                return true; // se ha encontrado un flag repetido
            }
        }
    }
    return false; // no se ha encontrado ningun flag repetido
}

// funcion "privada" autoincremental
static const inline Token_id inc_token(void) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(static const inline Token_id, inc_token)
            TYPE_DATA_DBG(void, "")
        END_TYPE_FUNC_DBG);
    static Token_id my_token_id = token_arg_short+1;
    return my_token_id++;
}

void formated_args(
        Lexer_t *lexer, 
        func_token_analysis token_analysis,                     // funcion que analiza cada token
        f_token_process _f_token_process,                       // funcion que procesa cada token
        data_ret_f_token_process *data_ret_of_f_token_process   // datos retornados por el callback
    ) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(void  , formated_args)
            TYPE_DATA_DBG(Lexer_t*, "lexer = %p")
            TYPE_DATA_DBG(func_token_analysis, "token_analysis = %p")
            TYPE_DATA_DBG(f_token_process, "_f_token_process = %p")
            TYPE_DATA_DBG(data_ret_f_token_process, "data_ret_of_f_token_process = %p")
        END_TYPE_FUNC_DBG,
    lexer, token_analysis, _f_token_process, data_ret_of_f_token_process);

    if (lexer == NULL) {
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error: Lexer no inicializado\n");
        return;
    }
    if (token_analysis == NULL) {
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error: Token analysis no inicializado\n");
        return;
    }
    Token_build_t* tok;
    Token_id token_eof    = ((Token_t*)get(lexer->hash_table, build_token_special(TOKEN_EOF)))->type;

    // es necesesario hacerlo si la variable global y static token_id nunca fue definida-
    Token_id token_id    = ((Token_t*)get(lexer->hash_table, build_token_special(TOKEN_ID)))->type;

    while ( 1 ) {
        tok = lexer_next_token(lexer, token_analysis);
        if ( tok == NULL ) {
            DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error: Tok devolvio NULL (tok == NULL)\n");
        } else {
            if ( tok->token == NULL ) {
                DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error: Token no encontrado (tok->token == NULL)\n");
                goto exit_free_tok;
            } else { // imprimir el token
                if ( token_eof == tok->token->type ) break;
                /*print_token(tok->token);
                if (tok->token->type == token_id){
                    printf_color("\ttoken id encontrado: %s\n", (const char *)tok->value_process);
                }*/
                _f_token_process(lexer, tok, data_ret_of_f_token_process); // procesar el token
                if (tok != NULL) {
                    free(tok); tok = NULL;
                }
            }
        }
    }

    exit_free_tok:
    if (tok != NULL) free(tok); // liberar el Token_build_t

    restore_lexer:
    // restaurar el lexer, es necesario para poder seguir operando con el
    restore_lexer(lexer);
}

void count_number_flags_short(
        Lexer_t * lexer,                                        // lexer del que obtener los tokens via hash_table
        Token_build_t* tok,                                     // token actual a analizar
        data_ret_f_token_process *data_ret_of_f_token_process   // datos retornados por el callback
    ) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(void  , count_number_flags_short)
            TYPE_DATA_DBG(Lexer_t*, "lexer = %p")
            TYPE_DATA_DBG(Token_build_t*, "tok = %p")
            TYPE_DATA_DBG(data_ret_f_token_process, "data_ret_of_f_token_process = %p")
        END_TYPE_FUNC_DBG,
    lexer, tok, data_ret_of_f_token_process);
    /*
     * Permite contar la cantidad de flags "cortas" instroducidas en la linea de comandos
     */
    Token_t* token_short_flags = ((Token_t*)get(lexer->hash_table, "-"));
    if (tok->token->type == token_short_flags->type){
        data_ret_of_f_token_process->count_number_flags_short.number_short_flags++; 
    }
}
void count_number_flags_long(
        Lexer_t * lexer,                                        // lexer del que obtener los tokens via hash_table
        Token_build_t* tok,                                     // token actual a analizar
        data_ret_f_token_process *data_ret_of_f_token_process   // datos retornados por el callback
    ) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(void  , count_number_flags_long)
            TYPE_DATA_DBG(Lexer_t*, "lexer = %p")
            TYPE_DATA_DBG(Token_build_t*, "tok = %p")
            TYPE_DATA_DBG(data_ret_f_token_process, "data_ret_of_f_token_process = %p")
        END_TYPE_FUNC_DBG,
    lexer, tok, data_ret_of_f_token_process);
    /*
     * Permite contar la cantidad de flags "largas" instroducidas en la linea de comandos
     */
    Token_t* token_long_flags = ((Token_t*)get(lexer->hash_table, "--"));
    if (tok->token->type == token_long_flags->type){
        data_ret_of_f_token_process->count_number_flags_long.number_long_flags++;
    }
}


void print_data_flag(const data_flag_t* flag) {
    if (!flag) {
        printf("La estructura es NULL.\n");
        return;
    }
    
    printf("Nombre del argumento            : '%s'\n", flag->name ? flag->name : "N/A");
    printf("Flag corta                      : -%s\n", flag->short_flag ? flag->short_flag : "N/A");
    printf("Flag larga                      : --%s\n", flag->long_flag ? flag->long_flag : "N/A");
    printf("Descripcion                     : %s\n", flag->description ? flag->description : "N/A");
    printf("Numero de flags asociados       : %u\n", flag->number_arguments);
    printf("Numero de argumentos requeridos : %u\n", flag->required_arguments);
}


void add_flag_to_hash_table(
    Lexer_t * lexer,                                        // lexer del que obtener los tokens via hash_table
    VOLATILE_DATA Token_build_t* tok,                       // token actual a analizar, es volatil y sera liberado al finalizar esta funcion
    data_ret_f_token_process *data_ret_of_f_token_process   // datos retornados por el callback
) {
    Token_id  token_eof         = ((Token_t*)get(lexer->hash_table, build_token_special(TOKEN_EOF)))->type;

    return_all_func:
    // si se contro un --<flag_long> o un -<flag_short>
    if (tok->token->type == token_arg_long || tok->token->type == token_arg_short){
        
        /*
         * Crear una array dinamico en la tabla de hash, obteniendo la cantidad de argumentos
         * posibles que la flag encontrada puede tener, de esa manera, generar un array con el tamaño
         * especificado.
         */
        argparse_t * arguments_data = *(data_ret_of_f_token_process->put_flags_short_and_long.arguments);
        data_flag_t* flag_info = (data_flag_t*)get(arguments_data->table_data_flag_t, ((const char*)tok->value_process));

        char* flag_actual = tok->value_process;
        if (flag_info != NULL) {
            ArrayList * data_this_flag = (
                (
                    flag_info->number_arguments != 0) ? 
                    createArrayList(0, NULL) 
                    : NULL
                );
            // a pesar de que tok se libere al finalzar la funcion, podemos usarla para calcular el hash
            put(arguments_data->table_args, (const char*)flag_actual, data_this_flag) ; 

            DEBUG_PRINT(DEBUG_LEVEL_INFO, " [Flag encontrada] Agregando flag %s a la tabla de hash\n", (const char*)tok->value_process);
            // print_data_flag(flag_info); // imprimir los datos de la flag encontrada

            size_t args_flags = 0; // cantidad de argumentos que se encontro a la flag

            repeat_searh_flags:
            // si esta flag tiene argumentos y no tiene mas que los especificados
            if (flag_info->number_arguments != 0 && args_flags < flag_info->number_arguments) { 
                /*
                 * hacemos una copia del lexer para seguir operando con el
                 * si se necesita restaurar el lexer a un punto anterior
                 */
                Lexer_t lexer_backup_data = backup_lexer(lexer); 

                /*
                 * avanzamo el lexer al siguiente token
                 */
                Token_build_t *token_desconocido = lexer_next_token(lexer, token_analysis_argparse_c);
                if (token_desconocido == NULL) return;
                if (token_desconocido->token == NULL) return;
                if (token_desconocido->token->type == token_eof) return;
                if (
                    (
                        (token_desconocido->token->type == token_arg_long || 
                        token_desconocido->token->type == token_arg_short)
                    ) && flag_info->number_arguments > 0 // si el token es una flag y tiene argumentos 
                ) {

                    // si la cantidad de argumentos para la flag es menor a la especificada, indicar error
                    if (args_flags <= flag_info->required_arguments) {
                        // restaurar el lexer al estado anterior
                        memcpy(lexer, &lexer_backup_data, sizeof(lexer));
                        printf("Error: Faltan argumentos requeridos para la flag %s\n", (const char*)flag_actual);
                        return;
                    } 
                } else {
                    // poner el token desconocido al array de argumentos de la flag
                    Token_build_t *token_copia = NULL;
                    debug_malloc(Token_build_t, token_copia, sizeof(Token_build_t)); // esto es necesario ya que token_desconocido se libera dentro y fuera de la funcion
                    memcpy(token_copia, token_desconocido, sizeof(Token_build_t));
                    push_back_a(data_this_flag, token_copia);

                    //printf("Agregando argumento %zu para la flag %s -> %s\n", args_flags + 1, (const char*)flag_actual, token_desconocido->value_process);
                    shrink_to_fit(data_this_flag);
                    // forEach(data_this_flag, printTokenBuildInfo); // imprimir los tokens de los argumentos, el arraylist
                    args_flags++;
                    //token_desconocido = lexer_next_token(lexer, token_analysis_argparse_c);
                    free(token_desconocido);
                    goto repeat_searh_flags;
                }
                
            }

        } else {
            printf("Error: Flag %s no encontrada en la tabla de flags\n", (const char*)flag_actual);
        }
    }
}

argparse_t* init_argparse(int argc, char** argv, data_flag_t* flags, size_t size_flags) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(argparse_t, init_argparse)
            TYPE_DATA_DBG(int, "argc = %d")
            TYPE_DATA_DBG(char**, "argv = %p")
        END_TYPE_FUNC_DBG,
        argc, argv);

    argparse_t *self;
    debug_calloc(argparse_t, self, 1, sizeof(argparse_t));
    *self = (argparse_t){.argc = argc, .argv = argv};
    self->size_all_arguments = argc * 3; // multiplicar por 3 para cada argumento por el espacio y las comillas
    for (int i = 1; i < argc; i++){
        self->size_all_arguments += strlen(argv[i]);
    }

    self->all_arguments = NULL;
    debug_calloc(char, self->all_arguments, self->size_all_arguments + 1, sizeof(char));

    size_t cursor = 0;
    for (int i = 1; i < argc; i++) {
        self->all_arguments[cursor++] = '"';             // Agregar comilla inicial
        strcpy(self->all_arguments + cursor, argv[i]);   // Copiar argumento
        cursor += strlen(argv[i]);                      // Ajustar el cursor después de copiar
        self->all_arguments[cursor++] = '"';             // Agregar comilla final
        if (i < argc - 1) {                             // Agregar espacio si no es el último argumento
            self->all_arguments[cursor++] = ' ';
        }
    }
    self->all_arguments[cursor] = '\0';  // Terminar la cadena

    self->lexer = init_lexer(self->all_arguments, self->size_all_arguments);
    printf("%s\n", self->all_arguments);
    func_auto_increment func = inc_token;

    const position positions_tokens[] = {
        push_token(&(self->lexer), create_token(create_name_token(token_val),           build_token_special(TOKEN_VAL),             token_val)),
        push_token(&(self->lexer), create_token(create_name_token(token_arg_short),      "-",                                   token_arg_short)),
        push_token(&(self->lexer), create_token(create_name_token(token_arg_long),      "--",                                  token_arg_long)),
        push_token(&(self->lexer), create_token(create_name_token(token_number),        build_token_special(TOKEN_NUMBER),          inc_token)),
        push_token(&(self->lexer), create_token(create_name_token(token_id),            build_token_special(TOKEN_ID),              inc_token)),
        push_token(&(self->lexer), create_token(create_name_token(token_eof),           build_token_special(TOKEN_EOF),             inc_token)),
        push_token(&(self->lexer), create_token(create_name_token(token_string_simple), build_token_special(TOKEN_STRING_SIMPLE),   inc_token)),
        push_token(&(self->lexer), create_token(create_name_token(token_string_double), build_token_special(TOKEN_STRING_DOUBLE),   inc_token))
    };

    // construir el lexer con los tokens:
    build_lexer(&(self->lexer));

    // print_tokens(&(self->lexer));
    // print_Token_build(&(self->lexer), token_analysis_argparse_c);


    data_ret_f_token_process data = {0};

    formated_args(&(self->lexer), token_analysis_argparse_c, count_number_flags_short, &data);
    //printf("Cantidad de flags cortas: %d\n", data.count_number_flags_short.number_short_flags);
    size_t size_hash_table_flags = data.count_number_flags_short.number_short_flags;

    data.count_number_flags_short.number_short_flags = 0; // restaurar el enum a 0 para la próxima ejecución

    formated_args(&(self->lexer), token_analysis_argparse_c, count_number_flags_long, &data);
    //printf("Cantidad de flags largas: %d\n", data.count_number_flags_long.number_long_flags);
    size_hash_table_flags += data.count_number_flags_long.number_long_flags;

    // convertir el array de flags a una tabla hash para una fácil y mejor manipulación
    self->table_data_flag_t = convert_data_flag_t_arr_to_hash_table(flags, size_flags);

    // calcular previamente el tamaño de la tabla hash necesario para contener todas las flags
    self->table_args = createHashTable(size_hash_table_flags);

    // poner la estructura de datos para almacenar los flags y sus valores en la tabla de hash
    data.put_flags_short_and_long.arguments = &self;

    // agregar todos los flags a la tabla hash con sus respectivos valores
    formated_args(&(self->lexer), token_analysis_argparse_c, add_flag_to_hash_table, &data);

    return self;
}

void free_argparse(argparse_t **self) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(void, free_argparse)
            TYPE_DATA_DBG(argparse_t**, "self = %p")
        END_TYPE_FUNC_DBG,
        self);
    if (self != NULL || *self!= NULL) return;
    
    argparse_t *_self = *self;
    if (_self->table_args != NULL){
        freeHashTable(_self->table_args, free);
        _self->table_args = NULL;
    }
    free_lexer(&(_self->lexer)); 
    free(_self);
    *self = NULL; // poner el puntero liberado a nulo
}


HashTable* convert_data_flag_t_arr_to_hash_table(data_flag_t* flags, size_t size_flags) {
    /*
     * en caso de que flags sea reservado dinamicamente, no se puede liberar mientras el hashtable
     * creado este en uso
     */
    if (flags == NULL) return NULL;

    // crear una tabla hash con la cantidad de flags especificados en size_flags.
    HashTable *hash_table = createHashTable(size_flags*2); // *2 para las flags cortos y largas.
    
    for (size_t i = 0; i < size_flags; i++) {
        if (flags[i].long_flag == NULL){
            if (flags[i].short_flag == NULL){
                // error: no se ha especificado el nombre del flag corto ni el nombre del flag largo.
                return NULL; 
            }
            DEBUG_PRINT(DEBUG_LEVEL_INFO, "Agregar short_flag: %s\n", flags[i].short_flag);
            put(hash_table, flags[i].short_flag, &(flags[i]));
        } else {
            DEBUG_PRINT(DEBUG_LEVEL_INFO, "Agregar long_flag: %s\n", flags[i].long_flag);
            put(hash_table, flags[i].long_flag, &(flags[i]));
            if (flags[i].short_flag != NULL) {
                DEBUG_PRINT(DEBUG_LEVEL_INFO, "Agregar short_flag: %s\n", flags[i].short_flag);
            put(hash_table, flags[i].short_flag, &(flags[i]));
            }
        }
    }
    return hash_table;
}


#endif