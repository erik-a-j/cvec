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
INC     := -Isrc
DEFINES :=

# ── Names and layout (NO SPACES inside these) ─────────────────────────────────
LIBNAME      := cvec
LIBVER_MAJOR := 1
LIBVER       := 1.0.0

SRCDIR  := $(current_dir)/src
OBJDIR  := $(current_dir)/out

LIBSRC  := cvec.c
TESTSRC := test.c


USE_MACRO      ?= 1
USE_STRING_EXT ?= 1
USE_DUMP       ?= 1
USE_FMT        ?= 1

ifeq ($(USE_MACRO),1)
	DEFINES += -DUSE_MACRO
endif
ifeq ($(USE_STRING_EXT),1)
	DEFINES += -DUSE_STRING_EXT
endif
ifeq ($(filter 1,$(USE_DUMP))$(filter 1,$(USE_FMT)),11)
	DEFINES += -DUSE_FMT -DUSE_DUMP
	LIBSRC += cvec_fmt.c cvec_dump.c
else
	ifeq ($(USE_FMT),1)
		DEFINES += -DUSE_FMT
		LIBSRC += cvec_fmt.c
	endif
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
all: static shared api_header tests

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
	$(CC) $(CFLAGS) $(DEFINES) -o $@ $(TESTOBJ) -L$(OBJDIR) -l$(LIBNAME) $(LDLIBS) -Wl,-rpath,'$$ORIGIN'

$(TEST_STATIC): $(TESTOBJ) $(STATICLIB)
	@echo "  LINK    $@ (static)"
	$(CC) $(CFLAGS) $(DEFINES) -o $@ $(TESTOBJ) $(STATICLIB) $(LDLIBS)

# ── Compile rules ─────────────────────────────────────────────────────────────
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@echo "  CC      $<"
	$(CC) $(INC) $(CFLAGS) $(DEFINES) -c $< -o $@

$(OBJDIR)/%.pic.o: $(SRCDIR)/%.c | $(OBJDIR)
	@echo "  CC(PIC) $<"
	$(CC) $(INC) $(CFLAGS) $(DEFINES) -fPIC -c $< -o $@

$(OBJDIR):
	@mkdir -p $@

# ── Convenience ───────────────────────────────────────────────────────────────
.PHONY: run run-shared run-static clean veryclean api_header
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

api_header:
	bash -c "$(current_dir)/tools/make_api_header.sh $(DEFINES)"

-include $(DEPS)
