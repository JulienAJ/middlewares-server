CPP=g++

CPPFLAGS= -I. -lIce -lIceUtil -pthread -lvlc -std=c++11

all: sliceToAll Server

# JAVA CLIENT

runClient:
	$(JAVA) -classpath $(JAVACLASSPATH) Client

# SERVER

Server: main.o server.o songDB.o
	$(CPP) $^ -o $@ $(CPPFLAGS)

main.o: main.cpp
	$(CPP) -c $< -o $@ $(CPPFLAGS)

server.o: server.cpp server.h
	$(CPP) -c $< -o $@ $(CPPFLAGS)

songDB.o: songDB.cpp songDB.hpp
	$(CPP) -c $< -o $@ $(CPPFLAGS)

runServer:
	./Server

# SLICE TO CODE

sliceToAll: sliceToCpp

sliceToCpp: server.ice
	slice2cpp $<

cleanIce: server.h server.cpp
	rm -f $^

# CLEAN

clean:
	rm -f *.o

mrproper: clean cleanIce
	rm -f Server
