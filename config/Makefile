all: defs.h.in configure

configure:
	autoconf
	mv configure ..

defs.h.in:
	autoheader
	mv defs.h.in ..


clean:
	rm -rf autom4te*
	
distclean: clean
