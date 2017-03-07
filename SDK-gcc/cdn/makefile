INC = -I/home/kawori/下载/HUAWEI_Code_Craft_2017_Preliminary_Contest_Question_zh/SDK-gcc/cdn/lib

CXXFLAGS += -std=c++11

all : cdn.o deploy.o io.o strlib.o graph.o
	g++ -g -o cdn cdn.o deploy.o io.o strlib.o graph.o

cdn.o : cdn.cpp
	g++ $(INC) -c -g cdn.cpp

deploy.o : deploy.cpp
	g++ $(INC) $(CXXFLAGS) -c -g deploy.cpp

graph.o : graph.cpp
	g++ $(CXXFLAGS) -c -g graph.cpp

io.o : io.cpp
	g++ -c -g io.cpp

strlib.o : strlib.cc
	g++ $(CXXFLAGS) -c -g strlib.cc

clean :
	rm -vf *.o cdn
