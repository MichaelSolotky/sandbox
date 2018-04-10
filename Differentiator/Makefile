.PHONY: clean

OBJECTS = main.o preprocessing.o make_tree.o differentiation.o

all: $(OBJECTS)
	g++ $(OBJECTS) -o program
%.o: %.cpp
	g++ -o $@ -c $<
clean:
	rm -rf $(OBJECTS) program
