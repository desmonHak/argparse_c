#include "argparse_c.h"


typedef enum position_args {
    arg_val(help),
    arg_val(suma),
    arg_val(resta),
    arg_val(div),
    arg_val(mult)
};

void help_display(argparse_t *);
void sumar(argparse_t *data);

data_flag_t flags[] = {
        [arg_val(help)] = (data_flag_t){
            .long_flag          = "help", 
            .short_flag         = "h", 
            .description        = "Muestra ayuda",
            .number_arguments   = 0,
            .required_arguments = 0,
            .name               = "help",
            .func_flag_exec     = help_display
        }, 
        [arg_val(suma)] = (data_flag_t){
            .long_flag          = "suma", 
            .short_flag         = "s", 
            .description        = "permite hacer una suma de dos numeros",
            .number_arguments   = 2,
            .required_arguments = 2,
            .name               = "sumar",
            .func_flag_exec     = sumar
        },
        [arg_val(resta)] = (data_flag_t){
            .long_flag          = "restar", 
            .short_flag         = "r", 
            .description        = "permite hacer una resta de dos numeros",
            .number_arguments   = 2,
            .required_arguments = 2,
            .name               = "sumar",
            .func_flag_exec     = NULL
        },
        [arg_val(div)] = (data_flag_t){
            .long_flag          = "div", 
            .short_flag         = "d", 
            .description        = "permite hacer una division de dos numeros",
            .number_arguments   = 2,
            .required_arguments = 2,
            .name               = "dividir",
            .func_flag_exec     = NULL
        },
        [arg_val(mult)] = (data_flag_t){
            .long_flag          = "mult", 
            .short_flag         = "m", 
            .description        = "permite hacer una multiplicacion de dos numeros",
            .number_arguments   = 2,
            .required_arguments = 2,
            .name               = "multiplicacion",
            .func_flag_exec     = NULL
        }
    };

void sumar(argparse_t *data) {
    ArrayList *arg_data = get(
        data->table_args, 
        flags[arg_val(suma)].short_flag) // obtener los valores del argumento '-s'
        ?: 
    get(
        data->table_args, 
        flags[arg_val(suma)].long_flag // obtener los valores del argumento '--suma'
    );

    if (size_a(arg_data) < 2) {
        printf("Error: Debes indicar dos valores para la suma.\n");
        return;
    }

    Token_build_t* arg1 = (Token_build_t*)arg_data->Array[0];
    Token_build_t* arg2 = (Token_build_t*)arg_data->Array[1];

    char *endptr = NULL;
    printf("Sumando1: %s\n", arg1->value_process);
    printf("Sumando2: %s\n", arg2->value_process);
    int arg1n = strtoll_plus(arg1->value_process, endptr, 10);
    endptr = NULL;
    int arg2n = strtoll_plus(arg2->value_process, endptr, 10);

    printf("Resultado: %d\n", arg1n + arg2n);
}

void help_display(argparse_t *self) {
    printf("Uso:  [OPCIONES]...\n");
    printf("Opciones:\n");
    for (int i = 0; i < sizeof(flags) / sizeof(data_flag_t); i++) {
        printf("  --%-10s -%-10s %s\n", flags[i].long_flag, flags[i].short_flag, flags[i].description);
    }
}

int main(int argc, char **argv) {

    #ifdef _WIN32
    #else
    sigset_t sa_mask;
    sigemptyset(&sa_mask); // Inicializa la máscara
    #endif

    // ./code.elf   --arg 2 3 6 -s hola munfo4 5
    // si se indica que solo 1 arg es obligatorio y el resto opcionales, se lo toma como obligatorios

    if (_check_flags_repetition(flags)) {
        printf("Error: Alguna flag que se definio esta repetia esta repetida.\n");
        return 1;
    }
    argparse_t *arguments = init_argparse(argc, argv, flags, sizeof(flags) / sizeof(data_flag_t) );
    //printHashTable(arguments->table_args);

    if (flags[arg_val(suma)].exists_flag){
        ArrayList *arg_data = get(arguments->table_args, "suma");
        // no es necesario en teoria, por que esto no deberia ocurrir con exists_flag == true
        if (arg_data == NULL) { 
            printf("Error: No se encontro el argumento '--suma'.\n");
            return 1;
        }

        printf("Argumento '--suma' con los siguientes valores %zu:\n", size_a(arg_data));
        forEachNew(arg_data, (void (*)(void *))printTokenBuildInfo);
    }

    // si se usa esto, al usar un "--suma hola 5 -s 4" el 4 sobrescribira el 'hola'
    // si get(arguments->table_args, "s") es null, se devuelve get(arguments->table_args, "suma")
    //ArrayList *arg_s = get(arguments->table_args, "s") ?: get(arguments->table_args, "suma");
    // if (arg_s == NULL) {
    //     printf("Error: No se encontro el argumento '-s'/'--suma'.\n");
    //     return 1;
    // }
    //ArrayList *arg_r = get(arguments->table_args, "r") ?: get(arguments->table_args, "restar");
    // if (arg_r == NULL) {
    //     printf("Error: No se encontro el argumento '-r'/'--restar'.\n");
    //     return 1;
    // }

    //ArrayList *arg_d = get(arguments->table_args, "d") ?: get(arguments->table_args, "div");

    free_argparse(&arguments);
    puts("Exit...");
}