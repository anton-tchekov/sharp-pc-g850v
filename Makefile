all: sendfile recvfile terminal

sendfile: sendfile.c
	gcc sendfile.c -Wall -Wextra -o sendfile

recvfile: recvfile.c
	gcc recvfile.c -Wall -Wextra -o recvfile

terminal: terminal.c
	gcc terminal.c -Wall -Wextra -o terminal -pthread -lncurses -lform

clean:
	rm -f sendfile recvfile
