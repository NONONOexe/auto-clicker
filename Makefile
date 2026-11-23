CC       = gcc
WINDRES  = windres
CFLAGS   = -Wall -D_WIN32_WINNT=0x0A00
LDFLAGS  = -mwindows -lcomctl32 -ldwmapi

TARGET   = autoclicker.exe

SRCS     = gui.c click.c lang.c settings.c hotkey.c clicker.c ui.c capture.c
RES      = app.rc
BUILDDIR = build
OBJS     = $(SRCS:%.c=$(BUILDDIR)/%.o) $(RES:%.rc=$(BUILDDIR)/%.o)
DEPS     = $(OBJS:.o=.d)

# Main target
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files
$(BUILDDIR)/%.o: %.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILDDIR)/%.o: %.rc | $(BUILDDIR)
	$(WINDRES) $< -O coff -o $@

$(BUILDDIR)/app.o: app.manifest

$(BUILDDIR):
	mkdir $(BUILDDIR)

# Include dependency files
-include $(DEPS)

# Clean
.PHONY: clean
clean:
	-rmdir /S /Q $(BUILDDIR) 2>NUL
	-del /Q $(TARGET) 2>NUL
