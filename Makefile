CC      = gcc
CFLAGS  = -Wall
LDFLAGS = -mwindows

TARGET  = autoclicker.exe

SRCS    = gui.c click.c lang.c settings.c
OBJS    = $(SRCS:.c=.o)
DEPS    = $(OBJS:.o=.d)

# Main target
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Include dependency files
-include $(OBJS:.o=.d)

# Clean
.PHONY: clean
clean:
	-del /Q *.o *.d $(TARGET) 2>NUL
