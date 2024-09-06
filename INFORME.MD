# INFORME TAREA 1 Alfonso Pinto Morales

## Funcionamiento de las Llamadas al Sistema

1. **`getppid()`**: Esta llamada al sistema devuelve el ID de proceso del padre (PPID) del proceso que la invoca. Es útil para que un proceso conozca quién lo ha creado, lo cual es relevante en tareas de depuración y monitoreo de procesos.

2. **`getancestor(int n)`**: Esta llamada al sistema devuelve el ID de proceso de un ancestro `n` del proceso que la invoca. Por ejemplo, `getancestor(0)` devuelve el propio proceso, `getancestor(1)` devuelve el proceso padre, y así sucesivamente. Si `n` es mayor que el número de ancestros disponibles, la función devuelve `-1`.

### Implementación de `getppid()`

1. **Definición de la Llamada al Sistema:**

    - Se añadió la definición de `SYS_getppid` en el archivo `kernel/syscall.h`.

2. **Asignación del Número de Llamada al Sistema:**

    - Se añadió la entrada `[SYS_getppid] sys_getppid` en el archivo `kernel/syscall.c` para asociar el número de llamada al sistema con su función correspondiente.

3. **Implementación de la Función del Núcleo:**

    - La función `sys_getppid()` fue implementada en `kernel/sysproc.c` para obtener y devolver el ID del proceso padre del proceso actual (`myproc()->parent->pid`).

4. **Modificación del Archivo `user.h`:**

    - Se añadió la declaración `int getppid(void);` en el archivo `user/user.h` para que los programas de usuario puedan invocar la nueva llamada al sistema.

5. **Modificación del Archivo `usys.S`:**

    - Se añadió la entrada `SYSCALL(getppid)` en el archivo `user/usys.S`.

6. **Creación del Programa de Prueba `yosoytupadre.c`:**

    - Se añadió un programa en `user/yosoytupadre.c` para probar la nueva llamada al sistema. Este programa simplemente invoca `getppid()` y muestra el resultado.

7. **Modificación del Makefile:**
    - Se actualizó el `Makefile` para compilar `yosoytupadre` y asegurarse de que se incluya en la construcción de xv6.

### Implementación de `getancestor(int n)`

1. **Definición de la Llamada al Sistema:**

    - Se añadió la definición de `SYS_getancestor` en el archivo `kernel/syscall.h`.

2. **Asignación del Número de Llamada al Sistema:**

    - Se añadió la entrada `[SYS_getancestor] sys_getancestor` en el archivo `kernel/syscall.c` para asociar el número de llamada al sistema con su función correspondiente.

3. **Implementación de la Función del Núcleo:**

    - La función `sys_getancestor()` fue implementada en `kernel/sysproc.c` para recorrer la jerarquía de procesos ascendentes hasta encontrar el ancestro `n`-ésimo. Si no existe tal ancestro, la función retorna `-1`.

4. **Modificación del Archivo `user.h`:**

    - Se añadió la declaración `int getancestor(int n);` en el archivo `user/user.h` para que los programas de usuario puedan invocar la nueva llamada al sistema.

5. **Modificación del Archivo `usys.S`:**

    - Se añadió la entrada `SYSCALL(getancestor)` en el archivo `user/usys.S`.

6. **Modificación del Programa de Prueba `yosoytupadre.c`:**

    - Se modificó el programa en `user/yosoytupadre.c` para probar la llamada `getancestor()`. Este programa invoca `getancestor(n)` y muestra el resultado para diferentes valores de `n`.

7. **Modificación del Makefile:**
    - Se actualizó el `Makefile` para compilar la nueva versión de `yosoytupadre` con soporte para ambas llamadas al sistema (en UPROGS).

## Desafíos Encontrados, Cómo Se Resolvieron y Aprendizajes

1. **Errores de Tipos en las Funciones:**

    - Inicialmente se encontraron errores al declarar las funciones con tipos incorrectos en `syscall.c`. Se resolvió cambiando las funciones para que retornaran el tipo `uint64` en lugar de `int`.

2. **Problemas con la Declaración y Definición de las Funciones:**

    - Hubo errores relacionados con la no declaración de funciones en `syscall.c` y `user.h`. Se resolvieron asegurando que cada nueva llamada al sistema tuviera tanto una declaración en `user/user.h` como una definición en `kernel/sysproc.c`.

3. **Actualización del Makefile:**

    - Se resolvieron errores al agregar nuevos programas de usuario, asegurando que el `Makefile` estuviera correctamente configurado para incluir los nuevos archivos de prueba.
