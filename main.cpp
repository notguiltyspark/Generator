

#include "Generator.h"
#include "Database.h"
#include "ControlModule.h"

#include <iostream>
#include <cmath>
#include <exception>
#include <memory>
#include <thread>
#include <future>


int main() {
	try {

		SharedParameters   prms;
		CommunicationPipe  comm;
		Database           db(&prms);
		ControlModule      cm(&prms);
		RandomMode	       rand_mode(1,  &prms);
		ReadingMode        read_mode(10, &prms);
		Generator          generator(rand_mode, &prms);
		
		generator.addMode(read_mode);
		
		std::thread t1(&Generator::run, &generator, std::ref(comm));
		std::thread t2(&Database::processSequence, &db, std::ref(comm));
		std::thread t3(&ControlModule::processInput, &cm,std::ref(generator));
		t1.join();
		t2.join();
		t3.join();
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	
	return 0;
}