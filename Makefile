mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(patsubst %/,%,$(dir $(mkfile_path)))
tools_dir   := $(current_dir)/tools

UNITYGITURL := https://github.com/ThrowTheSwitch/Unity.git

SRCDIR   := $(current_dir)/src
TSRCDIR  := $(current_dir)/test
UDIR     := $(TSRCDIR)/third/Unity
USRCDIR  := $(UDIR)/src
OUTDIR   := $(current_dir)/out
OBJDIR   := $(OUTDIR)/obj
DEPDIR   := $(OUTDIR)/deps
RESDIR   := $(OUTDIR)/results

BOOTSTRAP_SKIP := clean
ifeq ($(filter $(BOOTSTRAP_SKIP),$(MAKECMDGOALS)),)
  ifneq ($(wildcard $(UDIR)/.git),$(UDIR)/.git)
    $(info [bootstrap] Cloning Unity into $(UDIR)...)
    $(shell mkdir -p $(dir $(UDIR)) && git clone --depth=1 $(UNITYGITURL) $(UDIR) 1>&2)
  endif
endif

CC      ?= gcc
CSTD    ?= c99
DEFINES :=

SRC   := $(wildcard $(SRCDIR)/*.c)
TSRC  := $(wildcard $(TSRCDIR)/*.c)
USRC   = $(wildcard $(USRCDIR)/*.c)
OBJ   := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))
TOBJ  := $(patsubst $(TSRCDIR)/%.c,$(OBJDIR)/%.o,$(TSRC))
UOBJ   = $(patsubst $(USRCDIR)/%.c,$(OBJDIR)/%.o,$(USRC))

TESTBIN := $(OUTDIR)/test

WARN ?= -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion \
        -Wshadow -Wpointer-arith -Wcast-align -Wcast-qual -Wstrict-aliasing \
        -Wmissing-prototypes -Waggregate-return -Wformat=2 -Wundef \
        -Wwrite-strings -Wredundant-decls -Wnested-externs \
        -Wold-style-definition -Wstrict-overflow=5 -Wdouble-promotion \
        -Wswitch-enum -Wbad-function-cast -Wfloat-equal -Winline \
        -Wlogical-op -Wduplicated-cond -Wduplicated-branches \
        -Wno-unused-parameter -Wno-unknown-pragmas

IS_CLANG := $(shell $(CC) --version 2>/dev/null | head -n 1 | grep -iq clang && echo 1 || echo 0)
IS_GCC   := $(shell $(CC) --version 2>/dev/null | head -n 1 | grep -iq gcc && echo 1 || echo 0)
IS_GNU_STD := $(strip $(filter gnu%,$(CSTD)))
ifneq ($(IS_GNU_STD),)
  ifeq ($(IS_CLANG),1)
    WARN += -Wno-gnu-statement-expression -Wno-gnu-statement-expression-from-macro-expansion
  endif
  ifeq ($(IS_GCC),1)
    WARN := $(filter-out -Wpedantic,$(WARN))
  endif
endif

COMPILE = $(CC) -c
LINK    = $(CC)
CFLAGS  = -std=$(CSTD) $(WARN) $(DEFINES)
LDFLAGS = 

RESULTS = $(patsubst $(TSRCDIR)/%.c,$(RESDIR)/%.txt,$(TSRC))

PASSED = `grep -s PASS $(RESDIR)/*.txt`
FAIL = `grep -s FAIL $(RESDIR)/*.txt`
IGNORE = `grep -s IGNORE $(RESDIR)/*.txt`


CFLAGS_SRC   := -I$(SRCDIR)
CFLAGS_TEST  := -I$(SRCDIR) -I$(TSRCDIR) -I$(USRCDIR) -Wno-missing-prototypes -Wno-bad-function-cast -Wno-format-zero-length
CFLAGS_UNITY := -I$(USRCDIR) -Wno-all -Wno-pedantic -Wno-conversion -Wno-float-equal -Wno-bad-function-cast

# ── Default ───────────────────────────────────────────────────────────────────
.PHONY: all c11 c17 c23 c2y gnu gnu11 gnu17 gnu23 gnu2y tests clean
.DELETE_ON_ERROR:
all: clean tests
c11:
	$(MAKE) CSTD=c11
c17:
	$(MAKE) CSTD=c17
c23:
	$(MAKE) CSTD=c23
c2y:
	$(MAKE) CSTD=c2y
gnu:
	$(MAKE) CSTD=gnu99
gnu11:
	$(MAKE) CSTD=gnu11
gnu17:
	$(MAKE) CSTD=gnu17
gnu23:
	$(MAKE) CSTD=gnu23
gnu2y:
	$(MAKE) CSTD=gnu2y

clean:
	@echo "  CLEAN   objects & deps"
	rm -rf $(OUTDIR)

tests: $(UDIR) $(RESULTS) $(TESTBIN)
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
$(patsubst $(USRCDIR)/%.c,$(OBJDIR)/%.o,$(USRC)): CFLAGS += $(CFLAGS_UNITY)
$(patsubst $(TSRCDIR)/%.c,$(OBJDIR)/%.o,$(TSRC)): CFLAGS += $(CFLAGS_TEST)

$(TESTBIN): $(OBJ) $(UOBJ) $(TOBJ)
	@echo "  LD  $@"
	@mkdir -p $(@D)
	$(LINK) $(LDFLAGS) -o $@ $^

vpath %.c $(SRCDIR) $(USRCDIR) $(TSRCDIR)

$(OBJDIR)/%.o: %.c
	@echo "  CC  $<"
	@mkdir -p $(OBJDIR) $(DEPDIR)
	$(COMPILE) $(CFLAGS) -MMD -MP -MF $(DEPDIR)/$*.d -MT $@ -o $@ $<

-include $(patsubst $(OBJDIR)/%.o,$(DEPDIR)/%.d,$(OBJ) $(TOBJ) $(UOBJ))

