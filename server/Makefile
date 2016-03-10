# Ces cibles ne sont pas des vrais fichiers
.PHONY: clean

# On désactive toutes les règles implicites

# Déclarations de constantes
CC = gcc
CFLAGS = -Wall -Werror -W 
SRC = $(wildcard *.c)
OBJS = $(SRC:.c=.o)


server: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ `pkg-config --cflags --libs cairo x11`

$(OBJS): $(SRC)
	$(CC) $(CFLAGS) -c $^ `pkg-config --cflags --libs cairo x11`

makefile.dep: $(SRC)
	gcc -MM $^ > $@

clean:
	rm -f *.o server

include makefile.dep
