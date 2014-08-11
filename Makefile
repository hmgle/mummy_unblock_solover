CPPFLAGS += -Wall

TARGET = test1 test2
all:: $(TARGET)

test1: test1.o
	g++ $^ -o $@

test2: test2.o
	g++ $^ -o $@

test1.o: test1.cpp block.cpp block.h

test2.o: test2.cpp block.cpp block.h

clean::
	-rm -rf *.o $(TARGET)

