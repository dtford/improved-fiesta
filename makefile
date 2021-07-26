CXX=g++
CXXFLAGS=
LFLAGS=
LIBS=
DFLAGS=-DDEBUG

SRCDIR := src
INCDIR := inc
BUILDDIR := obj

SRCEXT := cpp
OBJEXT := o

INC := -I$(INCDIR)

SRC := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
$(info $$SRC is [${SRC}])
OBJ := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SRC:.$(SRCEXT)=.$(OBJEXT)))
$(info $$OBJ is [${OBJ}])

MAIN=thermo

RM=rm -rf

#remake: cleaner all

#clean:
#	$(RM) $(OBJECTDIR)

#cleaner: clean
#	$(RM) $(TARGETDIR)

$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<

$(MAIN): $(OBJ)
	$(CXX) -o $(MAIN) $^ $(LIB)

.PHONY: all remake clean cleaner
