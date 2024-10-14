#include "kernel/types.h"
#include "user/user.h"

int main(void)
{
    int i;
    for (i = 0; i < 20; i++)
    {
        if (fork() == 0)
        {
            printf("Ejecutando proceso %d\n", i);
            sleep(100); // Dormir por un corto tiempo
            exit(0);
        }
    }
    for (i = 0; i < 20; i++)
    {
        wait(0); // Esperar a que todos los procesos hijos terminen
    }
    exit(0);
}
