# Mini-Shell en C

## Integrantes:

- Angelo Reyes
- Benjamin Uribe
- Max Coñoman

## 1. Descripcion

Mini-Shell es un programa desarrollado en C que simula el funcionamiento básico de una terminal de Linux. Su objetivo es recibir comandos del usuario, ejecutar programas externos y administrar los procesos que se generan, tanto en primer como en segundo plano.

El proyecto utiliza `fork()` para crear procesos hijos, `execvp()` para ejecutar programas y `waitpid()` para gestionar su finalización. Tambien entrega informacion sobre la memoria del sistema y un resumen al cerrar la aplicacion.

## 2. Requisitos

- **Linux**, ya que el programa utiliza funciones POSIX y consulta `/proc/meminfo`.
- **GCC**, para compilar el codigo fuente en C.

## 3. Compilación y ejecución

Desde una terminal, ubícate en la carpeta que contenga `shell.c` y ejecuta:

```bash
gcc shell.c -o minishell
```

Para iniciar la mini-shell:

```bash
./minishell
```

Aparecera el prompt:

```text
minish >
```

Escribe un comando y presiona Enter. Para terminar la sesión puedes utilizar `exit` o Ctrl + D.

De manera opcional, puedes activar las advertencias del compilador:

```bash
gcc -Wall -Wextra -Wpedantic shell.c -o minishell
```

## 4. Funcionalidades

### Comandos internos

Estos comandos son procesados directamente por la mini-shell, sin ejecutar un programa externo:

| Comando | Descripción |
| --- | --- |
| `help` | Muestra los comandos disponibles y la forma básica de uso. |
| `cd [dir]` | Cambia de directorio. Si no se indica una ruta, utiliza el directorio `HOME`. |
| `exit` | Espera a los procesos hijos pendientes, muestra un resumen y cierra la mini-shell. |

### Programas externos

Se pueden ejecutar programas instalados en el sistema junto con sus argumentos, por ejemplo:

```text
minish > ls -l
minish > pwd
minish > echo Hola
```

Para ejecutarlos, la mini-shell crea un proceso hijo mediante `fork()` y utiliza `execvp()` para cargar el programa solicitado. 
**No delega automaticamente la interpretacion de los comandos a Bash u otra shell.**

### Primer y segundo plano

Por defecto, los programas se ejecutan en **primer plano**, y la mini-shell espera a que terminen antes de mostrar un nuevo prompt.

```text
minish > sleep 3
```

Para ejecutar un programa en **segundo plano**, se agrega `&` como ultimo argumento, separado por un espacio:

```text
minish > sleep 3 &
```

En este caso, la mini-shell muestra el PID del proceso y permite ingresar otros comandos mientras continua ejecutandose.

### Memoria y resumen final

Cuando se recoge un proceso hijo terminado, se muestra la **memoria disponible del sistema** consultando el valor `MemAvailable` de `/proc/meminfo`.

Al salir se presenta un resumen con:

- Numero de procesos externos creados durante la sesion.
- Memoria residente maxima registrada para los hijos recolectados.
- Memoria residente maxima de la propia mini-shell.

Los valores maximos se obtienen con `getrusage()`. No representan la suma de memoria utilizada por todos los procesos ni la memoria individual de cada hijo.

### Manejo de procesos

- Se comprueba el resultado de `fork()` para detectar errores al crear hijos.
- `waitpid()` espera a los procesos en primer plano.
- La señal `SIGCHLD` y `waitpid()` con `WNOHANG` permiten detectar y recolectar hijos que terminan en segundo plano, para reducir la aparicion de procesos zombis.
- Al usar `exit` o Ctrl+D, la mini-shell espera a los hijos pendientes antes de cerrar y mostrar el resumen. Esto significa que puede tardar en salir si algún proceso sigue ejecutandose.

## 5. Limitaciones

La mini-shell implementa las funciones necesarias para el proyecto, pero **no reemplaza a Bash** ni incluye todas las características de una terminal completa:

- No interpreta **tuberias** (`|`) ni **redirecciones** (`>`, `<`, `>>`).
- No cuenta con el comando `jobs` ni con control avanzado de trabajos, como `fg` o `bg`.
- No implementa historial de comandos, autocompletado ni expansion de variables o sustitucion de comandos.
- Los argumentos se separan por espacios, por lo que no se interpretan comillas ni caracteres especiales como lo haria Bash.
- Para ejecutar en segundo plano se debe escribir `&` separado al final del comando, por ejemplo `sleep 3 &`.
- La entrada tiene un tamaño maximo definido por `MAX_INPUT` (1024 bytes de bufer) y los argumentos por `MAX_ARG` (64 posiciones, incluida la posición final `NULL`). Las líneas que superan el bufer no se gestionan como una sola entrada completa.
- La recolección de hijos terminados no ofrece todas las garantías de una shell con control completo de procesos: puede existir un breve intervalo en el que un hijo terminado permanezca como zombi hasta la siguiente recoleccion.
