CXXFLAGS = -std=c++11 -g3 -O0 -I../
LDLIBS = ../libcncd.a -lrt $(shell pkg-config --libs protobuf)
TEST_SOURCES += $(shell find . -maxdepth 1 -name "*_test.cpp")
TEST_BINS += $(patsubst %.cpp,%,$(TEST_SOURCES))


all: $(TEST_BINS)

clean:
	rm -f *.o
	rm -f $(TEST_BINS)
