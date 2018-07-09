CC = gcc
CFLAGS = -I. -std=c11 -ggdb -W -Wall -Wvla -Werror -pedantic

DEPS = scheduler.h process.h types.h queue.h
LIBS = -lpthread

PROG = main
TEST = test

ODIR = obj

_PROG_OBJS = scheduler.o process.o queue.o fcfs.o
PROG_OBJS = $(patsubst %,$(ODIR)/%,$(_PROG_OBJS))

all: init $(PROG) $(TEST)

help:
	@echo "Commands: all help init $(PROG) $(TEST) clean"

init:
	@mkdir -p $(ODIR)

$(PROG): $(PROG_OBJS)
	@echo "Linking $@"
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(TEST):
	@bats tests

$(ODIR)/%.o: %.c $(DEPS)
	@echo "Compiling $@"
	@$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	@rm -rf $(ODIR)
	@rm -f *~ core $(PROG) $(TEST)
	@echo "Cleaned"

