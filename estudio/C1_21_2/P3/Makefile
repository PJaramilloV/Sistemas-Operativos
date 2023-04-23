TEST=test
PROB=nReservar

SYS=n
OPT=-g
VAR=-g

LDLIBS=
LIBNSYS= lib/lib$(SYS)Sys$(VAR).a
SYSDIR= ../npsystem

CFLAGS= -Wall -Werror -pedantic -std=c18 -I$(SYSDIR)/include
MAK= make --no-print-directory

SHELL=bash -o pipefail

readme:
	less README.txt

bin:
	@$(MAK) VAR= "OPT=-O -DNSYSTEM -DOPTIMIZED" SYS=n LDLIBS= lib test.nbin

bin-g:
	@$(MAK) SYS=n "OPT=-g -DNSYSTEM" LDLIBS= lib test.nbin-g

ddd: bin-g
	@gdbserver --version >& /dev/null || ( echo "Instale gdbserver con: sudo apt-get install gdbserver"; false)
	ddd --command launch.gdb test.nbin-g &
	gdbserver :2345 test.nbin-g

mem:
	@$(MAK) VAR=-mg "OPT=-fsanitize=address -g -DVALGRIND -DNSYSTEM" SYS=n LDLIBS= lib test.nbin-mg

ddd-mem: mem
	@gdbserver --version >& /dev/null || ( echo "Instale gdbserver con: sudo apt-get install gdbserver"; false)
	ddd --command launch.gdb test.nbin-mg &
	gdbserver :2345 test.nbin-mg

lib:
	@echo "<<< Construccion de $(SYSDIR)/$(LIBNSYS) >>>"
	@cd $(SYSDIR); $(MAK) "SYS=$(SYS)" "OPT=$(OPT)" "VAR=$(VAR)" $(LIBNSYS)
	@echo "<<< Construccion terminada >>>"

thread:
	@echo "Sanitize no funciona para detectar dataraces con nSystem."
	@echo "Solo funciona con pSystem."

resultados:
	@$(MAK) clean
	rm -f resultados.txt
	@echo === make bin === | tee -a resultados.txt
	@$(MAK) bin |& tee -a resultados.txt	
	./test.nbin |& tee -a resultados.txt
	@echo === make mem === | tee -a resultados.txt
	@$(MAK) mem |& tee -a resultados.txt	
	./test.nbin-mg |& tee -a resultados.txt
	@echo "Entregue por u-cursos el archivo $(PROB).c"

$(TEST).$(SYS)bin$(VAR): $(TEST).c $(SYSDIR)/include/nSystem.h $(SYSDIR)/$(LIBNSYS)
	$(CC) $(OPT) $(CFLAGS) $(TEST).c $(SYSDIR)/$(LIBNSYS) $(LDLIBS) -o $@
	@echo "Invoke this command to run the executable:" ./$@

clean:
	rm -f *.o *.nbin* core
	@cd $(SYSDIR) ; make clean
