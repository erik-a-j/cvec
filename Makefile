mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(patsubst %/,%,$(dir $(mkfile_path)))

# Compiler and flags
CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion \
           -Wshadow -Wpointer-arith -Wcast-align -Wcast-qual -Wstrict-aliasing \
           -Wmissing-prototypes -Waggregate-return \
           -Wformat=2 -Wundef -Wwrite-strings -Wredundant-decls -Wnested-externs \
           -Wold-style-definition -Wstrict-overflow=5 -Wdouble-promotion \
           -Wswitch-enum -Wbad-function-cast -Wfloat-equal -Winline \
           -Wlogical-op -Wduplicated-cond -Wduplicated-branches \
           -Wno-unused-parameter -O2 -g -fno-omit-frame-pointer -fsanitize=address -MMD -MP
LDLIBS  := -lasan

# Target name
TARGET  := test_cvec

# Source files (all .c files in this directory)
SRCDIR  := $(current_dir)
OBJDIR  := $(current_dir)/out
SRCS    := test.c
OBJS    := $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))
DEPS    := $(OBJS:.o=.d)

# Default rule
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

# Compile
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	@mkdir -p $@

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Convenience target to run the built executable
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run

# include auto-generated header dependency files
-include $(DEPS)