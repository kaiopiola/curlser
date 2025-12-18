# Makefile for curlser
# Supports Linux, macOS and Windows (MinGW)

# Detect operating system
UNAME_S := $(shell uname -s 2>/dev/null || echo Windows)

# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
LDFLAGS = -lcurl

# Diretórios
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Arquivos fonte
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/http.c \
          $(SRC_DIR)/formatters/formatters.c \
          $(SRC_DIR)/formatters/json.c \
          $(SRC_DIR)/formatters/xml.c \
          $(SRC_DIR)/formatters/html.c

# Objetos
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Nome do executavel
ifeq ($(UNAME_S),Windows)
    TARGET = $(BIN_DIR)/curlser.exe
else
    TARGET = $(BIN_DIR)/curlser
endif

# Flags específicas por plataforma
ifeq ($(UNAME_S),Darwin)
    # macOS - pode precisar de paths do Homebrew
    CFLAGS += -I/opt/homebrew/include -I/usr/local/include
    LDFLAGS += -L/opt/homebrew/lib -L/usr/local/lib
endif

ifeq ($(UNAME_S),Linux)
    # Linux
    CFLAGS += -D_GNU_SOURCE
endif

ifeq ($(UNAME_S),Windows)
    # Windows com MinGW
    LDFLAGS += -lws2_32
endif

# Regra padrão
all: dirs $(TARGET)

# Cria diretórios
dirs:
	@mkdir -p $(BUILD_DIR)/formatters
	@mkdir -p $(BIN_DIR)

# Compila executavel
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build concluido: $@"

# Compila objetos
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpa build
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Instala (Linux/macOS)
install: $(TARGET)
ifeq ($(UNAME_S),Windows)
	@echo "Para Windows, copie $(TARGET) para um diretorio no PATH"
else
	install -m 755 $(TARGET) /usr/local/bin/curlser
	@echo "Instalado em /usr/local/bin/curlser"
endif

# Desinstala
uninstall:
ifeq ($(UNAME_S),Windows)
	@echo "Remova manualmente curlser.exe do PATH"
else
	rm -f /usr/local/bin/curlser
endif

# Testa
test: $(TARGET)
	@echo "Testando com httpbin.org..."
	@echo ""
	@echo "=== Teste JSON ==="
	$(TARGET) https://httpbin.org/json
	@echo ""
	@echo "=== Teste Headers ==="
	$(TARGET) -i https://httpbin.org/headers
	@echo ""
	@echo "=== Teste POST ==="
	$(TARGET) -X POST -H "Content-Type: application/json" -d '{"test": "data"}' https://httpbin.org/post

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: clean all

.PHONY: all dirs clean install uninstall test debug
