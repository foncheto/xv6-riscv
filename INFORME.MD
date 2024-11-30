# INFORME 4 Alfonso Pinto

## Funcionamiento y lógica

Este proyecto implementa la funcionalidad de `chmod`, que permite modificar los permisos de un archivo dentro del sistema de archivos. Para validar su correcto funcionamiento, desarrollé el programa `chmod_test.c`, que evalúa escenarios comunes relacionados con la gestión de permisos.

1. **Comando de usuario (`chmod`)**:

    - Permite al usuario ejecutar `chmod <mode> <file>` para cambiar los permisos de un archivo.
    - Los permisos se almacenan como un valor entero en los metadatos del archivo (en la estructura `dinode`).

2. **Validación en `chmod_test.c`**:

    - Este programa prueba los cambios de permisos y su efecto en diferentes operaciones, incluyendo:
        - Creación de un archivo con permisos iniciales.
        - Restricciones al intentar escribir en un archivo con permisos de solo lectura.
        - Restauración de los permisos para confirmar que se aplican correctamente.
    - Las pruebas se realizan utilizando llamadas al sistema como `chmod` y `open`.

3. **Lógica del kernel**:

    - La llamada al sistema `sys_chmod` valida los argumentos proporcionados, bloquea el inode correspondiente, actualiza los permisos y sincroniza los cambios con el disco.

---

## Cambios realizados

### 1. **Estructura del sistema de archivos**

-   **`struct dinode`**: Se añadió un campo `perm` (int) para almacenar los permisos de cada archivo.
-   **Alineación del tamaño**: Para evitar errores de alineación en disco, se incluyó un campo de relleno (`padding`) que asegura que el tamaño del `dinode` sea divisible por el tamaño del bloque (`BSIZE`).

### 2. **Implementación de `sys_chmod`**

-   Se realizaron las siguientes modificaciones en `kernel/sysfile.c`:
    -   Validación de los argumentos de entrada (`path` y `mode`).
    -   Bloqueo del inode del archivo con `ilock` antes de modificar los permisos.
    -   Validación para impedir cambios en archivos marcados como inmutables.
    -   Sincronización de los cambios con `log_write` y liberación del inode.

### 3. **Validación de permisos en `sys_open`**

-   Se actualizó la función `sys_open` para verificar los permisos del archivo antes de permitir su apertura:
    -   Si el archivo tiene permisos de solo lectura (`perm == 1`), se bloquea cualquier intento de abrirlo en modo de escritura (`O_WRONLY`).

### 4. **Pruebas con `chmod_test.c`**

-   El programa prueba los permisos en las siguientes etapas:
    -   Creación de un archivo con permisos iniciales de lectura y escritura.
    -   Escritura inicial para confirmar que los permisos funcionan.
    -   Cambio de permisos a solo lectura (`chmod(filename, 1)`).
    -   Verificación de que no es posible abrir el archivo en modo escritura.
    -   Restauración de permisos de lectura y escritura (`chmod(filename, 3)`).
    -   Escritura final para confirmar que los permisos se restauraron correctamente.

Ejemplo del código actualizado:

```c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main() {
    const char *filename = "testfile";

    // Creación del archivo con permisos de lectura y escritura
    int fd = open(filename, O_CREATE | O_RDWR);
    if (fd < 0) {
        printf("Error: no se pudo crear el archivo.\n");
        exit(1);
    }
    printf("Archivo creado con éxito.\n");

    // Escritura inicial
    if (write(fd, "Test", 4) != 4) {
        printf("Error: no se pudo escribir en el archivo.\n");
        close(fd);
        exit(1);
    }
    printf("Escritura inicial completada.\n");
    close(fd);

    // Cambio de permisos a solo lectura
    if (chmod(filename, 1) < 0) {
        printf("Error: chmod a solo lectura falló.\n");
        exit(1);
    }
    printf("Permisos cambiados a solo lectura.\n");

    // Intento de abrir en modo escritura
    fd = open(filename, O_WRONLY);
    if (fd >= 0) {
        printf("Error: se pudo abrir en modo escritura cuando debería estar en solo lectura.\n");
        close(fd);
        exit(1);
    }
    printf("El archivo no puede abrirse en modo escritura.\n");

    // Restaurar permisos de lectura/escritura
    if (chmod(filename, 3) < 0) {
        printf("Error: chmod a lectura/escritura falló.\n");
        exit(1);
    }
    printf("Permisos restaurados a lectura/escritura.\n");

    // Escritura final
    fd = open(filename, O_RDWR);
    if (fd < 0) {
        printf("Error: no se pudo abrir el archivo en modo lectura/escritura.\n");
        exit(1);
    }
    if (write(fd, " Final Test", 11) != 11) {
        printf("Error: no se pudo escribir en el archivo.\n");
        close(fd);
        exit(1);
    }
    printf("Escritura final completada.\n");
    close(fd);

    exit(0);
}
```

### 5. **Modificaciones en el sistema de compilación**

-   Se actualizó el `Makefile` para incluir el nuevo comando `chmod` y el programa de pruebas `chmod_test.c`.

---

## Dificultades encontradas y soluciones implementadas

1. **Validación de permisos en `sys_open`**:

    - **Problema**: Los permisos de solo lectura no se aplicaban correctamente.
    - **Solución**: Se agregó validación explícita en `sys_open` para bloquear intentos de apertura en modo escritura.

2. **Errores de compilación en `sys_chmod`**:

    - **Problema**: Faltaban declaraciones en los encabezados.
    - **Solución**: Se ajustaron las declaraciones y definiciones en los archivos `syscall.h`, `fs.h` y otros.

3. **Alineación de tamaño en `dinode`**:

    - **Problema**: El tamaño del `dinode` dejó de ser divisible por el tamaño del bloque, causando errores de inicialización.
    - **Solución**: Se añadió un campo de relleno (`padding`) para asegurar la alineación.

4. **Depuración de permisos**:
    - **Problema**: No estaba claro si los permisos se aplicaban correctamente.
    - **Solución**: Se añadieron mensajes de depuración (`printf`) para rastrear los cambios en los permisos.
