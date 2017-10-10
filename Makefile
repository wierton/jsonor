.DEFAULT_GOAL=all

CC        = g++
GDB       = gdb
CFLAGS    = -c -O2 -std=c++14 -Wall -MMD -Isrc/
BUILD_DIR = build
SRCS      = $(shell find ./src -name "*.cc")
OBJS      = $(patsubst ./%.cc,$(BUILD_DIR)/%.o,$(SRCS))
BIN       = $(BUILD_DIR)/jsonor

ifeq ($(test),true)
SRCS += ./test/test.cc
else
SRCS += ./example.cc
endif

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
