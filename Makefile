PREFIX ?= /usr

PROGRAMS = fb_mandelbrot fb_rectangle fb_sierpinski

all: $(PROGRAMS)

fb_mandelbrot: fb_mandelbrot.c
fb_rectangle: fb_rectangle.c
fb_mandelbrot fb_rectangle:
	$(CC) -Wall $^ -o $@ $(CFLAGS) $(LDFLAGS)

fb_sierpinski: fb_sierpinski.c
	$(CC) -Wall $^ -o $@ -lm $(CFLAGS) $(LDFLAGS)

install: all
	install -d $(DESTDIR)$(PREFIX)/bin
	install $(PROGRAMS) $(DESTDIR)$(PREFIX)/bin

clean:
	$(RM) $(PROGRAMS)
