#include "argparse_c.h"

int main(int argc, char **argv) {

    data_flag_t flags[] = {
        (data_flag_t){
            .long_flag          = "--verbose", 
            .short_flag         = '-v', 
            .description        = "Imprime más información durante la ejecución",
            .number_arguments   = 0,
            .optional_arguments = 0,
            .required_arguments = 0,
            .name               = "verbose",
        }, 
        (data_flag_t){
            .long_flag          = "--arg", 
            .short_flag         = '-a', 
            .description        = "argumento random 1",
            .number_arguments   = 4,
            .optional_arguments = 3,
            .required_arguments = 1,
            .name               = "arumento 1",
        },
            
    };

    argparse_t arguments = init_argparse(argc, argv, flags, sizeof(flags) / sizeof(data_flag_t) );

    free_argparse(&arguments);
    puts("Exit...");
}