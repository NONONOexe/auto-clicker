CC       = gcc
CFLAGS   = -Wall
LDFLAGS  = -mwindows -lcomctl32

TARGET   = autoclicker.exe

SRCS     = gui.c click.c lang.c settings.c hotkey.c clicker.c ui.c
BUILDDIR = build
OBJS     = $(SRCS:%.c=$(BUILDDIR)/%.o)
DEPS     = $(OBJS:.o=.d)

# Main target
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files
$(BUILDDIR)/%.o: %.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILDDIR):
	mkdir $(BUILDDIR)

# Include dependency files
-include $(DEPS)

# Clean
.PHONY: clean
clean:
	-rmdir /S /Q $(BUILDDIR) 2>NUL
	-del /Q $(TARGET) 2>NUL
