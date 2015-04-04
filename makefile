TARGET = AMR
CC = gcc -std=c99 -c
override CFLAGS += -Wall
#CFLAGS = $(CFLAGS) -Wall -I.
LINKER   = gcc -o

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONEY: clean
clean:
	@$(rm) $(OBJECTS)
	@$(rm) amr.c
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"

onefile:
	awk 'FNR==1{print ""}1' $(SRCDIR)/user.h > AMR.c
	awk 'FNR==1{print ""}1' $(SRCDIR)/*.h >> AMR.c
	awk 'FNR==1{print ""}1' $(SRCDIR)/*.c >> AMR.c
	sed -i '/#include \".*/d' ./AMR.c
