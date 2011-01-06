#-------------------------------------------------------------------------
#
# Makefile for the plpsm shared object
#
# src/pl/plpsm/src/Makefile
#
#-------------------------------------------------------------------------

subdir = src/pl/psm0
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global

# Shared library parameters
NAME= psm0

override CPPFLAGS := -I. -I$(srcdir) $(CPPFLAGS)
SHLIB_LINK = $(filter -lintl, $(LIBS))
rpath =

OBJS = gram.o handler.o scanner.o compiler.o executor.o

all: all-lib

# Shared library stuff
include $(top_srcdir)/src/Makefile.shlib


install: installdirs all install-lib

installdirs: installdirs-lib

uninstall: uninstall-lib


# Force these dependencies to be known even without dependency info built:
gram.o handler.o scanner.o: psm.h gram.h

# See notes in src/backend/parser/Makefile about the following two rules

pl_gram.h: pl_gram.c ;

gram.c: gram.y
ifdef BISON
	$(BISON) -d $(BISONFLAGS) -o $@ $<
else
	@$(missing) bison $< $@
endif

# so they are not cleaned here.
clean distclean: clean-lib
	rm -f $(OBJS)

maintainer-clean: clean
	rm -f gram.c gram.y
