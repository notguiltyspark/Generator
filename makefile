abstract_system: 
	g++ -c -std=c++11 main.cpp Generator.cpp Database.cpp ControlModule.cpp
	g++ -std=c++11 main.o Generator.o Database.o ControlModule.o -o main
	rm -rf *.o
