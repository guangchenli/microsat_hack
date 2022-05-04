SRC = src
TEST= test
BUILD = build
CFLAGS = -std=c99 -O2 -g2 -gdwarf
EXE = microsat+
INCLUDE = ./src
TEST_INCLUDE = ./test

OBJS = microsat.o main.o utils.o rm_heap.o

TEST_OBJS = munit.o

all: $(EXE)

dir:
	mkdir -p $(BUILD)

test: test_rm_heap
	./$(BUILD)/$<

test_rm_heap: rm_heap.o test_rm_heap.o munit.o
	$(CC) $(addprefix $(BUILD)/, $^) -I$(INCLUDE) -I$(TEST_INCLUDE) \
	$(CFLAGS) -o $(BUILD)/$@

%.o: $(TEST)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -I$(TEST_INCLUDE) -c \
	$< -o $(BUILD)/$@

%.o: $(SRC)/%.c dir
	$(CC) $(CFLAGS) -c $< -o $(BUILD)/$@

$(EXE): $(OBJS)
	$(CC) $(addprefix $(BUILD)/, $^) -o $(BUILD)/$(EXE)

clean:
	rm $(BUILD)/*

format:
	clang-format -style=llvm -i $(SRC)/*.c $(SRC)/*.h

.PHONY: clean dir format
