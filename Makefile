
CXXFLAGS = -std=c++11 -thread
SOURCES = $(shell find . -name "*.cpp" -o -name "*.cc")
OBJS = $(SOURCES:.cpp,.cc=.o)

gcoded: $(OBJS)
	g++ -std=c++11 -o gcoded $(OBJS) -lrt $(shell pkg-config --libs protobuf)

clean:
	rm -f *.o
	rm -f gcoded
