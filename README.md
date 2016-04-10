# FactorerMPI

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

# Zlecanie zadań

Aby zlecić zadanie (bruteforce) należy najpierw uruchomić aplikację serwera (komendą mpirun -np "liczba wątków" ./exec).
Kiedy połączenie zostanie nawiązane, należy z poziomu aplikacji klienta (Python) wpisać w konsoli komendę B_FORCE i zatwierdzić enterem.
Kiedy komenda zostanie przesłana, można wprowadzić liczbę (dowolną stałoprzecinkową) i znowu zatwierdzić enterem. Aby uzyskać wynik, należy wprowadzić komendę RESULT. Jeżeli klater będzie zajęty, zwróci informację BUSY, jeżeli nie wyświetli wynik.

Krótko:
- Uruchamiamy serwer MPI
- Uruchamiamy klienta Python

I dalej w konsoli kienta:

- Wpisujemy B_FORCE 
- Wpisujemy liczbę	
- Wpisujemy RESULT
- Aby zakończyć - wpisujemy QUIT.
