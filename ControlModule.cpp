#include "ControlModule.h"


void print_instruction() {
	std::cout << "---------- Console instruction ----------\n"
		<< "In random mode corresponding commands are:\n"
		<< "\t* <PARAMETER_NAME>=<VALUE> - change parameters\n"
		<< "\t* apply  - apply changes\n"
		<< "\t* switch - switch to the reading mode\n"
		<< "In reading mode corresponding commands are:\n"
		<< "\t* switch - switch to the random mode\n"
		<< "Any other form of input would be conidered as invalid\n" << std::endl;
	std::cout << "---------- List of parameters ----------\n"
		<< "R - generating time(milliseconds)\n"
		<< "T - receiving time(milliseconds)\n"
		<< "M - max array length\n"
		<< "N - max number of blocks\n" << std::endl;
}

ControlModule::ControlModule(SharedParameters* ptr_params) {
	if (ptr_params != nullptr) {
		ptr_parameters = ptr_params;
	}

	print_instruction();
};

ControlModule::~ControlModule() {
};


void ControlModule::processInput(Generator& g) {
	auto isAccessible = checkIfAccessible(g);
	if (isAccessible)
		processIfAccessible(g);
	else
		processIfInaccessible(g);
};

void ControlModule::processIfAccessible(Generator& g) {
	std::string temp_str = "";
	Parameters  temp_params = *ptr_parameters;
	std::cout << prompt;
	while (std::cin >> temp_str) {
		if (temp_str[1] == '=') {
			switch (temp_str[0]) {
			case 'R': {
				//а если меньше 0 время?
				temp_params.R = atoi(&((temp_str.c_str())[2]));
				std::cin.clear();
				break;
			}
			case 'T': {
				temp_params.T = atoi(&((temp_str.c_str())[2]));
				std::cin.clear();
				break;
			}
			case 'M': {
				temp_params.M = atoi(&((temp_str.c_str())[2]));
				std::cin.clear();
				break;
			}
			case 'N': {
				// а если больше size_t?
				temp_params.N = atoi(&((temp_str.c_str())[2]));
				std::cin.clear();
				break;
			}
			default: {
				std::cerr << "Invalid name of parameter, try again" << std::endl;
				break;
			}
			}
		}
		else if (temp_str == "apply") {
			std::lock_guard<std::mutex> locker(ptr_parameters->mu_param);
			*ptr_parameters = temp_params;
		}
		else if (temp_str == "switch") {
			g.switchMode();
			processInput(g);
		}
		else if (temp_str == "exit") {
			//std::terminate();
		}
		else {
			std::cerr << "Invalid input\n";
			std::cin.clear();
		}
		std::cout << prompt;
	}
};

void ControlModule::processIfInaccessible(Generator& g) {
	std::string temp_str = "";
	Parameters  temp_params = *ptr_parameters;
	std::cout << prompt;
	while (std::cin >> temp_str) {
		if (temp_str == "switch") {
			g.switchMode();
			//std::cout << "switch completed, beginning processing\n";
			processInput(g);
		}
		else if (temp_str == "exit") {
			//std::terminate();
		}
		else {
			std::cerr << "Invalid command\n";
		}
		std::cout << prompt;
	}
};

bool ControlModule::checkIfAccessible(Generator& g) {
	return g.canChangeParameters();
};