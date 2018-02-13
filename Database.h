#pragma once

#include "Parameters.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <deque>

class Database{
public:

	Database(SharedParameters* ptr_param);
	~Database();

	void processSequence(CommunicationPipe& comm);
	void updateParameters();

private:

	std::vector<int>              data;
	std::deque<std::vector<int>>  dtb;
	SharedParameters*             ptr_parameters;
	Parameters                    parameters_state;
	std::string                   writeFilename = "database.txt";

};

