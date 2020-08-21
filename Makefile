CXX = g++
STDVERSION = -std=c++17 -lstdc++fs
WARNINGS = -pedantic -Wall -Wfatal-errors -Wextra -Wno-unused-parameter -Wno-unused-variable


TARGET = VMTranslator
OBJECTS = code_writer.o parser.o vutility.o

all:$(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(WARNINGS) -o $(TARGET) src/Main.cpp $(OBJECTS) $(STDVERSION)

vutility.o: src/vutility.cpp include/vutility.hpp 
	$(CXX) $(WARNINGS) $(STDVERSION) -c src/vutility.cpp -o vutility.o

parser.o: src/parser.cpp include/parser.hpp 
	$(CXX) $(WARNINGS) $(STDVERSION) -c src/parser.cpp -o parser.o

code_writer.o: src/code_writer.cpp include/code_writer.hpp
	$(CXX) $(WARNINGS) $(STDVERSION) -c src/code_writer.cpp -o code_writer.o


clean:
	rm -rf *.o $(TARGET) *.asm 