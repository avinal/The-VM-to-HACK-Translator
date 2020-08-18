CXX = g++
STDVERSION = -std=c++17
WARNINGS = -pedantic -Wall -Wfatal-errors -Wextra -Wno-unused-parameter -Wno-unused-variable


TARGET = VMTranslator
OBJ_DIR = build 
OBJECTS = code_writer.o parser.o vutility.o
TEST_OBJS = parser_test.o 
all:$(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(WARNINGS) $(STDVERSION) -o $(TARGET) Main.cpp $(OBJECTS)

vutility.o: vutility.cpp vutility.hpp 
	$(CXX) $(WARNINGS) $(STDVERSION) -c vutility.cpp -o vutility.o

parser.o: parser.cpp parser.hpp 
	$(CXX) $(WARNINGS) $(STDVERSION) -c parser.cpp -o parser.o

code_writer.o:code_writer.cpp code_writer.hpp
	$(CXX) $(STDVERSION) -c code_writer.cpp -o code_writer.o


test:
	$(CXX) $(STDVERSION) tests/*.cpp tests/catch/catch_main.cpp parser.cpp code_writer.cpp -o test

clean:
	rm -rf *.o $(TARGET) *.asm 