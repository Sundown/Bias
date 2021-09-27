TARGET = bias

PREFIX ?= /usr/local

.PHONY: all install uninstall clean

all: $(TARGET)

$(TARGET):
	$(CC) -Wall -W -pedantic -Ofast -std=c11 -lm $(shell find source/ -name "*.c") -o $@

run: bias
	./bias

install: $(TARGET)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)

clean:
	$(RM) $(TARGET)

uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/bin/$(TARGET)
