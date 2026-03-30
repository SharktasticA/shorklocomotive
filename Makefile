CC ?= gcc
AR ?= ar
RANLIB ?= ranlib
STRIP ?= strip

CFLAGS += -I.
LDFLAGS += -static

SRC = main.c

sl: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o sl $(LDFLAGS)
	$(STRIP) sl

PREFIX ?= /usr
BINDIR = $(PREFIX)/bin

install: sl
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 sl $(DESTDIR)$(BINDIR)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/sl

clean:
	rm -f sl

.PHONY: install uninstall clean
