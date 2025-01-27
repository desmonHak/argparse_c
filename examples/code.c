#include "argparse_c.h"

int main(int argc, char **argv) {

    #ifdef _WIN32
    #else
    sigset_t sa_mask;
    sigemptyset(&sa_mask); // Inicializa la máscara
    #endif


    data_flag_t flags[] = {
        (data_flag_t){
            .long_flag          = "verbose", 
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
        }
    };
    if (_check_flags_repetition(flags)) {
        printf("Error: Alguna flag que se definio esta repetia está repetida.\n");
        return 1;
    }
    argparse_t *arguments = init_argparse(argc, argv, flags, sizeof(flags) / sizeof(data_flag_t) );
    free_argparse(&arguments);
        printf("arguments %p\n", arguments);

    puts("Exit...");
}