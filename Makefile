FLAGS=-Dgpio -lwiringPi

all: myServer myClient

myServer: myServer.c
	gcc myServer.c $(FLAGS) -o myServer -lopen62541

myClient: myClient.c
	gcc myClient.c -o myClient -lopen62541


clean:
	rm myServer
