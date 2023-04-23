
PROB=buf
HEADER=$(PROB)
TEST=test

SYS=p
OPT=-g
VAR=-g

LDLIBS=-pthread
LIBNSYS= lib/lib$(SYS)Sys$(VAR).a
SYSDIR= ../npsystem

CFLAGS= -Wall -Werror -pedantic -std=c18 -DINIT -I$(SYSDIR)/include
MAK= make --no-print-directory

SHELL=bash -o pipefail

readme:
	less README.txt

bin:
	@$(MAK) VAR= OPT=-O lib test-$(PROB).pbin

ddd:
	@$(MAK) lib test-$(PROB).pbin-g
	@gdbserver --version >& /dev/null || ( echo "Instale gdbserver con: sudo apt-get install gdbserver"; false)
	ddd --command launch.gdb test-$(PROB).pbin-g &
	gdbserver :2345 test-$(PROB).pbin-g

ddd-mem:
	@$(MAK) VAR=-mg "OPT=-fsanitize=address -g -DVALGRIND" lib test-$(PROB).pbin-mg
	@gdbserver --version >& /dev/null || ( echo "Instale gdbserver con: sudo apt-get install gdbserver"; false)
	ddd --command launch.gdb test-$(PROB).pbin-mg &
	gdbserver :2345 test-$(PROB).pbin-mg

mem:
	@$(MAK) VAR=-mg "OPT=-fsanitize=address -g -DVALGRIND" lib test-$(PROB).pbin-mg

thread:
	@$(MAK) VAR=-tg "OPT=-fsanitize=thread -g -DVALGRIND" lib test-$(PROB).pbin-tg

ddd-thread:
	@$(MAK) VAR=-tg "OPT=-fsanitize=thread -g -DVALGRIND" lib test-$(PROB).pbin-tg
	@gdbserver --version >& /dev/null || ( echo "Instale gdbserver con: sudo apt-get install gdbserver"; false)
	ddd --command launch.gdb test-$(PROB).pbin-tg &
	gdbserver :2345 test-$(PROB).pbin-tg

nsys-bin:
	@$(MAK) VAR= "OPT=-O -DNSYSTEM" SYS=n LDLIBS= lib test-$(PROB).nbin

nsys-ddd:
	@$(MAK) SYS=n "OPT=-g -DNSYSTEM" LDLIBS= lib test-$(PROB).nbin-g

nsys-mem:
	@$(MAK) VAR=-mg "OPT=-fsanitize=address -g -DVALGRIND -DNSYSTEM" SYS=n LDLIBS= lib test-$(PROB).nbin-mg

lib:
	@echo "<<< Construccion de $(SYSDIR)/$(LIBNSYS) >>>"
	@cd $(SYSDIR); $(MAK) "SYS=$(SYS)" "OPT=$(OPT)" "VAR=$(VAR)" $(LIBNSYS)
	@echo "<<< Construccion terminada >>>"

resultados:
	@$(MAK) clean
	rm -f resultados.txt
	@echo === make bin === | tee resultados.txt
	@$(MAK) bin |& tee -a resultados.txt	
	./test-$(PROB).pbin |& tee -a resultados.txt
	@echo === make mem === | tee -a resultados.txt
	@$(MAK) mem |& tee -a resultados.txt	
	./test-$(PROB).pbin-mg |& tee -a resultados.txt
	@echo === make thread === | tee -a resultados.txt
	@$(MAK) thread |& tee -a resultados.txt	
	./test-$(PROB).pbin-tg |& tee -a resultados.txt

$(TEST)-$(PROB).$(SYS)bin$(VAR): $(PROB).c $(TEST).c $(SYSDIR)/include/nSystem.h $(SYSDIR)/$(LIBNSYS)
	$(CC) $(OPT) $(CFLAGS) $(PROB).c $(TEST).c $(SYSDIR)/$(LIBNSYS) $(LDLIBS) -o $@
	@echo "Ejecute con este comando:" ./$@

clean:
	rm -f *.o *.nbin* *.pbin* core
	@cd $(SYSDIR) ; make clean
