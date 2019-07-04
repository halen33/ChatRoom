CC=g++
CFLAGS=-std=c++11
all:server_t.cpp  utility.h client.cpp 
	$(CC) $(CFLAGS) server_t.cpp utility.h  -lpthread -o app
	$(CC) $(CFLAGS) client.cpp  utility.h  -lpthread -o ap
clean:
	rm -f *.o app ap


