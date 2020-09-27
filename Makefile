# Compilador a usarse
CC=msp430-gcc
# Banderas para compilacion
CFLAGS= -Wall -g -mmcu=msp430g2553
# Archivo objeto
OBJS=WlchrCtrl.o

# Reglas para compilacion
all: $(OBJS)
	$(CC) $(CFLAGS) -o WlchrCtrl.elf $(OBJS)

# Esta es una regla implicita que dice como compilar todos los
# archivos con extension *.c para obtener archivos *.o
# el valor de $< se toma del lado derecho de los dos puntos
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Borra los archivos *.o y *.elf
clean:
	rm -fr WlchrCtrl.elf $(OBJS)
