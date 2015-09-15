PATH=C:\3DODev\bin;%PATH% 
del tetris.iso
make clean
make -f Makefile.3DO
pause
start /d "OperaFS" OperaFS.exe
