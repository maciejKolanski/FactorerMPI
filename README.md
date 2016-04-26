# FactorerMPI
## LINKOWANE BIBLIOTEKI
gmp mpich mysqlcppconn-static SCIEZKA/libmysqlclient.a dl z m pthread cr

Do stworzenia projektu wykorzystano CodeBlocks 16.01

Aby zainstalować MPICH na Ubuntu:

sudo apt-get install libcr-dev mpich2 mpich2-doc

W codeblocks stworzyć projekt, wybrać Settings->Compiler->Toolchain executable

C compiler mpicc.mpich2
C++ compiler mpicxx.mpich2
Linker for dynamic mpicxx.mpich2

pliki mpicxx.mpich2 i mpicc.mpich2 powinny być /usr/bin/

Jakby coś się nie kompilowało, to pytajcie. Na widnowsach też pewnie można, ale to już sami ogarnijcie, jak zaisntalować mpi.

Do gita użyłem SmartGit - w celach niekomercyjnych darmowy klient.
