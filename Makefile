#CFLAGS	= +cpm -Wall --list --c-code-in-asm  -DGCC
CFLAGS	= +cpm -Wall -DGCC
LINKOP	= +cpm -create-app -lmath48
DESTDIR = ~/HostFileBdos/c/
DESTDIR1 = /var/www/html
SUM = sum
CP = cp
INDENT = indent -kr
SUDO = sudo

all: cube

cube: cube.o liboled.o
	zcc $(LINKOP) -ocube cube.o liboled.o $(SNAP)

cube.o: cube.c
	date > date.h
	zcc $(CFLAGS) -c cube.c

liboled.o: liboled.c
	zcc $(CFLAGS) -c liboled.c

clean:
	$(RM) *.o *.err *.lis *.def *.lst *.sym *.exe *.COM  *.map cube

just:
	$(INDENT) cube.c
	$(INDENT) liboled.c 

scope:
	cscope

install:
	$(SUDO) $(CP) ./*.COM $(DESTDIR1)/. 
	$(CP) CUBE.COM $(DESTDIR)cube.com

check:
	$(SUM) *.COM

