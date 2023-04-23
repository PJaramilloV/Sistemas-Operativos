target remote :2345
b nExitSystem
cond 1 rc!=0
b nMain
cont
