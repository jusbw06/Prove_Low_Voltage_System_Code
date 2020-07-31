CC = gcc
CFLAGS = -Wall -pedantic -g
MAIN = serial_read processData manager motorGo
all : $(MAIN)

serial_read : serialReader.o
	$(CC) $(CFLAGS) -o serial_read serialReader.o

processData : processData.o
	$(CC) $(CFLAGS) -o processData processData.o

manager : manager.o
	$(CC) $(CFLAGS) -o manager manager.o

motorGo : motorGo.o
	$(CC) $(CFLAGS) -o motorGo motorGo.o -lwiringPi

serialReader.o : serialReader.c
	$(CC) $(CFLAGS) -c serialReader.c

processData.o : processData.c
	$(CC) $(CFLAGS) -c processData.c

manager.o : manager.c
	$(CC) $(CFLAGS) -c manager.c

motorGo.o : motorGo.c
	$(CC) $(CFLAGS) -c motorGo.c -lwiringPi

clean :
	rm *.o $(MAIN)
