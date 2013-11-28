
CXXFLAGS = -std=c++11 -g3 -O0
SOURCES += $(shell find . -maxdepth 1 -name "*.cpp")
OBJS = mill.pb.o
OBJS += $(SOURCES:.cpp=.o)
LIBOBJS = $(filter-out ./main.o, $(OBJS))

%.pb.cc: %.proto
	protoc --cpp_out=. $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

libcncd.a: $(LIBOBJS)
	ar rcs $@ $^

gcoded: $(OBJS)
	g++ $(CXXFLAGS) -o gcoded $^ -lrt $(shell pkg-config --libs protobuf)

clean:
	rm -f *.o
	rm -f gcoded
	rm -f libcncd.a
	rm -f mill.pb.*
