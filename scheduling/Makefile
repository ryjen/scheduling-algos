CC = gcc
CFLAGS = -I. -std=c11 -ggdb -W -Wall -Wvla -Werror -pedantic

DEPS = scheduler.h process.h types.h queue.h algorithm.h
LIBS = -lpthread

PROGS = fcfs str spn rr lottery mlfq
TEST = test

ODIR = obj

_PROG_OBJS = scheduler.o process.o queue.o algorithm.o
PROG_OBJS = $(patsubst %,$(ODIR)/%,$(_PROG_OBJS))

_TEST_OBJS = test.o queue_test.o $(_PROG_OBJS)
TEST_OBJS = $(patsubst %,$(ODIR)/%,$(_TEST_OBJS))

all: $(ODIR) $(PROGS) $(TEST)

help:
	@echo "Commands: all help init $(PROG) $(TEST) clean"

$(ODIR):
	@[ -d $(ODIR) ] || mkdir -p $(ODIR)

fcfs: $(ODIR)/fcfs.o $(PROG_OBJS)
	@echo "Linking $@"
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

spn: $(ODIR)/spn.o $(PROG_OBJS)
	@echo "Linking $@"
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

str: $(ODIR)/str.o $(PROG_OBJS)
	@echo "Linking $@"
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

rr: $(ODIR)/rr.o $(PROG_OBJS)
	@echo "Linking $@"
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

lottery: $(ODIR)/lottery.o $(PROG_OBJS)
	@echo "Linking $@"
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

mlfq: $(ODIR)/mlfq.o $(PROG_OBJS)
	@echo "Linking $@"
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(TEST): $(TEST_OBJS)
	@echo "Linking $@"
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR)/%.o: %.c $(DEPS)
	@echo "Compiling $@"
	@$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	@rm -rf $(ODIR)
	@rm -f *~ core $(PROGS) $(TEST)
	@echo "Cleaned"

