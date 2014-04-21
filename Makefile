# Directories to be created
BUILDDIR=build
TARGETDIR=bin

# Targets and flags
RELEASE=smartcaps
DEBUG=smartcaps-dbg
CFLAGS=-Wall
CFLAGS_RELEASE=-O2 -DNDEBUG=1
CFLAGS_DEBUG=

# Object and source files
OBJECTS=$(addprefix $(BUILDDIR)/, $(notdir $(SOURCES:.c=.o)))
SOURCES=src/smartcaps.c src/devices.c src/inject.c
# SOURCES=$(wildcard src/*.c)

# Targets
all: $(RELEASE)

.PHONY: install
install:
	@mkdir -p /usr/local/bin
	@install bin/smartcaps /usr/local/bin

.PHONY: release
release: $(RELEASE)

.PHONY: debug
debug: $(DEBUG)

.PHONY: clean
clean:
	@rm -rf $(BUILDDIR)	
	@rm -rf $(TARGETDIR)

# The real job
$(RELEASE): CFLAGS+=$(CFLAGS_RELEASE)
$(RELEASE): $(OBJECTS)
	@mkdir -p $(TARGETDIR)
	@echo $(CFLAGS)
	@cc -o $(TARGETDIR)/$(RELEASE) $(OBJECTS) 

$(DEBUG): CFLAGS+=$(CFLAGS_DEBUG)
$(DEBUG): $(OBJECTS)
	@mkdir -p $(TARGETDIR)
	@cc -o $(TARGETDIR)/$(DEBUG) $(OBJECTS)

$(BUILDDIR)/%.o : src/%.c
	@mkdir -p $(dir $@)
	@cc $(CFLAGS) -c $< -o $@
