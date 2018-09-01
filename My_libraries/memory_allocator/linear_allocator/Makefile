.PHONY: clean

OBJECTS = main.o linear_allocator.o

all: $(OBJECTS)
	g++ $(OBJECTS) -o program -O2 -pipe
main.o: main.cpp linear_allocator.h
	g++ -c main.cpp -o main.o -O2 -pipe
linear_allocator.o: linear_allocator.cpp linear_allocator.h
	g++ -c linear_allocator.cpp -o linear_allocator.o -O2 -pipe
clean:
	rm -rf $(OBJECTS) program
