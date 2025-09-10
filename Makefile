CC = gcc
CFLAGS = -Wextra -Wall -g -O0 -fPIC 
LDFLAGS = -shared

SRCS = slice.c sarray.c hashy.c
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))

OBJDIR = build
TARGET = $(OBJDIR)/libkalle.so

all: $(OBJDIR) $(TARGET)

clean:
	rm -fr $(OBJDIR)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

