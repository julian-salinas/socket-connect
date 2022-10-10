all:
	-cd src && $(MAKE) all

clean:
	-cd src && $(MAKE) clean

debug:
	-cd src && $(MAKE) debug

test: all
	-cd tests/unit-tests && $(MAKE) test

install: test
	-cd src && $(MAKE) install

uninstall:
	-cd src && $(MAKE) uninstall