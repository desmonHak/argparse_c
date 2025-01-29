# argpaser_c

----

Esta es una pequeña libreria para parsear argumentos desde la linea de comandos, inspirada en la libreria `argpaser` de python.

----

Primero de sebe definir las flags posibles que el programa puede recibir, ademas de si las flags reciben parametros, como nombre de la flag, o que funcion auto-ejecutar si la flag es invocada.

Primneo se define un ``enum`` para los parametros, por ejemplo, si nuestro programa tiene flags ``help``, `suma`, `resta`, `division` y `muñtiplicacion`  

```c
typedef enum position_args {
    arg_val(help),
    arg_val(suma),
    arg_val(resta),
    arg_val(div),
    arg_val(mult)
};
```

Luego se a de declarar las funcion que se van a ejecutar si la flag es usada/llamada:
```c
void help_display(argparse_t *);
void sumar(argparse_t *data);
void restar(argparse_t *data);
void dividir(argparse_t *data);
void multiplicar(argparse_t *data);
```

Estas deben de ser del tipo `void (*)(argparse_t*)`, es decir, una funcion de retorno `void` y que reciba un argumento de tipo `argparse_t*`. Este argumento se autopasa a la funcion para poder obtener los argumentos de la flag y su informacion.

----

## Definir posibles flags a usar:

Ahora con estos datos, se puede proceder a hacer un array que contenga definiciones de las flags que el programa puede usar.

El tipo de dato debe ser `data_flag_t` para cada elemento de este array, formando algo similar a lo siguiente:

```c
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
        .func_flag_exec     = restar
    },
    // mas flags si es necesario.
}
```
- ``long_flag`` : nombre para la flag en su verion larga, por ejemplo, si se define "help", se formateara a "--help".
- ``short_flag`` :  nombre para la flag en su verion corta, por ejemplo, si se define "h", se formateara a "-h".
- ``description`` : permite añadir una descripcion a cierta flag.
- ``number_arguments`` : permite definir la cantidad maxima de argumentos.
- ``required_arguments`` : permite indicar la cantidad minima de flags.
- ``name`` : permite poner un nombre a la flag.
- ``func_flag_exec`` :  subrutina/funcion que ejecutar si la flag es invocada.

----

## funciones auto-llamables por `init_argparse`

Las funciones atuo-llamables reciben una estructura del siguiente tipo:
```c
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
```

Donde el miembro '``table_args``' es una tabla de hash que se accede usando las flags cortas y largas como 'key'. El valor devuelto es un `ArrayList *` (array dinamico) donde cada elemento contenido es uno de los argumentos pasada a la flag. Cada elemento de este array es de tipo `Token_build_t*` donde se accede a 'x' dato usando el miembro `Array` del `ArrayList *`.

Los elemento de ``table_args`` se pueden acceder usando la funcion '``get (tabla_hash, clave)``', la funcion devuelve ``NULL`` en caso de que no se encuentre un elemento con esa clave.

Se recomiendo hacer lo siguienter si su compilador lo admite:

```c
ArrayList *arg_data = 
    get(
        data->table_args, 
        // obtener los valores del argumento '-s'
        flags[arg_val(suma)].short_flag
    ) 
    ?: 
    get (
        data->table_args, 
        // obtener los valores del argumento '--suma'
        flags[arg_val(suma)].long_flag 
    );
```

Aqui se usa un operador ternario `?:` para indicar que se use aquel valor que no sea ``NULL``. Aun asi puede darse el cado, de que ambas funciones retorne ``NULL``, lo cual seria un caso raro ya que indicaria que la flag no fue usada o haya ocurrido un error.

Una vez obtenido el `ArrayList` se puede obtener la cantidad de argumentos que la flag recibio, usando la funcion `size_a`.

Ejemplos:

```c
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
```
Aqui `flags[i]` es el array de flags que se definio en un inicio. `strtoll_plus` es una funcion que permite convertir un argumento en numero especificando una base o usando 'auto-detencion de base numerica', la cual usa '``strtoll``'.

----

Una vez se dfine toda la informacion, se puede proceder a iniciar el proceso de parseado de la linea de comandos.

En el ``main``, se debera definir que se recibe argumentos, junto a su numero (`int argc, char **argv`),
para posteriormente llamar a `init_argparse` con esos datos:

```c
int main(int argc, char **argv) {
    // ....
    argparse_t *arguments = init_argparse(argc, argv, flags, sizeof(flags) / sizeof(data_flag_t) );

    // ...
    free_argparse(&arguments);
    return 0;
}

```

----