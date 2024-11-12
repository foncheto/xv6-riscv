# INFORME TAREA 3: Protección de Memoria en xv6

### Alfonso Pinto Morales

---

## Funcionamiento de la Protección de Memoria

### Llamadas al Sistema Implementadas

1. **`mprotect(void *addr, int len)`**:  
   Esta llamada al sistema permite proteger un rango de páginas de memoria como solo lectura. La función recibe una dirección inicial (`addr`) y un número de páginas (`len`) a proteger. Modifica los bits de permisos en las entradas de la tabla de páginas (PTE) de cada página afectada, deshabilitando el bit de escritura (`PTE_W`) para marcar la memoria como solo lectura.

2. **`munprotect(void *addr, int len)`**:  
   Esta llamada revierte la protección establecida por `mprotect`, permitiendo nuevamente la escritura en las páginas afectadas. Al igual que `mprotect`, utiliza `addr` y `len` para determinar el rango de páginas, y restablece el bit de escritura (`PTE_W`) en las PTEs para habilitar la escritura.

Ambas funciones modifican directamente los permisos de la tabla de páginas, permitiendo a xv6 controlar el acceso a memoria y generando fallos de protección si el proceso intenta escribir en una página protegida.

---

## Implementación Detallada

### 1. Definición de las Llamadas al Sistema

-   **Asignación de números de syscall**: Se añadieron las definiciones de `SYS_mprotect` y `SYS_munprotect` en `kernel/syscall.h` y se les asignaron los números 22 y 23 respectivamente.
-   Esto permite que xv6 reconozca estas llamadas como operaciones válidas y asigna una referencia única a cada una.

### 2. Implementación de las Funciones del Núcleo

-   **Funciones de núcleo (`sys_mprotect` y `sys_munprotect`)**:  
    Se implementaron en `kernel/sysproc.c` y actúan como intermediarios entre la llamada del usuario y la lógica de protección de memoria. Estas funciones validan los argumentos y llaman a las funciones `mprotect` y `munprotect` del núcleo para realizar las modificaciones en la tabla de páginas.
-   **Función `validate_addr_len`**:  
    Esta función auxiliar verifica que la dirección (`addr`) esté alineada en límites de página y que `len` sea positivo, asegurando que el rango especificado es válido para el proceso.

### 3. Modificaciones en Archivos del Sistema

-   **Inclusión en `user/user.h`**:  
    Se añadieron las declaraciones de las nuevas llamadas al sistema para que los programas de usuario puedan acceder a `mprotect` y `munprotect`.
-   **Modificación de `kernel/syscall.c`**:  
    Se incluyeron las referencias a `sys_mprotect` y `sys_munprotect` en la tabla de manejadores de syscalls, permitiendo que xv6 llame a las funciones correspondientes cuando se ejecuten desde un programa de usuario.
-   **Actualización de `usys.pl`**:  
    Este script se modificó para que genere el código de ensamblador necesario para las nuevas syscalls, facilitando el acceso desde el espacio de usuario.
-   **Makefile**:  
    Se añadió el programa de prueba (`memtest`) al Makefile para que se compile automáticamente junto con el sistema.

### 4. Programa de Prueba (`memtest`)

-   Se implementó el programa `memtest.c` para verificar que la protección de memoria funciona como se espera.
-   El programa reserva memoria, la marca como solo lectura y verifica que los intentos de escritura en la memoria protegida fallan. Después, usa `munprotect` para restaurar los permisos de escritura y prueba que la escritura es nuevamente permitida.

---

## Lógica de la Protección de Memoria

La protección de memoria se implementa manipulando directamente los bits de permisos en las PTEs de la tabla de páginas:

### 1. Validación de Argumentos

-   **Alineación**: Se verifica que la dirección `addr` esté alineada en un límite de página, ya que la protección opera a nivel de páginas.
-   **Rango**: Se asegura que `len` sea positivo y que el rango de direcciones pertenece al proceso actual.
-   **Acceso Seguro**: Al validar estos elementos, prevenimos accesos inseguros y limitamos la protección solo a las áreas de memoria del proceso que la solicita.

### 2. Modificación de Permisos

-   **Protección (mprotect)**:  
    Para cada PTE de la página, se desactiva el bit `PTE_W`, eliminando los permisos de escritura y marcando la página como solo lectura. Esto asegura que cualquier intento de escribir en esta página genere un fallo de protección.
-   **Desprotección (munprotect)**:  
    Se restaura el bit `PTE_W` en cada PTE afectada, permitiendo nuevamente la escritura.
-   **sfence_vma()**:  
    Tras modificar los permisos, se utiliza `sfence_vma()` para asegurar que los cambios en la tabla de páginas tengan efecto inmediato al limpiar las entradas en la TLB.

### 3. Manejo de Errores

-   **Alineación Incorrecta**: Si `addr` no está alineada a una página, se retorna un error.
-   **Rango Inválido**: Si el rango no es positivo o no pertenece al proceso, se retorna un error.
-   **Permisos**: Verificación de permisos antes de intentar modificar las PTEs.
-   **Retorno de Errores**: Las funciones devuelven `-1` si ocurre un error en cualquiera de los pasos de validación o modificación de permisos.

---

## Desafíos Encontrados y Soluciones

1. **Manejo de Argumentos de Syscall**

    - **Problema**: Inicialmente se presentaron errores al utilizar `argaddr` y `argint` debido a una incorrecta manipulación de tipos.
    - **Solución**: Se implementaron correctamente las funciones auxiliares para obtener argumentos de la syscall, cambiando los tipos de `void*` a `uint64` en la definición de las funciones del sistema, lo cual resolvió los errores de tipo.

2. **Declaración de Funciones**

    - **Problema**: Se encontraron errores de declaración implícita en el proceso de compilación.
    - **Solución**: Se añadieron prototipos de las funciones en `defs.h` y se reorganizó el código para asegurar que todas las referencias fueran correctas.

3. **Validación de Rango de Memoria**

    - **Problema**: Fue necesario asegurar que el rango de memoria especificado estuviera correctamente alineado y dentro de los límites del proceso.
    - **Solución**: Se implementó `validate_addr_len` para realizar estas validaciones, simplificando las funciones principales `mprotect` y `munprotect`.

4. **Referencias Indefinidas**

    - **Problema**: Algunas referencias no estaban correctamente enlazadas en la compilación.
    - **Solución**: Se revisaron y actualizaron todos los archivos del sistema afectados, incluyendo el Makefile, para asegurar la compilación sin errores.

---

## Evidencia de Funcionamiento

Se adjunta un pantallazo en la carpeta `Pruebas` con la ejecución del programa de prueba `memtest` en xv6, el cual muestra:

1. La memoria se puede reservar correctamente mediante `sbrk`.
2. La protección `mprotect` establece la memoria como solo lectura.
3. Los intentos de escritura en la memoria protegida fallan como se espera.
4. `munprotect` permite revertir la protección y los intentos de escritura son exitosos tras la desprotección.

Este programa de prueba valida que las llamadas al sistema `mprotect` y `munprotect` funcionan correctamente para proteger y desproteger regiones de memoria en xv6, cumpliendo con los objetivos de la tarea.
