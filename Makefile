# Nom de l'exécutable
EXEC = puissance_n

# Répertoires
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INCLUDE_DIR = include

# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -I$(INCLUDE_DIR)

# Sources et objets
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Cible par défaut
all: $(BIN_DIR)/$(EXEC)

# Règle pour créer l'exécutable
$(BIN_DIR)/$(EXEC): $(OBJS)
    @mkdir -p $(BIN_DIR)
    $(CC) $(CFLAGS) -o $@ $^

# Règle pour compiler les fichiers objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
    @mkdir -p $(OBJ_DIR)
    $(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des objets
clean:
    rm -rf $(OBJ_DIR)

# Nettoyage complet
fclean: clean
    rm -rf $(BIN_DIR)

# Recompilation complète
re: fclean all

.PHONY: all clean fclean re