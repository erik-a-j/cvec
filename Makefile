mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(patsubst %/,%,$(dir $(mkfile_path)))
tools_dir   := $(current_dir)/tools

CC      ?= gcc
CSTD    ?= -std=c11
DEFINES :=

UNITYGITURL := https://github.com/ThrowTheSwitch/Unity.git

SRCDIR   := $(current_dir)/src
TSRCDIR  := $(current_dir)/test
UDIR     := $(TSRCDIR)/Unity
USRCDIR  := $(UDIR)/src
OUTDIR   := $(current_dir)/out
OBJDIR   := $(OUTDIR)/obj
DEPDIR   := $(OUTDIR)/deps
RESDIR   := $(OUTDIR)/results

SRC   := $(wildcard $(SRCDIR)/*.c)
TSRC  := $(wildcard $(TSRCDIR)/*.c)
USRC  := $(wildcard $(USRCDIR)/*.c)
HDR   := $(wildcard $(SRCDIR)/*.h)
THDR  := $(wildcard $(TSRCDIR)/*.h)
UHDR  := $(wildcard $(USRCDIR)/*.h)
OBJ   := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))
TOBJ  := $(patsubst $(TSRCDIR)/%.c,$(OBJDIR)/%.o,$(TSRC))
UOBJ  := $(patsubst $(USRCDIR)/%.c,$(OBJDIR)/%.o,$(USRC))

TESTBIN := $(OUTDIR)/test

WARN ?= -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion \
        -Wshadow -Wpointer-arith -Wcast-align -Wcast-qual -Wstrict-aliasing \
        -Wmissing-prototypes -Waggregate-return -Wformat=2 -Wundef \
        -Wwrite-strings -Wredundant-decls -Wnested-externs \
        -Wold-style-definition -Wstrict-overflow=5 -Wdouble-promotion \
        -Wswitch-enum -Wbad-function-cast -Wfloat-equal -Winline \
        -Wlogical-op -Wduplicated-cond -Wduplicated-branches \
        -Wno-unused-parameter -Wno-unknown-pragmas

COMPILE = $(CC) -c
LINK    = $(CC)
CFLAGS  = $(CSTD) $(WARN) $(DEFINES)
LDFLAGS = 

RESULTS = $(patsubst $(TSRCDIR)/%.c,$(RESDIR)/%.txt,$(TSRC))

PASSED = `grep -s PASS $(RESDIR)/*.txt`
FAIL = `grep -s FAIL $(RESDIR)/*.txt`
IGNORE = `grep -s IGNORE $(RESDIR)/*.txt`


CFLAGS_SRC   := -I$(SRCDIR)
CFLAGS_TEST  := -I$(SRCDIR) -I$(TSRCDIR) -I$(USRCDIR) -Wno-missing-prototypes
CFLAGS_UNITY := -I$(USRCDIR) -Wno-all -Wno-pedantic -Wno-conversion -Wno-float-equal

# ── Default ───────────────────────────────────────────────────────────────────
.PHONY: all tests clean
.DELETE_ON_ERROR:

all: tests
tests: $(UDIR) $(TESTBIN) $(RESULTS)
	@echo -e "-----------------------\nIGNORES:\n-----------------------"
	@echo "$(IGNORE)"
	@echo -e "-----------------------\nFAILURES:\n-----------------------"
	@echo "$(FAIL)"
	@echo -e "-----------------------\nPASSED:\n-----------------------"
	@echo "$(PASSED)"
	@echo -e "\nDONE"

$(RESDIR)/%.txt: $(TESTBIN)
	@mkdir -p $(RESDIR)
	-$< > $@ 2>&1

$(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC)): CFLAGS += $(CFLAGS_SRC)
$(patsubst $(TSRCDIR)/%.c,$(OBJDIR)/%.o,$(TSRC)): CFLAGS += $(CFLAGS_TEST)
$(patsubst $(USRCDIR)/%.c,$(OBJDIR)/%.o,$(USRC)): CFLAGS += $(CFLAGS_UNITY)

$(TESTBIN): $(OBJ) $(TOBJ) $(UOBJ)
	@echo "  LD  $@"
	@mkdir -p $(@D)
	$(LINK) $(LDFLAGS) -o $@ $^

vpath %.c $(SRCDIR) $(TSRCDIR) $(USRCDIR)

$(OBJDIR)/%.o: %.c
	@echo "  CC  $<"
	@mkdir -p $(OBJDIR) $(DEPDIR)
	$(COMPILE) $(CFLAGS) -MMD -MP -MF $(DEPDIR)/$*.d -MT $@ -o $@ $<

-include $(patsubst $(OBJDIR)/%.o,$(DEPDIR)/%.d,$(OBJ) $(TOBJ) $(UOBJ))

$(UDIR):
	@echo "  git cloning $(UNITYGITURL)..."
	@git clone $(UNITYGITURL) $(UDIR)

clean:
	@echo "  CLEAN   objects & deps"
	rm -rf $(OUTDIR)

