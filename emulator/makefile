CC = gcc
CFLAGS = -Wall -Werror -Isrc -g -D_DEBUG
SRCDIR = src
OBJDIR = bin
TARGET = bin/scap-emu

SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

$(TARGET): $(OBJECTS)
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJDIR)/*.o $(TARGET)
