# makefile Jukebox

CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -g -std=c99
LDLIBS  = -lm
MAIN    = main
ENTREGA = $(MAIN)

# lista de arquivos de cabeçalho (a completar)
HDR = jukefila.h pedido.h

# lista de arquivos-objeto (a completar)

OBJ = jukefila.o pedido.o main.o

# construir o executável
$(MAIN): $(MAIN).o $(OBJ) 

# construir os arquivos-objeto (a completar)
$(MAIN).o: $(MAIN).c $(HDR)

# construir os TADs
jukefila.o = jukefila.c jukefila.h
pedido.o = pedido.c pedido.h 

# executar
run: $(MAIN)
	./$(MAIN)

# testar no Valgrind
valgrind: $(MAIN)
	valgrind --leak-check=full --show-leak-kinds=all -s --track-origins=yes ./$(MAIN)

# gerar arquivo TGZ para entregar
tgz: clean
	-mkdir -p /tmp/$(USER)/$(ENTREGA)
	chmod 0700 /tmp/$(USER)/$(ENTREGA)
	cp *.c *.h makefile /tmp/$(USER)/$(ENTREGA)
	tar czvf $(ENTREGA).tgz -C /tmp/$(USER) $(ENTREGA)
	rm -rf /tmp/$(USER)
	@echo "Arquivo $(ENTREGA).tgz criado para entrega"

# limpar arquivos temporários
clean:
	rm -f *~ $(OBJ) $(MAIN) /tmp/$(USER)/$(ENTREGA) $(ENTREGA).tgz
