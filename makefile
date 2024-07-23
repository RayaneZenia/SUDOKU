# Le compilateur
CC = gcc

# Options de compilation
CFLAGS = -O0 

# Fichier source
SRC = sudoku.c

# Nom de l'exécutable
EXEC = sudoku

# Répertoire des bibliothèques
LIB_DIR = $(PWD)/lib


# Bibliothèques à lier
LIBS = -lsudoku -pthread

# Règle par défaut
all: $(EXEC)

# Compilation de l'exécutable
$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ -Wl,-rpath=$(LIB_DIR) -L$(LIB_DIR) $(LIBS)

# Nettoyage des fichiers générés
clean:
	$(RM) $(EXEC) *~
	

# Exécution du programme avec un argument spécifique
run-%: $(EXEC)
	./$(EXEC) $*
