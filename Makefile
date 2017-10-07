.DEFAULT_GOAL=all

CC = clang++
GDB = gdb
CFLAGS = -c -O2 -std=c++14 -Wall -MMD # -ggdb
BUILD_DIR = build
SRCS = $(filter-out ./build/%.cc,$(shell find . -name "*.cc"))
# $(info SRCS:$(SRCS))
OBJS = $(patsubst ./%.cc,$(BUILD_DIR)/%.o,$(SRCS))
BIN = $(BUILD_DIR)/jsonor
ARGS = -y c11.y

.PHONY: all gdb run rebuild clean

all: $(OBJS)
	@$(CC) $^ -o $(BIN)
	@echo "Generate ==> $(BIN)"

$(BUILD_DIR)/%.o: %.cc
	@echo "+ $<"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $< -o $@

gdb: all
	$(GDB) --args ./$(BIN) $(ARGS)

run: all
	@./$(BIN) $(ARGS)

rebuild: clean all

DEPS = $(OBJS:.o=.d)
-include $(DEPS) 

clean:
	rm -rf $(BUILD_DIR)
