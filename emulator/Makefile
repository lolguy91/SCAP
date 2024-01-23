CC = gcc
CFLAGS = -Wall -Werror -Isrc -g 
SRCDIR = src
OBJDIR = bin
TARGET = bin/scap-emu
INSTALL_DIR = /usr/local/bin

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

install: $(TARGET)
	@sudo mkdir -p $(INSTALL_DIR)
	@sudo ln -sf $(abspath $(TARGET)) $(INSTALL_DIR)/scap-emu
