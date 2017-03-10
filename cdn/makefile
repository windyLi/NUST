CXX = g++

INC = -I/home/kawori/teambuilder-master/cdn/lib

CXXFLAGS = -std=c++11

all : cdn.o deploy.o io.o strlib.o graph.o route.o server.o
	$(CXX) -g -o cdn cdn.o deploy.o io.o strlib.o graph.o route.o server.o

cdn.o : cdn.cpp
	$(CXX) $(INC) -c -g cdn.cpp

deploy.o : deploy.cpp
	$(CXX) $(INC) $(CXXFLAGS) -c -g deploy.cpp

route.o : route.cpp
	$(CXX) $(CXXFLAGS) -c -g route.cpp

server.o : server.cpp
	$(CXX) $(CXXFLAGS) -c -g server.cpp

graph.o : graph.cpp
	$(CXX) $(CXXFLAGS) -c -g graph.cpp

io.o : io.cpp
	$(CXX) -c -g io.cpp

strlib.o : strlib.cc
	$(CXX) $(CXXFLAGS) -c -g strlib.cc

clean :
	rm -vf *.o cdn
