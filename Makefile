CPP=g++

CPPFLAGS= -I. -lIce -lIceStorm -lIceUtil -pthread -lvlc -std=c++11

all: sliceToCpp Server

# SERVER

Server: main.o server.o songDB.o
	$(CPP) $^ -o $@ $(CPPFLAGS)

main.o: main.cpp
	$(CPP) -c $< -o $@ $(CPPFLAGS)

server.o: server.cpp server.h
	$(CPP) -c $< -o $@ $(CPPFLAGS)

songDB.o: songDB.cpp songDB.hpp
	$(CPP) -c $< -o $@ $(CPPFLAGS)

run:
	./Server

# SLICE TO CODE

sliceToCpp: server.ice
	slice2cpp $<

cleanIce: server.h server.cpp
	rm -f $^

# GLACIER

runGlacier:
	glacier2router --Ice.Config=glacier2.cfg

# ICEBOX

runIceBox:
	icebox --Ice.Config=icebox.cfg

# CLEAN

clean:
	rm -f *.o

mrproper: clean cleanIce
	rm -f Server
