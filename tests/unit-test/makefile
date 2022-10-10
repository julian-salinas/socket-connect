RM=rm -rf
CC=gcc

BASE_DIR=../..

C_SPEC=$(BASE_DIR)/cspec
C_SPEC_BIN=$(C_SPEC)/release
C_SPEC_SO=$(C_SPEC_BIN)/libcspecs.so

CONNECT=$(BASE_DIR)/src
CONNECT_BIN=$(CONNECT)/build
CONNECT_SO=$(CONNECT_BIN)/libconnect.so
CONNECT_SRCS=$(patsubst ./%,$(CONNECT)/%,$(shell make --no-print-directory -C $(CONNECT) sources)) 

C_SRCS=$(shell find . -iname "*.c" | tr '\n' ' ')
OBJS=$(C_SRCS:./%.c=build/%.o)

BIN_DIR=build
BIN=$(BIN_DIR)/connect-unit-test

all: $(BIN)

$(BIN_DIR):
	mkdir -p $@

$(BIN): $(C_SPEC_SO) $(CONNECT_SO) $(BIN_DIR) $(OBJS)
	$(CC) -L"$(CONNECT_BIN)" -L"$(C_SPEC_BIN)" -o "$@" $(OBJS) -lconnect -lcspecs -lpthread

build/%.o: ./%.c
	$(CC) -I"$(CONNECT)" -I"$(C_SPEC)" -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

debug: CC += -DDEBUG -g
debug: all

clean:
	$(RM) build

test: all
	LD_LIBRARY_PATH="$(COMMONS_BIN):$(C_SPEC_BIN)" ./$(BIN)

$(CONNECT_SO): $(CONNECT_SRCS)
	-cd $(COMMONS) && $(MAKE) all

$(C_SPEC_SO):
	-cd $(BASE_DIR) && git submodule init && git submodule update
	-cd $(C_SPEC) && $(MAKE) all

valgrind: debug
	LD_LIBRARY_PATH="$(CONNECT_BIN)/:$(C_SPEC_BIN)" valgrind --error-exitcode=42 --leak-check=full -v ./$(BIN)

.PHONY: all debug clean test valgrind