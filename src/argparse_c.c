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
                } else self = lexer_parser_arg_short(lexer);
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



static inline char* get_sort_flag(data_flag_t  *self) {
    return self->short_flag;
}
static inline char* get_long_flag(data_flag_t  *self) {
    return self->long_flag;
}

bool check_flags_repetition(
        data_flag_t* flags,                 // flags con la informacion
        size_t size_flags,                  // cantidad de flags
        char* (*get_flag_f)(data_flag_t*)   // funcion que devuelve una flag corta o larga
    ) {

    // Coste O(n^2) por la comprobacion de repeticion
    char* flags_string = NULL;
    for (int i = 0; i < size_flags; i++) {
        flags_string = get_flag_f(&(flags[i]));
        for (int j = 0; i < size_flags; j++) {

        }
    }

}

// funcion "privada" autoincremental
static const inline Token_id inc_token(void) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(static const inline Token_id, inc_token)
            TYPE_DATA_DBG(void, "")
        END_TYPE_FUNC_DBG);
    static Token_id my_token_id = token_arg_sort+1;
    return my_token_id++;
}


argparse_t init_argparse(int argc, char** argv, data_flag_t* flags, size_t size_flags) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(argparse_t, init_argparse)
            TYPE_DATA_DBG(int, "argc = %d")
            TYPE_DATA_DBG(char**, "argv = %p")
        END_TYPE_FUNC_DBG,
        argc, argv);

    argparse_t self = {.argc = argc, .argv = argv};
    self.size_all_arguments = argc * 3; // multiplicar por 3 para cada argumento por el espacio y las comillas
    for (int i = 1; i < argc; i++){
        self.size_all_arguments += strlen(argv[i]);
    }

    self.all_arguments = NULL;
    debug_calloc(char, self.all_arguments, self.size_all_arguments + 1, sizeof(char));

    size_t cursor = 0;
    for (int i = 1; i < argc; i++) {
        self.all_arguments[cursor++] = '"';             // Agregar comilla inicial
        strcpy(self.all_arguments + cursor, argv[i]);   // Copiar argumento
        cursor += strlen(argv[i]);                      // Ajustar el cursor después de copiar
        self.all_arguments[cursor++] = '"';             // Agregar comilla final
        if (i < argc - 1) {                             // Agregar espacio si no es el último argumento
            self.all_arguments[cursor++] = ' ';
        }
    }
    self.all_arguments[cursor] = '\0';  // Terminar la cadena

    self.lexer = init_lexer(self.all_arguments, self.size_all_arguments);
    printf("%s\n", self.all_arguments);
    func_auto_increment func = inc_token;

    const position positions_tokens[] = {
        push_token(&(self.lexer), create_token(create_name_token(token_val),           build_token_special(TOKEN_VAL),             token_val)),
        push_token(&(self.lexer), create_token(create_name_token(token_arg_sort),       "-",                                  token_arg_sort)),
        push_token(&(self.lexer), create_token(create_name_token(token_number),        build_token_special(TOKEN_NUMBER),          inc_token)),
        push_token(&(self.lexer), create_token(create_name_token(token_id),            build_token_special(TOKEN_ID),              inc_token)),
        push_token(&(self.lexer), create_token(create_name_token(token_eof),           build_token_special(TOKEN_EOF),             inc_token)),
        push_token(&(self.lexer), create_token(create_name_token(token_string_simple), build_token_special(TOKEN_STRING_SIMPLE),   inc_token)),
        push_token(&(self.lexer), create_token(create_name_token(token_string_double), build_token_special(TOKEN_STRING_DOUBLE),   inc_token))
    };

    // construir el lexer con los tokens:
    build_lexer(&(self.lexer));

    // print_tokens(&(self.lexer));
    print_Token_build(&(self.lexer), token_analysis_argparse_c);

    
    //self.table_args = createHashTable();

}

void free_argparse(argparse_t *self) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(void, free_argparse)
            TYPE_DATA_DBG(argparse_t, "self = %p")
        END_TYPE_FUNC_DBG,
        self);
    //freeHashTable(arguments);
    //free_lexer(&(self->lexer)); // la funcion destructora de vectores dinamicos tiene errores
    // y crashea al intentar liberar elementos liberados
}



#endif