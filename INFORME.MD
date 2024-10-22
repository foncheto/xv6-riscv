Informe tarea 2  
Modificaciones realizadas

1. Estructura de la prioridad y boost  
   Para implementar el sistema de prioridades, añadí dos nuevos campos a la estructura `proc` en el archivo `proc.h`:

```c
struct proc {
    ...
    int priority;  // Campo de prioridad
    int boost;     // Campo de boost
    ...
};
```

Luego, inicialicé estos campos en la función `allocproc()` en el archivo `proc.c`:

```c
p->priority = 0;  // La prioridad comienza en 0
p->boost = 1;     // El boost comienza en 1
```

2. Modificación del scheduler  
   El `scheduler` fue modificado para actualizar la prioridad de cada proceso en cada ciclo. Se implementó una lógica que ajusta la prioridad dependiendo del valor del boost.

El código modificado en la función `scheduler()` es:

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

Este código modifica la prioridad de cada proceso según su boost. Si la prioridad alcanza 9, el boost se reduce a -1, y si baja a 0, el boost vuelve a 1.

3. Programa de prueba  
   Desarrollé un programa de prueba (`test_priority.c`) que crea 20 procesos, los cuales imprimen su PID cuando son ejecutados. Para evitar que la salida se mezcle, utilicé la función `sleep()` en los procesos, con un tiempo de espera proporcional al índice de cada proceso:

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

4. Modificación del Makefile  
   Agregué el nuevo programa de prueba `test_priority.c` al Makefile para asegurar que se compile correctamente. El cambio fue agregar la siguiente línea:

```makefile
_# Incluir test_priority.c en el Makefile_
UPROGS= \
    ...
    test_priority \
    ...
```

Dificultades encontradas y soluciones

-   **Problemas con la salida desordenada**: Los mensajes impresos por los procesos se mezclaban en la consola.  
    **Solución**: Usé `sleep()` para que los procesos imprimieran en momentos diferentes.

-   **Error de compilación (No rule to make target)**: Un error al agregar el nuevo programa de prueba al Makefile fue causado por una barra invertida mal colocada.  
    **Solución**: Corregí la barra y alineé correctamente las líneas en el Makefile, resolviendo el problema.
