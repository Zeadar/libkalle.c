CC = cc
LDFLAGS = -shared
CFLAGS = -fPIC

SRCS = slice.c sarray.c hashy.c
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))

OBJDIR = build
TARGET = $(OBJDIR)/libmemhandle.so

all: debug

debug: CFLAGS += -Wextra -Wall -g -O0
debug: $(OBJDIR) $(TARGET)

release: CFLAGS += -O3 -march=native
release: $(OBJDIR) $(TARGET)
	strip $(TARGET)
	rm -v $(OBJDIR)/*.o

clean:
	rm -frv $(OBJDIR)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -pv $(OBJDIR)

