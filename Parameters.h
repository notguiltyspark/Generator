#pragma once

#include <mutex>
#include <thread>
#include <future>
#include <deque>

class SharedParameters;

struct Parameters{

	int R = 10;
	int T = 10;
	size_t M = 4;
	size_t N = 2;
	
};

class SharedParameters : public Parameters {
public:
	std::mutex mu_param;
	std::mutex mu_run;
	
	SharedParameters& operator=(const Parameters& params) {
		R = params.R;
		T = params.T;
		M = params.M;
		N = params.N;
		return *this;
	}
};
/*
struct DataProtection {
	std::mutex mu_pipe;
	std::mutex mu_param;
	std::condition_variable cond_pipe;
};
*/

class CommunicationPipe {
public:
	std::deque<int>         pipe;
	std::mutex              mu_pipe;
	std::condition_variable cond_pipe;
};