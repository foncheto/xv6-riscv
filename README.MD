# INFORME TAREA 2: Sistema de Prioridades en xv6

**Alumno:** Alfonso Pinto Morales

---

## 1. Paso a Paso

### 1.1. Estructura de la Prioridad y Boost

Para gestionar la ejecución de procesos según su prioridad, se añadieron dos nuevos campos en la estructura `proc` dentro del archivo `proc.h`:

```c
struct proc {
    ...
    int priority;  // Nivel de prioridad del proceso
    int boost;     // Dirección de ajuste de la prioridad (positivo o negativo)
    ...
};
```

#### Inicialización de la prioridad y boost

En la función `allocproc()` del archivo `proc.c`, se inicializan estos campos con valores predeterminados:

```c
p->priority = 0;  // Prioridad inicial en 0 (menor prioridad)
p->boost = 1;     // El boost inicial es positivo
```

Este sistema asigna a cada proceso un nivel de prioridad ajustable, que se incrementa o disminuye automáticamente según el valor de `boost`.

---

### 1.2. Modificación del `Scheduler`

Se implementaron cambios en la función `scheduler()` para que las prioridades de los procesos se actualicen dinámicamente en cada ciclo de ejecución. A continuación, se muestra la lógica de este ajuste:

```c
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;

  for(;;){
    intr_on();  // Activar interrupciones para evitar deadlocks
    int found = 0;

    for(p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      if(p->state == RUNNABLE) {
        // Actualizar la prioridad en función del boost
        p->priority += p->boost;

        // Cambiar la dirección del boost cuando se alcanza un límite
        if(p->priority >= 9) {
          p->boost = -1;  // Reducir prioridad
        } else if(p->priority <= 0) {
          p->boost = 1;   // Aumentar prioridad
        }

        // Ejecutar el proceso seleccionado
        p->state = RUNNING;
        c->proc = p;
        swtch(&c->context, &p->context);
        c->proc = 0;
        found = 1;
      }
      release(&p->lock);
    }

    if(!found) {
      // No hay procesos RUNNABLE; esperar una interrupción
      intr_on();
      asm volatile("wfi");
    }
  }
}
```

Esta lógica permite que los procesos más "favorecidos" tengan su prioridad ajustada para evitar monopolizar la CPU. Cuando la prioridad alcanza su valor máximo (9), el boost se invierte, y la prioridad comienza a decrecer hasta llegar a 0, donde el boost vuelve a ser positivo.

---

### 1.3. Programa de Prueba

Se creó un programa llamado `test_priority.c` que genera 20 procesos hijos. Cada uno imprime su PID tras un breve `sleep()` calculado en función de su índice para evitar que los mensajes se mezclen:

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
      sleep(i * 10);  // Control del tiempo de impresión
      printf("Proceso con PID: %d ejecutado\n", getpid());
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

---

### 1.4. Modificación del Makefile

Para integrar el programa de prueba `test_priority.c`, se añadió la siguiente línea al bloque `UPROGS` del `Makefile`:

```makefile
UPROGS= \
    ...
    test_priority \
    ...
```

---

## 2. Desafíos Encontrados y Soluciones

### 2.1. Problema de Salida Desordenada

**Desafío:** Los mensajes impresos por los procesos se mezclaban en la consola, lo que hacía difícil identificar qué proceso estaba imprimiendo.

**Solución:** Se implementó un `sleep()` en los procesos hijos para distribuir la ejecución a lo largo del tiempo, evitando conflictos de salida en la consola.

### 2.2. Error de Compilación en el Makefile

**Desafío:** Durante la compilación del nuevo programa de prueba, se produjo un error de tipo `No rule to make target`, causado por una barra invertida malformada en el `Makefile`.

**Solución:** Se corrigió la barra invertida y se verificó la correcta alineación del archivo para asegurar que todos los programas se compilaran sin problemas.
