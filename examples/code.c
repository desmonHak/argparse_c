#include "argparse_c.h"

int main(int argc, char **argv) {

    #ifdef _WIN32
    #else
    sigset_t sa_mask;
    sigemptyset(&sa_mask); // Inicializa la máscara
    #endif


    data_flag_t flags[] = {
        (data_flag_t){
            .long_flag          = "p-hola", 
            .short_flag         = "v", 
            .description        = "Imprime mas informacion durante la ejecucion",
            .number_arguments   = 0,
            .required_arguments = 0,
            .name               = "verbose",
        }, 
        (data_flag_t){
            .long_flag          = "arg", 
            .short_flag         = "a", 
            .description        = "argumento random 1",
            .number_arguments   = 4,
            .required_arguments = 1,
            .name               = "arumento 1",
        },
        (data_flag_t){
            .long_flag          = "suma", 
            .short_flag         = "s", 
            .description        = "permite hacer una suma de dos numeros",
            .number_arguments   = 2,
            .required_arguments = 2,
            .name               = "sumar",
        },
        (data_flag_t){
            .long_flag          = "restar", 
            .short_flag         = "r", 
            .description        = "permite hacer una resta de dos numeros",
            .number_arguments   = 2,
            .required_arguments = 2,
            .name               = "sumar",
        },
        (data_flag_t){
            .long_flag          = "div", 
            .short_flag         = "d", 
            .description        = "permite hacer una division de dos numeros",
            .number_arguments   = 2,
            .required_arguments = 2,
            .name               = "dividir",
        },
        (data_flag_t){
            .long_flag          = "mult", 
            .short_flag         = "m", 
            .description        = "permite hacer una multiplicacion de dos numeros",
            .number_arguments   = 2,
            .required_arguments = 2,
            .name               = "multiplicacion",
        }
    };
    if (_check_flags_repetition(flags)) {
        printf("Error: Alguna flag que se definio esta repetia esta repetida.\n");
        return 1;
    }
    argparse_t *arguments = init_argparse(argc, argv, flags, sizeof(flags) / sizeof(data_flag_t) );

    ArrayList *arg_data = get(arguments->table_args, "arg");
    if (arg_data == NULL) {
        printf("Error: No se encontro el argumento '--arg'.\n");
        return 1;
    }

    printf("Argumento '--arg' con los siguientes valores %zu:\n", size_a(arg_data));
    forEachNew(arg_data, (void (*)(void *))printTokenBuildInfo);

    for (int i = 0; i < arg_data->Size; i++) {
        printf("dato[%d]: %p\n", i, arg_data->Array[i]);
        printTokenBuildInfo((void*)arg_data->Array[i]);
    }

    free_argparse(&arguments);

    puts("Exit...");
}