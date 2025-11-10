mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(patsubst %/,%,$(dir $(mkfile_path)))
tools_dir   := $(current_dir)/tools

CC      ?= gcc
CSTD    ?= -std=c11
DEFINES :=
DEPGEN  := -MMD -MP

SRCDIR     := $(current_dir)/src
OUTDIR     ?= $(current_dir)/out
BUILDDIR   := $(OUTDIR)/build
OBJDIR     := $(BUILDDIR)/obj

SRC   := $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*/*.c)
HDR   := $(wildcard $(SRCDIR)/*.h) $(wildcard $(SRCDIR)/*/*.h)
OBJ   := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))

TESTBIN := $(OUTDIR)/test

WARN ?= -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion \
        -Wshadow -Wpointer-arith -Wcast-align -Wcast-qual -Wstrict-aliasing \
        -Wmissing-prototypes -Waggregate-return -Wformat=2 -Wundef \
        -Wwrite-strings -Wredundant-decls -Wnested-externs \
        -Wold-style-definition -Wstrict-overflow=5 -Wdouble-promotion \
        -Wswitch-enum -Wbad-function-cast -Wfloat-equal -Winline \
        -Wlogical-op -Wduplicated-cond -Wduplicated-branches \
        -Wno-unused-parameter -Wno-unknown-pragmas

# Recompose CFLAGS/LDFLAGS with mode + extras
CFLAGS  := -g $(CSTD) $(WARN) $(DEFINES) $(DEPGEN)
LDFLAGS := 

# ── Default ───────────────────────────────────────────────────────────────────
.PHONY: all
all: $(TESTBIN)

# ── Compile rules ─────────────────────────────────────────────────────────────
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo "  CC      $<"
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(DEFINES) -c $< -o $@

$(TESTBIN): $(OBJ)
	@echo "  CC      $<"
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)


# ── Convenience ───────────────────────────────────────────────────────────────
.PHONY: clean 

clean:
	@echo "  CLEAN   objects & deps"
	rm -rf $(OUTDIR)

-include $(OBJ:.o=.d)

.DELETE_ON_ERROR:
