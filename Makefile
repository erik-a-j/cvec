mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(patsubst %/,%,$(dir $(mkfile_path)))
tools_dir   := $(current_dir)/tools

COLOR ?= 1
ESC    :=
BOLD   :=
YELLOW :=
RED    :=
GREEN  :=
BLUE   :=
RESET  :=
ifeq ($(COLOR),1)
  ESC    := $(shell printf '\033')
  BOLD   := $(ESC)[1m
  YELLOW := $(ESC)[33m
  RED    := $(ESC)[31m
  GREEN  := $(ESC)[32m
  BLUE   := $(ESC)[34m
  RESET  := $(ESC)[0m
endif

define warn
$(warning ⚠️ $(BOLD)$(YELLOW)$(1)$(RESET))
endef
define info
$(info ℹ️ $(BOLD)$(BLUE)$(1)$(RESET))
endef

# ── Build mode (debug|release) ────────────────────────────────────────────────
MODE ?= debug

CC      ?= gcc
CSTD    := -std=c11
INC     := -I$(current_dir)/src/include
DEFINES :=
DEPGEN  := -MMD -MP

# ── Names and layout (NO SPACES inside these) ─────────────────────────────────
LIBNAME      := cvec
LIBVER_MAJOR := 1
LIBVER       := 1.0.0

SRCDIR     := $(current_dir)/src
OUTDIR     := $(current_dir)/out
BUILDDIR   := $(OUTDIR)/build
TESTDIR    := $(OUTDIR)/test
OBJDIR     := $(BUILDDIR)/obj
RELEASEDIR := $(OUTDIR)/cvec

LIBSRC  := cvec.c
TESTSRC := test/test.c

USE_MACRO      ?= 1
USE_STRING_EXT ?= 1
USE_DUMP       ?= 1
USE_FMT        ?= 1

ifeq ($(USE_MACRO),1)
  DEFINES += -DUSE_MACRO
endif
ifeq ($(USE_DUMP),1)
  DEFINES += -DUSE_STRING_EXT -DUSE_FMT -DUSE_DUMP
  LIBSRC += cvec_fmt.c cvec_dump.c
else
  ifeq ($(USE_STRING_EXT),1)
    DEFINES += -DUSE_STRING_EXT
  endif
  ifeq ($(USE_FMT),1)
    DEFINES += -DUSE_FMT
    LIBSRC += cvec_fmt.c
  endif
endif

WARN ?= -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion \
        -Wshadow -Wpointer-arith -Wcast-align -Wcast-qual -Wstrict-aliasing \
        -Wmissing-prototypes -Waggregate-return -Wformat=2 -Wundef \
        -Wwrite-strings -Wredundant-decls -Wnested-externs \
        -Wold-style-definition -Wstrict-overflow=5 -Wdouble-promotion \
        -Wswitch-enum -Wbad-function-cast -Wfloat-equal -Winline \
        -Wlogical-op -Wduplicated-cond -Wduplicated-branches \
        -Wno-unused-parameter -Wno-unknown-pragmas

# You can flip features here for release; override with make MODE=release
LTO        ?= 1
HARDEN     ?= 1
PORTABLE   ?= 1   # set to 0 to add -march=native
STRIP_BINS ?= 1   # strip test binaries in release

STRIP      ?= strip

# Common extras
GCSECTIONS := -ffunction-sections -fdata-sections
LD_GC     := -Wl,--gc-sections
LD_SPEED  := -Wl,-O1,--as-needed

# Security hardening for executables/libs
HARDEN_CFLAGS  := -fstack-protector-strong -D_FORTIFY_SOURCE=3
HARDEN_LDFLAGS := -Wl,-z,relro,-z,now -Wl,-z,defs

# Visibility: hide all by default; mark public API with __attribute__((visibility("default")))
VISIBILITY := -fvisibility=hidden

# Reproducible builds: map absolute paths out of debug info (optional)
REPRO ?= 1
ifeq ($(REPRO),1)
  REPRO_FLAG := -ffile-prefix-map=$(current_dir)=.
endif

USER_SET_AR     := $(filter command\ line environment,$(origin AR))
USER_SET_RANLIB := $(filter command\ line environment,$(origin RANLIB))
ifeq ($(LTO),1)
  LTO_CFLAGS  := -flto
  LTO_LDFLAGS := -flto
  CC_IS_CLANG := $(shell $(CC) -v 2>&1 | grep -qi clang && echo yes || echo no)

  ifeq ($(USER_SET_AR),)
    ifeq ($(CC_IS_CLANG),yes)
      AR_SELECTED := $(shell command -v llvm-ar >/dev/null 2>&1 && echo llvm-ar || $(CC) -print-prog-name=ar)
      ifneq ($(notdir $(AR_SELECTED)),llvm-ar)
        $(call warn,llvm-ar not found; using AR='$(AR_SELECTED)')
      endif
    else
      AR_SELECTED := $(shell $(CC) -print-prog-name=ar)
    endif
  else
    AR_SELECTED := $(AR)
  endif

  ifeq ($(USER_SET_RANLIB),)
    ifeq ($(CC_IS_CLANG),yes)
      RANLIB_SELECTED := $(shell command -v llvm-ranlib >/dev/null 2>&1 && echo llvm-ranlib || $(CC) -print-prog-name=ranlib)
      ifneq ($(notdir $(RANLIB_SELECTED)),llvm-ranlib)
        $(call warn,llvm-ranlib not found; using RANLIB='$(RANLIB_SELECTED)')
      endif
    else
      RANLIB_SELECTED := $(shell $(CC) -print-prog-name=ranlib)
    endif
  else
    RANLIB_SELECTED := $(RANLIB)
  endif
endif

# Finalize variables (with fallbacks if LTO=0)
AR     := $(if $(AR_SELECTED),$(AR_SELECTED),$(AR))
AR     := $(if $(AR),$(AR),ar)
RANLIB := $(if $(RANLIB_SELECTED),$(RANLIB_SELECTED),$(RANLIB))
RANLIB := $(if $(RANLIB),$(RANLIB),ranlib)

AR_IS_GNULLVM := $(shell ( $(AR) --version 2>/dev/null | grep -Eiq 'GNU|LLVM' ) && echo yes || echo no)
ifeq ($(origin ARFLAGS), default)
  ifeq ($(AR_IS_GNULLVM),yes)
    ARFLAGS = rcsD
  else
    ARFLAGS = rcs
  endif
endif
USE_RANLIB := $(if $(findstring s,$(ARFLAGS)),0,1)

# Architecture tuning (portable by default)
ifeq ($(PORTABLE),0)
  ARCHFLAGS := -march=native
endif

# ── Per-mode flag bundles ────────────────────────────────────────────────────
ifeq ($(MODE),release)
  OPTDBG := -O3
  SAN    :=
  DEFINES += -DNDEBUG
else
  # debug (default)
  OPTDBG := -O2 -g -fno-omit-frame-pointer
  SAN    := -fsanitize=address
endif

# Recompose CFLAGS/LDFLAGS with mode + extras
CFLAGS  := $(CSTD) $(WARN) $(OPTDBG) $(SAN) $(DEPGEN) $(INC) $(DEFINES) \
           $(GCSECTIONS) $(VISIBILITY) $(REPRO_FLAG) $(LTO_CFLAGS) $(ARCHFLAGS)
LDFLAGS := $(SAN) $(LD_SPEED) $(LD_GC) $(LTO_LDFLAGS)

ifeq ($(MODE),release)
  ifeq ($(HARDEN),1)
    CFLAGS  += $(HARDEN_CFLAGS)
    LDFLAGS += $(HARDEN_LDFLAGS)
  endif
endif

LIBOBJ     := $(addprefix $(OBJDIR)/,$(LIBSRC:.c=.o))
LIBOBJ_PIC := $(addprefix $(OBJDIR)/,$(LIBSRC:.c=.pic.o))
TESTOBJ    := $(addprefix $(OBJDIR)/,$(TESTSRC:.c=.o))

TEST_RPATH := -Wl,--enable-new-dtags -Wl,-rpath,'$$ORIGIN:$$ORIGIN/../build'

STATICLIB  := $(BUILDDIR)/lib$(LIBNAME).a

SO_BASENAME := lib$(LIBNAME).so
SO_SONAME   := $(SO_BASENAME).$(LIBVER_MAJOR)
SO_REAL     := $(BUILDDIR)/$(SO_BASENAME).$(LIBVER)
SO_LINK     := $(BUILDDIR)/$(SO_SONAME)
SO_SHORT    := $(BUILDDIR)/$(SO_BASENAME)

TEST_SHARED := $(TESTDIR)/test_$(LIBNAME)_shared
TEST_STATIC := $(TESTDIR)/test_$(LIBNAME)_static

DEPS := $(LIBOBJ:.o=.d) $(LIBOBJ_PIC:.o=.d) $(TESTOBJ:.o=.d)

# ── Default ───────────────────────────────────────────────────────────────────
.PHONY: all
all: static shared tests

.PHONY: static shared test-static test-shared tests
static: $(STATICLIB)
shared: $(SO_REAL)
tests:  $(TEST_SHARED) $(TEST_STATIC)
test-static:  $(TEST_STATIC)
test-shared:  $(TEST_SHARED)

# ── Static lib ────────────────────────────────────────────────────────────────
$(STATICLIB): $(LIBOBJ) | $(BUILDDIR)
	@echo "  AR      $@"
	$(AR) $(ARFLAGS) $@ $(LIBOBJ)
ifneq ($(USE_RANLIB),0)
	$(RANLIB) $@
endif

# ── Shared lib + symlinks (created here; keep names contiguous) ───────────────
$(SO_REAL): $(LIBOBJ_PIC) | $(BUILDDIR)
	@echo "  SO      $@  (SONAME=$(SO_SONAME))"
	$(CC) -shared $(LDFLAGS) -Wl,-soname,$(SO_SONAME) -o $@ $(LIBOBJ_PIC) $(LDLIBS)
	@ln -sf $(notdir $@) $(SO_LINK)
	@ln -sf $(notdir $(SO_LINK)) $(SO_SHORT)

# ── Tests ──────────────────────────────────────────────────────────────────────
$(TEST_SHARED): $(TESTOBJ) $(SO_REAL) | $(TESTDIR)
	@echo "  LINK    $@ (shared)"
	$(CC) $(CFLAGS) $(LDFLAGS) $(DEFINES) $(TEST_RPATH) -o $@ $(TESTOBJ) -L$(BUILDDIR) -l$(LIBNAME) $(LDLIBS)

$(TEST_STATIC): $(TESTOBJ) $(STATICLIB) | $(TESTDIR)
	@echo "  LINK    $@ (static)"
	$(CC) $(CFLAGS) $(LDFLAGS) $(DEFINES) -o $@ $(TESTOBJ) $(STATICLIB) $(LDLIBS)

# ── Compile rules ─────────────────────────────────────────────────────────────
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo "  CC      $<"
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(DEFINES) -c $< -o $@

$(OBJDIR)/%.pic.o: $(SRCDIR)/%.c
	@echo "  CC(PIC) $<"
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(DEFINES) -fPIC -c $< -o $@

$(OUTDIR):
	@mkdir -p $@
$(BUILDDIR):
	@mkdir -p $@
$(TESTDIR):
	@mkdir -p $@
$(OBJDIR):
	@mkdir -p $@
$(RELEASEDIR):
	@mkdir -p $@

.PHONY: release
release:
	$(MAKE) MODE=release static shared copy-headers api_header

# ── Convenience ───────────────────────────────────────────────────────────────
.PHONY: run run-shared run-static clean copy-headers api_header
run: run-shared
run-shared: $(TEST_SHARED)
	@echo "  RUN     $<"
	$(TEST_SHARED)

run-static: $(TEST_STATIC)
	@echo "  RUN     $<"
	$(TEST_STATIC)

clean:
	@echo "  CLEAN   objects & deps"
	rm -rf $(OUTDIR)

copy-headers:
	"$(tools_dir)/header_names.sh" $(DEFINES) | \
	"$(tools_dir)/copy_headers.sh" "$(RELEASEDIR)/include"

api_header:
	"$(tools_dir)/header_names.sh" $(DEFINES) | \
	"$(tools_dir)/make_api_header.sh" "$(RELEASEDIR)/include/cvec.h"


-include $(DEPS)
