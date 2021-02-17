FLAGS=-Dgpio -lwiringPi

all: myServer

myServer: myServer.c
	gcc myServer.c $(FLAGS) -o myServer -lopen62541

clean:
	rm myServer
