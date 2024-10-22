#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void)
{
    int i;

    // Crear 20 procesos
    for (i = 0; i < 20; i++)
    {
        int pid = fork();
        if (pid == 0)
        {
            // Hacer que el proceso se detenga unos segundos antes de imprimir
            sleep(i * 10); // Diferente tiempo de espera para cada proceso

            // Este es el código que ejecutarán los procesos hijos.
            printf("Ejecutando proceso con PID: %d\n", getpid());

            exit(0); // Salir del proceso hijo
        }
    }

    // Esperar a que todos los procesos hijos terminen
    for (i = 0; i < 20; i++)
    {
        wait(0);
    }

    // Finalizar el programa de prueba
    exit(0);
}