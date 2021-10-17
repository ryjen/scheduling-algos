CC = gcc
AR = ar
CFLAGS = -I. -std=c11 -ggdb -W -Wall -Wvla -Werror -pedantic

DEPS = queue.h
LIBS = 

BINARY = libqueue.a
TEST = test

ODIR = obj

_BIN_OBJS = queue.o
BIN_OBJS = $(patsubst %,$(ODIR)/%,$(_BIN_OBJS))

_TEST_OBJS = test.o queue_test.o $(_BIN_OBJS)
TEST_OBJS = $(patsubst %,$(ODIR)/%,$(_TEST_OBJS))

.PHONY: clean test help 

all: $(ODIR) $(BINARY) $(TEST)

help:
	@echo "Commands: all help init $(BINARY) $(TEST) clean"

$(ODIR):
	@[ -d $(ODIR) ] || mkdir -p $(ODIR)

$(BINARY): $(BIN_OBJS)
	@echo "Linking $@"
	@$(AR) -r $@ $^

$(TEST): $(TEST_OBJS)
	@echo "Linking $@"
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
	@[ ! -x $@ ] || ./$@

$(ODIR)/%.o: %.c $(DEPS)
	@echo "Compiling $@"
	@$(CC) -c -o $@ $< $(CFLAGS)

clean:
	@rm -rf $(ODIR)
	@rm -f *~ core $(BINARY) $(TEST)
	@echo "Cleaned"

