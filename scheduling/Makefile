CC = gcc
CFLAGS = -I. -I../queue -std=c11 -ggdb -W -Wall -Wvla -Werror -pedantic -L../queue

DEPS = scheduler.h process.h types.h algorithm.h
LIBS = -lpthread -lqueue

PROGS = fcfs str spn rr lottery mlfq
TESTS = $(patsubst %, %.test, $(PROGS))
TEST_GENERATOR = generate-processes

ODIR = obj

_PROG_OBJS = scheduler.o process.o algorithm.o
PROG_OBJS = $(patsubst %,$(ODIR)/%,$(_PROG_OBJS))

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

test: $(PROGS) $(TESTS)

$(ODIR)/%.o: %.c $(DEPS)
	@echo "Compiling $@"
	@$(CC) -c -o $@ $< $(CFLAGS)

%.test: 
	@./$(TEST_GENERATOR) | ./$*.verify

.PHONY: clean

clean:
	@rm -rf $(ODIR)
	@rm -f *~ core $(PROGS) $(TEST)
	@echo "Cleaned"

