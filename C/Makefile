# TAML C Library Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -O2 -Iinclude
LDFLAGS = 
AR = ar
ARFLAGS = rcs

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
LIB_DIR = lib
TEST_DIR = tests
EXAMPLE_DIR = examples

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# Library name
LIBRARY = $(LIB_DIR)/libtaml.a
SHARED_LIBRARY = $(LIB_DIR)/libtaml.so

# Test and example programs
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)
TEST_PROGRAMS = $(patsubst $(TEST_DIR)/%.c,$(TEST_DIR)/%,$(TEST_SOURCES))

EXAMPLE_SOURCES = $(wildcard $(EXAMPLE_DIR)/*.c)
EXAMPLE_PROGRAMS = $(patsubst $(EXAMPLE_DIR)/%.c,$(EXAMPLE_DIR)/%,$(EXAMPLE_SOURCES))

# Default target
all: directories $(LIBRARY) $(SHARED_LIBRARY)

# Create directories
directories:
	@mkdir -p $(OBJ_DIR) $(LIB_DIR)

# Compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build static library
$(LIBRARY): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^
	@echo "Static library built: $@"

# Build shared library
$(SHARED_LIBRARY): $(OBJECTS)
	$(CC) -shared -o $@ $^ $(LDFLAGS)
	@echo "Shared library built: $@"

# Build tests
tests: $(LIBRARY) $(TEST_PROGRAMS)

$(TEST_DIR)/%: $(TEST_DIR)/%.c $(LIBRARY)
	$(CC) $(CFLAGS) $< $(LIBRARY) -o $@

# Build examples
examples: $(LIBRARY) $(EXAMPLE_PROGRAMS)

$(EXAMPLE_DIR)/%: $(EXAMPLE_DIR)/%.c $(LIBRARY)
	$(CC) $(CFLAGS) $< $(LIBRARY) -o $@

# Run tests
test: tests
	@echo "Running tests..."
	@for test in $(TEST_PROGRAMS); do \
		echo "Running $$test..."; \
		./$$test || exit 1; \
	done
	@echo "All tests passed!"

# Install library
install: all
	@echo "Installing TAML library..."
	@mkdir -p /usr/local/lib /usr/local/include
	@cp $(LIBRARY) /usr/local/lib/
	@cp $(SHARED_LIBRARY) /usr/local/lib/
	@cp $(INC_DIR)/taml.h /usr/local/include/
	@echo "Library installed to /usr/local/lib"
	@echo "Header installed to /usr/local/include"

# Uninstall library
uninstall:
	@echo "Uninstalling TAML library..."
	@rm -f /usr/local/lib/libtaml.a
	@rm -f /usr/local/lib/libtaml.so
	@rm -f /usr/local/include/taml.h
	@echo "Library uninstalled"

# Clean build artifacts
clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(LIB_DIR)/*.a $(LIB_DIR)/*.so
	rm -f $(TEST_PROGRAMS) $(EXAMPLE_PROGRAMS)
	@echo "Clean complete"

# Help
help:
	@echo "TAML C Library Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all        - Build the library (default)"
	@echo "  tests      - Build test programs"
	@echo "  examples   - Build example programs"
	@echo "  test       - Run all tests"
	@echo "  install    - Install library to /usr/local"
	@echo "  uninstall  - Uninstall library"
	@echo "  clean      - Remove build artifacts"
	@echo "  help       - Show this help message"

.PHONY: all directories tests examples test install uninstall clean help
