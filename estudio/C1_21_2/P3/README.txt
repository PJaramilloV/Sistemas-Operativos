=============================================================
Esta es la documentación para compilar y ejecutar su solución
=============================================================

Se está ejecutando el comando: less README.txt

***************************
*** Para salir: tecla q ***
***************************

Para avanzar a una nueva página: tecla <page down>
Para retroceder a la página anterior: tecla <page up>
Para avanzar una sola línea: tecla <enter>
Para buscar un texto: tecla / seguido del texto (/...texto...)
         por ejemplo: /ddd

-----------------------------------------------

Instrucciones para resolver esta pregunta

Cree el archivo nReservar.c en el directorio ../npsystem/nsrc
con la solución de esta pregunta.  Ya se incluye una plantilla en
../nsystem/nsrc/nReservar.c.plantilla. nReservar.c es el archivo que
entregará por U-cursos.  Estas son las 2 maneras de compilar su solución
con nSystem (no se puede probar con pSystem):

make bin : compilación optimizada
make mem : compilación para verificar el uso correcto de la memoria

La ejecución de los binarios resultantes de estas compilaciones lo
felicitará si se aprobaron todos los tests.  Recuerde que el test
de su tarea es recomendable, pero no obligatorio.  Dadas las restricciones
de tiempo, si falla se otorgará puntaje de acuerdo a lo logrado en su código.

La metodología de depuración es idéntica a la de la tarea 3.  Para
lanzar ddd:

make ddd

o

make ddd-mem

Como primer paso, en la ventana de comandos de ddd, ponga un breakpoint
en la función exit con este comando:

b exit

Luego ejecute con el botón cont.

Si falla uno de los tests, la ejecución se detendrá justo después del
test que falló.  Vaya al menu Status de ddd y seleccione Backtrace para abrir
una ventana con las funciones en ejecución.  Seleccione alguna de estas
funciones para determinar que sucedió.

**** No necesita seguir leyendo a continuación. ***

**** No necesita seguir leyendo a continuación. ***

**** No necesita seguir leyendo a continuación. ***

(ya debería haberlo leído para la tarea 3.)


=== Explicación sobre gdbserver para depurar con ddd  ===

Si no presenta problemas de manejo de memoria lance ddd ejecutando en el
terminal el comando:

     make ddd

Este comando lanza ddd para depurar test.bin-g en modo remoto.  Esto se
debe a que si se usa ddd en el modo normal, no aparecen todos los nPrintf,
por culpa de un bug de ddd.  Con gdb sí aparecen todos los nPrintf.
En el modo remoto, los nPrintf aparecen en el mismo terminal en
donde ejecutó el comando make ddd, no en la ventana de ddd.

Para depurar en modo remoto, debe instalar gdbserver con:

     sudo apt-get install gdbserver

Cuando aparece la ventana de ddd, el programa ya se inició pero se encuentra
detenido a la entrada de la función nMain.  Coloque breakpoints si los necesita
y luego continúe la ejecución con el comando cont en la ventana de ddd.

Si falla uno de los tests, la ejecución se detendrá justo después del
test que falló.  Vaya al menu Status de ddd y seleccione Backtrace para abrir
una ventana con las funciones en ejecución.  Seleccione alguna de estas
funciones para determinar que sucedió.

Es normal que ddd abra una ventana de diálogo para reclamar porque no
encuentra los fuentes de exit.  Ignore el problema presionando el
botón OK y continúe.

La desventaja de depurar en modo remoto es que deberá relanzar ddd cada
vez que necesite reiniciar la ejecución.  Solo puede lanzar una ejecución
de ddd en modo remoto a la vez.  Si make ddd arroja el error:

     Can't bind address: Address already in use

es porque ya hay un gdbserver lanzado ocupando el port 2345.  Si no hay
ninguna ventana de ddd asociada a la ejecución de ese gdbserver, Ud.
puede terminar ese proceso ejecutando el comando:

     ps aux | grep gdbserver

Si la salida muestra una línea como:

     pss 4019 0.0 0.1 11624 4032 pts/3 S 18:24 0:00 gdbserver :2345 test.nbin-g

