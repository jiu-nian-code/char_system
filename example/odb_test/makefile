CC = g++
CFLAGS = -std=c++11 -g -Wall
INCLUDES = -I/usr/include
LIBS = -lodb -lodb-mysql -lmysqlclient -lboost_system

all: main

main: main.cc person-odb.cxx
	$(CC) $(CFLAGS) $(INCLUDES) -o main main.cc person-odb.cxx $(LIBS)

clean:
	rm -f main