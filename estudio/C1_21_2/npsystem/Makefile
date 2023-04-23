# Este Makefile solo funciona con Linux de 64 bits

VAR=-g
OPT=-g
SYS=p

CFLAGS= -Wall -Werror -pedantic -std=c18 -Iinclude

CSRCS= $(shell echo $(SYS)src/*.c)
COBJS= $(CSRCS:.c=.o)

nOBJS= $(COBJS:.o=$(VAR).o) nsrc/nStack-amd64.o
pOBJS= $(COBJS:.o=$(VAR).o)

OBJS= $($(SYS)OBJS)

LIBNSYS= lib/lib$(SYS)Sys$(VAR).a

default:
	@echo $(CSRCS)
	@echo $(COBJS)
	@echo $(OBJS)
	@echo $(LIBNSYS)

$(LIBNSYS): $(OBJS)
	rm -f $@
	ar r $@ $^
	ranlib $@

%$(VAR).o: %.c
	$(CC) $(OPT) -c $(CFLAGS) -D$(SYS)SYSTEM=1 $< -o $@

clean:
	rm -f nsrc/*.o psrc/*.o lib/*.a
