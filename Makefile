.PHONY: all run clean


CC=clang

all: main.out

run: all
	LD_PRELOAD="/c/ITMO/parallel/distributed-computing-master/src/pa2/libruntime.so" ./main.out -p 5 10 10 10 10 10

clean:
	rm main.out

main.out: main.c ipc.c bank_robbery.c auxiliary.c c_process.c k_process.c balance.c pipes.c utils.c
	$(CC) -o $@ -std=c99 -Wall -pedantic -g -O0 $^ $(CFLAGS)-L. -lruntime

%.c: