
SOURCES = $(shell find . -name "*.cpp")
OBJS = $(SOURCES:.cpp=.o)

timer: $(OBJS)
	g++ -o timer $(OBJS) -lrt

clean:
	rm -f *.o
	rm -f timer
