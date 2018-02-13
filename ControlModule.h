#pragma once

#include "Parameters.h"
#include "Generator.h"
#include <iostream>
#include <string>
#include <algorithm>

void print_instruction();

const std::string prompt = ">";

class ControlModule{
public:
	ControlModule(SharedParameters* ptr_params);
	~ControlModule();
	
	void processInput(Generator& g);

private:

	void processIfAccessible(Generator& g);
	void processIfInaccessible(Generator& g);
	bool checkIfAccessible(Generator& g);

private:

	SharedParameters* ptr_parameters;

};

