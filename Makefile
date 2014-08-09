CPPFLAGS += -Wall

TARGET = test1
all:: $(TARGET)

test1: test1.o

test1.o: test1.cpp block.cpp block.h

clean::
	-rm -rf *.o $(TARGET)

