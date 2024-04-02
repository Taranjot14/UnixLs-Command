# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra

# Executable name
TARGET = UnixLs

# Source files
SRCS = UnixLs.c

# Default options
DEFAULT_OPTS = 

# Extract options from the source file
OPTIONS := $(shell grep -oP '(?<=\/\*COMMAND_OPTIONS:)(.*?)(?=\*\/)' $(SRCS))

# Compile and link
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(DEFAULT_OPTS)

.PHONY: clean
clean:
	rm -f $(TARGET)
