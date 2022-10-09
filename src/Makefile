RM=rm -rf
CC=gcc

C_SRCS=$(shell find . -iname "*.c" | tr '\n' ' ')
H_SRCS=$(shell find . -iname "*.h" | tr '\n' ' ')
OBJS=$(C_SRCS:./%.c=build/%.o)

ifneq ($(shell id -un),root)
SUDO=sudo
endif

all: build/libconnect.so

build/connect:
	mkdir -p $@

build/libconnect.so: build/connect $(OBJS)
	$(CC) -shared -o "$@" $(OBJS)

build/%.o: %.c
	$(CC) -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

debug: CC += -DDEBUG -g
debug: all

sources:
	@echo $(C_SRCS) $(H_SRCS)

clean:
	$(RM) build

install: all
	$(SUDO) cp -u build/libconnect.so /usr/lib
	$(SUDO) cp --parents -u $(H_SRCS) /usr/include

uninstall:
	$(SUDO) rm -f /usr/lib/libconnect.so
	$(SUDO) rm -rf /usr/include/connect

.PHONY: all debug sources clean install uninstall