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

Corrija el archivo buf.c de modo que funcione correctamente.  buf.c es
es el archivo que entregará por U-cursos.  Estas son las 3 maneras de
compilar su solución con pSystem (por razones de tiempo, no se probará
con nSystem):

make bin : compilación optimizada
make mem : compilación para verificar el uso correcto de la memoria
make thread : compilación para verificar la ausencia de dataraces

La ejecución de los binarios resultantes de estas compilaciones lo
felicitará si se aprobó el test.  Dado lo corta de esta solución,
si uno de las ejecuciones falla, lo más probable es que no reciba
puntaje en esta pregunta.

La metodología de depuración es idéntica a la de la tarea 2.  Para
lanzar ddd:

make ddd

o

make ddd-mem
make ddd-thread

Como primer paso, en la ventana de comandos de ddd, ponga un breakpoint
en la función exit con este comando:

b exit

Luego ejecute con el botón cont.

Si falla uno de los tests, la ejecución se detendrá justo después del
test que falló.  Vaya al menu Status de ddd y seleccione Backtrace para abrir
una ventana con las funciones en ejecución.  Seleccione alguna de estas
funciones para determinar que sucedió.
