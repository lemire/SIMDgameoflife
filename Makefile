
.SUFFIXES:
#
.SUFFIXES: .cpp .o .c .h


.PHONY: clean cleandist

CFLAGS =  -fPIC -std=c99 -O3  -Wall -Wextra -Wshadow -Wno-implicit-function-declaration  -march=native
LIBS= -lpng
EXECUTABLES=gameoflife

HEADER=include/basicautomata.h include/pnghelper.h

all: $(EXECUTABLES)

gameoflife:  src/gameoflife.c $(HEADER)
	$(CC) $(CFLAGS) -o gameoflife  src/gameoflife.c -Iinclude $(LIBS)

clean:
	rm -f $(EXECUTABLES)

cleandist:
	rm -f $(EXECUTABLES)
