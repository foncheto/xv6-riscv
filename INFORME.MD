# INFORME TAREA 2 Alfonso Pinto Morales

## Modificaciones Realizadas

1. **Estructura de la prioridad y boost**  
   Para implementar el sistema de prioridades, se añadieron dos nuevos campos a la estructura `proc` en el archivo `proc.h`:

```c
struct proc {
    ...
    int priority;  // Campo de prioridad
    int boost;     // Campo de boost
    ...
};
```

En la función `allocproc()` en `proc.c`, se inicializaron estos campos:

```c
p->priority = 0;  // La prioridad comienza en 0
p->boost = 1;     // El boost comienza en 1
```

2. **Modificación del scheduler**  
   El `scheduler` fue modificado para actualizar la prioridad de cada proceso en cada ciclo. La prioridad cambia según el valor del boost. Aquí está el código modificado en la función `scheduler()`:

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

Esta lógica ajusta la prioridad de los procesos: cuando la prioridad alcanza 9, el boost se reduce a -1, y cuando baja a 0, el boost vuelve a 1.

3. **Programa de Prueba**  
   Se creó un programa de prueba (`test_priority.c`) que genera 20 procesos. Cada proceso imprime su PID, con un `sleep()` proporcional al índice del proceso para evitar que los mensajes se mezclen en la consola:

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

4. **Modificación del Makefile**  
   Se actualizó el `Makefile` para compilar el programa de prueba `test_priority.c`. Para ello, se añadió la siguiente línea en el bloque de `UPROGS`:

```makefile
UPROGS= \
    ...
    test_priority \
    ...
```

## Desafíos Encontrados y Soluciones

1. **Problemas de salida desordenada**: Los mensajes impresos por los procesos se mezclaban en la consola, lo que dificultaba saber qué proceso estaba imprimiendo en cada momento.

    - **Solución**: Se utilizó la función `sleep()` en los procesos hijos para que imprimieran en diferentes intervalos de tiempo.

2. **Error de compilación (`No rule to make target`)**: Hubo un error debido a una barra invertida malformada en el `Makefile` al intentar compilar el nuevo programa de prueba.

    - **Solución**: Se corrigió la barra invertida y se verificó la correcta alineación de las líneas en el `Makefile`, lo que resolvió el problema.

```

```
