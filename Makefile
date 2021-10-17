
.PHONY: queue scheduling clean test

all:
	@echo "build - build demos"
	@echo "test - run tests"
	@echo "clean - clean builds"

build:
	@$(MAKE) -C queue
	@$(MAKE) -C scheduling 

clean:
	@$(MAKE) -C queue clean
	@$(MAKE) -C scheduling clean

test:
	@$(MAKE) -C queue test
	@$(MAKE) -C scheduling test
