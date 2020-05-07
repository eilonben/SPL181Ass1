PROG=bin/fs

SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(SOURCES:src/%.cpp=bin/%.o)

all: $(PROG)

$(PROG): $(OBJECTS)
	g++ -o $@ $^

bin/%.o: src/%.cpp
	g++ -g -Wall -std=c++11 -Iinclude -c -o $@ $^

clean:
	rm -f $(OBJECTS) $(PROG)
