````markdown
# INFORME TAREA 2 Alfonso Pinto Morales

## Objetivo

El objetivo de esta tarea es modificar el programador de procesos en **xv6** para implementar un sistema de prioridades que regule el orden de ejecución de los procesos, utilizando un campo de prioridad y un campo de boost para ajustar dinámicamente la prioridad de cada proceso.

## Modificaciones Realizadas

### Incorporar Prioridad

1. **Agregar un campo de prioridad**  
   Se añadió un campo `priority` a la estructura de proceso (`proc`) en `proc.h` para manejar la prioridad de cada proceso.

2. **Inicializar prioridad en 0**  
   En la función `allocproc()` en `proc.c`, la prioridad de los nuevos procesos se inicializa en 0, indicando que el número más bajo representa una mayor prioridad.

```c
p->priority = 0;
```
````

### Incorporar Boost

1. **Agregar un campo de boost**  
   Se añadió un campo `boost` a la estructura de proceso en `proc.h` para controlar cómo cambia la prioridad con el tiempo.

2. **Inicializar boost en 1**  
   En `allocproc()`, se inicializa el campo `boost` en 1, lo que permite aumentar la prioridad de los procesos con el tiempo.

```c
p->boost = 1;
```

### Modificación del Scheduler

El `scheduler()` fue modificado para actualizar la prioridad de cada proceso en cada iteración del ciclo. Si la prioridad de un proceso llega a 9, el boost se invierte y comienza a disminuir la prioridad. Si baja a 0, el boost vuelve a ser positivo, aumentando la prioridad.

```c
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();

  c->proc = 0;
  for(;;){
    intr_on();  // Habilitar interrupciones para evitar deadlocks

    int found = 0;
    for(p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      if(p->state == RUNNABLE) {

        // Lógica de prioridad y boost
        p->priority += p->boost;

        if(p->priority >= 9) {
          p->boost = -1;
        } else if(p->priority <= 0) {
          p->boost = 1;
        }

        // Cambiar al proceso seleccionado
        p->state = RUNNING;
        c->proc = p;
        swtch(&c->context, &p->context);

        c->proc = 0;
        found = 1;
      }
      release(&p->lock);
    }

    if(found == 0) {
      // No hay nada para ejecutar; esperar una interrupción.
      intr_on();
      asm volatile("wfi");
    }
  }
}
```

### Programa de Prueba

Se creó el programa de prueba **`test_priority.c`** que genera 20 procesos. Cada proceso imprime su PID, con un `sleep()` proporcional al índice del proceso para evitar que la salida de los mensajes se mezcle.

```c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int i;

  // Crear 20 procesos
  for(i = 0; i < 20; i++) {
    int pid = fork();
    if(pid == 0) {
      // Detenerse unos segundos antes de imprimir
      sleep(i * 10);

      // Código ejecutado por los procesos hijos
      printf("Ejecutando proceso con PID: %d\n", getpid());

      exit(0);
    }
  }

  // Esperar a que todos los procesos hijos terminen
  for(i = 0; i < 20; i++) {
    wait(0);
  }

  exit(0);
}
```

### Modificación del Makefile

Para asegurar que el nuevo programa de prueba se compile correctamente, se añadió **`test_priority`** en el `Makefile` dentro del bloque `UPROGS`:

```makefile
UPROGS= \
    ...
    test_priority \
    ...
```

## Desafíos Encontrados y Soluciones

1. **Salida desordenada de procesos**:  
   Los mensajes impresos por los procesos se mezclaban en la consola.

    - **Solución**: Usé la función `sleep()` para que los procesos imprimieran en diferentes momentos, evitando que los mensajes se mezclen.

2. **Error de compilación ("No rule to make target")**:  
   Al agregar el programa de prueba al `Makefile`, se produjo un error debido a una barra invertida mal colocada.

    - **Solución**: Se corrigió la barra invertida y se verificó la correcta alineación de las líneas en el `Makefile`.

## Consejos

-   Revisar el código del programador de procesos existente para entender cómo se gestionan los procesos.
-   Documentar adecuadamente todas las modificaciones realizadas para mantener el código claro y comprensible.
-   Probar los cambios con el programa de prueba para asegurar que no afecten negativamente el funcionamiento general del sistema.

## Instrucciones de Entrega

1. Crear una nueva rama en tu fork de xv6 para esta tarea.
2. Realizar los cambios y commits en esta nueva rama.
3. Subir la rama a tu repositorio en GitHub.
4. Proporcionar en el buzón de WebC el enlace a tu rama en GitHub:

    ```plaintext
    https://github.com/tu_usuario/xv6-riscv/tree/nombre_apellido_t2
    ```

## Detalle de la Entrega

-   **Código fuente modificado**: Incluir los cambios en el sistema de prioridades y el scheduler.
-   **Programa de prueba**: `test_priority.c`.
-   **Informe README**: Incluir el funcionamiento y lógica del sistema de prioridades, las modificaciones realizadas, dificultades encontradas, y soluciones implementadas.

```

```
