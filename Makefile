mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(patsubst %/,%,$(dir $(mkfile_path)))

# ── Compiler and flags ─────────────────────────────────────────────────────────
CC      := gcc
CSTD    := -std=c11
WARN    := -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion \
           -Wshadow -Wpointer-arith -Wcast-align -Wcast-qual -Wstrict-aliasing \
           -Wmissing-prototypes -Waggregate-return -Wformat=2 -Wundef \
           -Wwrite-strings -Wredundant-decls -Wnested-externs \
           -Wold-style-definition -Wstrict-overflow=5 -Wdouble-promotion \
           -Wswitch-enum -Wbad-function-cast -Wfloat-equal -Winline \
           -Wlogical-op -Wduplicated-cond -Wduplicated-branches \
           -Wno-unused-parameter -Wno-unknown-pragmas
OPTDBG  := -O2 -g -fno-omit-frame-pointer
SAN     := -fsanitize=address
DEPGEN  := -MMD -MP

CFLAGS  := $(CSTD) $(WARN) $(OPTDBG) $(SAN) $(DEPGEN)
LDFLAGS :=
LDLIBS  := -lasan

# ── Names and layout (NO SPACES inside these) ─────────────────────────────────
LIBNAME      := cvec
LIBVER_MAJOR := 1
LIBVER       := 1.0.0

SRCDIR  := $(current_dir)/cvec
OBJDIR  := $(current_dir)/out

LIBSRC  := cvec.c
TESTSRC := test.c

USE_PUSH_BACK_VFMT ?= 1

ifeq ($(USE_PUSH_BACK_VFMT),1)
	CFLAGS += -DUSE_PUSH_BACK_VFMT
	LIBSRC += cvec_vfmt.c
endif

LIBOBJ     := $(addprefix $(OBJDIR)/,$(LIBSRC:.c=.o))
LIBOBJ_PIC := $(addprefix $(OBJDIR)/,$(LIBSRC:.c=.pic.o))
TESTOBJ    := $(addprefix $(OBJDIR)/,$(TESTSRC:.c=.o))

STATICLIB  := $(OBJDIR)/lib$(LIBNAME).a

SO_BASENAME := lib$(LIBNAME).so
SO_SONAME   := $(SO_BASENAME).$(LIBVER_MAJOR)
SO_REAL     := $(OBJDIR)/$(SO_BASENAME).$(LIBVER)
SO_LINK     := $(OBJDIR)/$(SO_SONAME)
SO_SHORT    := $(OBJDIR)/$(SO_BASENAME)

TEST_SHARED := $(OBJDIR)/test_$(LIBNAME)_shared
TEST_STATIC := $(OBJDIR)/test_$(LIBNAME)_static

DEPS := $(LIBOBJ:.o=.d) $(LIBOBJ_PIC:.o=.d) $(TESTOBJ:.o=.d)

# ── Default ───────────────────────────────────────────────────────────────────
.PHONY: all
all: static shared tests

.PHONY: static shared tests
static: $(STATICLIB)
shared: $(SO_REAL)
tests:  $(TEST_SHARED) $(TEST_STATIC)

# ── Static lib ────────────────────────────────────────────────────────────────
$(STATICLIB): $(LIBOBJ) | $(OBJDIR)
	@echo "  AR      $@"
	ar rcs $@ $(LIBOBJ)

# ── Shared lib + symlinks (created here; keep names contiguous) ───────────────
$(SO_REAL): $(LIBOBJ_PIC) | $(OBJDIR)
	@echo "  SO      $@  (SONAME=$(SO_SONAME))"
	$(CC) -shared $(LDFLAGS) -Wl,-soname,$(SO_SONAME) -o $@ $(LIBOBJ_PIC) $(LDLIBS)
	@ln -sf $(notdir $@) $(SO_LINK)
	@ln -sf $(notdir $(SO_LINK)) $(SO_SHORT)

# ── Tests ─────────────────────────────────────────────────────────────────────
$(TEST_SHARED): $(TESTOBJ) $(SO_REAL)
	@echo "  LINK    $@ (shared)"
	$(CC) $(CFLAGS) -o $@ $(TESTOBJ) -L$(OBJDIR) -l$(LIBNAME) $(LDLIBS) -Wl,-rpath,'$$ORIGIN'

$(TEST_STATIC): $(TESTOBJ) $(STATICLIB)
	@echo "  LINK    $@ (static)"
	$(CC) $(CFLAGS) -o $@ $(TESTOBJ) $(STATICLIB) $(LDLIBS)

# ── Compile rules ─────────────────────────────────────────────────────────────
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@echo "  CC      $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.pic.o: $(SRCDIR)/%.c | $(OBJDIR)
	@echo "  CC(PIC) $<"
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(OBJDIR):
	@mkdir -p $@

# ── Convenience ───────────────────────────────────────────────────────────────
.PHONY: run run-shared run-static clean veryclean
run: run-shared
run-shared: $(TEST_SHARED)
	@echo "  RUN     $<"
	$(TEST_SHARED)
run-static: $(TEST_STATIC)
	@echo "  RUN     $<"
	$(TEST_STATIC)

clean:
	@echo "  CLEAN   objects & deps"
	rm -rf $(OBJDIR)

veryclean: clean
	@echo "  CLEAN   libs & bins"
	rm -f $(STATICLIB) $(SO_REAL) $(SO_LINK) $(SO_SHORT) $(TEST_SHARED) $(TEST_STATIC)

-include $(DEPS)
