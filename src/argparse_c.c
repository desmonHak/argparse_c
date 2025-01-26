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
                free(tok);
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

    // calcular previamente el tamaño de la tabla hash necesario para contener todas las flags
    self->table_args = createHashTable(size_hash_table_flags);
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
        freeHashTable(_self->table_args);
        _self->table_args = NULL;
    }
    free_lexer(&(_self->lexer)); 
    free(_self);
    *self = NULL; // poner el puntero liberado a nulo
}



#endif