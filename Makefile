LIBS =
CFLAGS =  -Wall -D_GNU_SOURCE -DDEBUG
CSTR =

Ctg_Src =	*.cpp

Ctg_Hdr = 	*.h

Ctg_Obj = $(Ctg_Src:.c=.o)

CP = /bin/cp
MKDIR = /bin/mkdir
RM = /bin/rm
SED = /bin/sed
TAR = /bin/tar
CC = /usr/bin/gcc
SED = /usr/bin/sed
all: ctg_exporter .depend Makefile

ctg_exporter: .depend $(Ctg_Obj) $(Ctg_Hdr)
	$(CC) $(CFLAGS) $(LIBS) -o ctg_exporter $(Ctg_Obj)

.depend: Makefile
	$(CC) -MM $(CSTR) $(Ctg_Src) | $(SED) 's/:/: Makefile/g' >$@

clean:
	$(RM) -f *.o ctg_exporter .depend
