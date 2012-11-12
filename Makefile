LIBS =
CFLAGS =  -Wall -D_GNU_SOURCE -std=c99 -DDEBUG
CSTR =

Ctg_Src =	main.c ctg_helper.c \

Ctg_Hdr = 	ctg_helper.h

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

.depend: Makefile $(Ctg_Reader_Hdr)
	$(CC) -MM $(CSTR) $(Ctg_Src) | $(SED) 's/:/: Makefile/g' >$@

clean:
	$(RM) -f *.o ctg_exporter .depend
