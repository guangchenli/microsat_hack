SRC = src
BUILD = build
CFLAGs = -std=c99 -O2
EXE = microsat+
INCLUDE = src

OBJS = microsat.o main.o utils.o

all: $(EXE)

dir:
	mkdir -p $(BUILD)

%.o: $(SRC)/%.c dir
	$(CC) $(CFLAGS) -c $< -o $(BUILD)/$@

$(EXE): $(OBJS)
	$(CC) $(addprefix $(BUILD)/, $^) -o $(BUILD)/$(EXE)

clean:
	rm $(BUILD)/*.o $(BUILD)/$(EXE)

format:
	clang-format -style=llvm -i $(SRC)/*.c $(SRC)/*.h

.PHONY: clean dir format
