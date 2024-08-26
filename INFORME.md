**Tarea 0 Alfonso Pinto Morales**

En esta primera tarea, realicé la instalación de las herramientas necesarias para ejecutar xv6 en mi computador Mac. A continuación, se describen los pasos seguidos:

### 1. Instalación de Homebrew

Homebrew es un gestor de paquetes para macOS que facilita la instalación de software desde la línea de comandos. Para instalar Homebrew, utilicé el siguiente comando en la Terminal:

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### 2. Instalación de QEMU y el compilador RISC-V

QEMU es un emulador que permite ejecutar el kernel de xv6 en una máquina virtual. Para instalar estas herramientas, ejecuté el siguiente comando:

```bash
brew install qemu gcc-riscv64-embedded
```

### 3. Instalación del Toolchain de RISC-V

Dado que xv6 requiere un compilador específico para la arquitectura RISC-V, instalé el toolchain de RISC-V utilizando Homebrew. Para ello, añadí el repositorio correspondiente y luego instalé las herramientas necesarias:

```bash
brew tap riscv/riscv
brew install riscv-tools
```

### 4. Clonación y configuración del repositorio de xv6

Con las herramientas instaladas, procedí a hacer un fork del repositorio oficial de xv6 en GitHub, y luego cloné mi fork en la computadora con el siguiente comando:

```bash
git clone https://github.com/foncheto/xv6-riscv.git
cd xv6-riscv
```

### 4A Creación rama del repositorio

Para trabajar en la tarea 0, creé una rama llamada alfonso_pinto_t0 con el siguiente comando:

```bash
git checkout -b alfonso_pinto_t0
```

### 5. Compilación del kernel de xv6

Desde la carpeta del repositorio clonado, ejecuté los siguientes comandos para compilar el kernel de xv6 y correrlo en QEMU:

```bash
make
make qemu
```

### 6. Ejecución de pruebas

Una vez en la consola de xv6, ejecuté algunos comandos básicos para verificar que todo funcionaba correctamente:

```bash
ls
echo "Hola xv6"
cat README
```

### 7. Conclusión

Después de realizar las pruebas mencionadas, confirmé que xv6 se instaló y funciona correctamente en mi MacBook.
Se guardan los cambios en la rama creada y se suben al repositorio remoto con los siguientes comandos:

```bash
git add .
git commit -m "Create INFORME.md"
git push origin alfonso_pinto_t0
```
