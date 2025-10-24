# Compiler and flags
CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion \
           -Wshadow -Wpointer-arith -Wcast-align -Wcast-qual -Wstrict-aliasing \
           -Wmissing-prototypes -Waggregate-return \
           -Wformat=2 -Wundef -Wwrite-strings -Wredundant-decls -Wnested-externs \
           -Wold-style-definition -Wstrict-overflow=5 -Wdouble-promotion \
           -Wswitch-enum -Wbad-function-cast -Wfloat-equal -Winline \
           -Wlogical-op -Wduplicated-cond -Wduplicated-branches \
           -Wno-unused-parameter -Werror -O2 -g

# Target name
TARGET  := test_cvec

# Source files (all .c files in this directory)
SRCS    := test.c #$(wildcard *.c)
OBJS    := $(SRCS:.c=.o)

# Default rule
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Convenience target to run the built executable
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