Termine el proceso con el comando:

     kill -9 4019

El número 4019 es la identificación del proceso: el pid o process identifier.

También puede lanzar con ddd la compilación que verifica el uso correcto
de la memoria.  Para ello ejecute en el terminal el comando:

     make ddd-mg

Al terminar la ejecución con sanitize desde ddd, aparece este mensaje:

     LeakSanitizer has encountered a fatal error.

No le haga caso.  Es porque sanitize tiene dificultades para correr bajo gdb.

=== Entrega ===

Los archivos que debe entregar son:

resultados.txt : generado con el comando make resultados
nDisk.c : su solución de la parte c

make resultados ejecutará automáticamente los binarios para las 3
compilaciones.  Todas las ejecuciones deben terminar con felicitaciones.

-----------------------------------------------

Limpieza de archivos

make clean: hace limpieza borrando todos los archivos que se pueden volver
            a reconstruir a partir de los fuentes: *.o *.bin* *.a etc.

-----------------------------------------------

Acerca de sanitize

En esta tarea se usa el compilador gcc con la opción -fsanitize=address
para verificar el uso correcto de la memoria.  Es similar a valgrind pero
con ayuda del compilador se logra detectar una variedad más amplia de errores.

Tambien se usa gcc con la opción -fsanitize=thread para detectar
dataraces.  Es similar a valgrind con drd u otros, pero más eficiente.

-----------------------------------------------

Acerca del comando make

El comando make sirve para automatizar el proceso de compilación asegurando
recompilar el archivo binario ejecutable cuando cambió uno de los archivos
fuentes de los cuales depende.

La especificación de los archivos de los cuales depende está en el archivo
Makefile, que es muy complicado de entender.  Es el profesor el que los
elabora.

Durante la invocación, make muestra los comandos que está ejecutando,
junto a otros mensajes.  Ud.  puede invocar manualmente estos comandos
en el terminal y obtendrá el mismo resultado, si los invoca en el
mismo orden.

A veces es útil usar make con la opción -n para que solo muestre
exactamente qué comandos va a ejecutar, sin ejecutarlos de verdad.
Por ejemplo en la tarea 1 la invocación de: make clean; make -n partec-mem
mostrará:

  make --no-print-directory VAR=-mg "OPT=-fsanitize=address -g -DVALGRIND" lib test-disco.pbin-mg

O sea se invoca a sí mismo (recursivamente) pero con otras opciones para
detallar mejor qué trabajo se debe hacer.  La pregunta es qué va a hacer
esta invocación recursiva.  Para ello basta usar nuavemente la opción -n,
es decir invocar el comando: make -n --no-print-directory VAR=-mg "OPT=-fsanitize=address -g -DVALGRIND" lib test-disco.pbin-mg

que mostrará:

  echo "<<< Construccion de npsystem/lib/libpSys-mg.a >>>"
  cd npsystem; make --no-print-directory "SYS=p" "OPT=-fsanitize=address -g -DVALGRIND" "VAR=-mg" lib/libpSys-mg.a
  echo "<<< Construccion terminada >>>"
  cc -fsanitize=address -g -DVALGRIND -Wall -Werror -pedantic -std=c18 -Inpsystem/include discoq.c test.c npsystem/lib/libpSys-mg.a -pthread -o test-discoq.pbin-mg
  echo "Invoke this command to run the executable:" ./test-discoq.pbin-mg
  
¿Qué comandos usaría para determinar que hace make --no-print-directory "SYS=p" "OPT=-fsanitize=address -g -DVALGRIND" "VAR=-mg" lib/libpSys-mg.a?
 
===

También es útil usar make con la opción -B para forzar la recompilación
de los fuentes a pesar de que no han cambiado desde la última compilación.
Por ejemplo:

make -B partec-mem

Recompilará todo lo que corresponde a psystem-mem.
Si a continuacion invoca:

make partec-mem

No recompilará nada porque los fuentes no han cambiado desde la última
compilación.  Eso es útil para no recompilar el nSystem cuando no es necesario.

Si ahora invoca:

make -B partec-mem

Se recompilará todo nuevamente, producto de la opción -B, a pesar de que
los fuentes no han cambiado.
