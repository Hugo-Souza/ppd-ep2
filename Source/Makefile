CC=gcc

all: clean sequential pthread 

sequential:
	$(CC) laplace_seq.c -o laplace_seq -lm

pthread:
	$(CC) laplace_pth.c -o laplace_pth -lpthread -lm

clean:
	rm -f laplace_seq  laplace_pth 
