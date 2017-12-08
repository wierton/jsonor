.DEFAULT_GOAL=all

CC        = g++
GDB       = gdb
CFLAGS    = -c -ggdb -O2 -std=c++14 -Wall -MMD -Isrc/
BUILD_DIR = build
SRCS      = ./src/json.cc ./src/test.cc ./example.cc
OBJS      = $(patsubst ./%.cc,$(BUILD_DIR)/%.o,$(SRCS))
BIN       = $(BUILD_DIR)/jsonor


$(info "SRCS:$(SRCS)")

.PHONY: all test gdb run rebuild clean

test:all

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
