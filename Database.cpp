#include "Database.h"



Database::Database(SharedParameters* ptr_param){
	if (ptr_param != nullptr) {
		ptr_parameters = ptr_param;
	}
	else
		throw;
}


void Database::processSequence(CommunicationPipe& comm) {
	int val = 0;
	int sz = 0;

	std::ofstream outputFile(writeFilename);
	if (outputFile.fail()) {
		std::cerr << "Can't open output file" << std::endl;
		exit(1);
	}
	std::chrono::steady_clock clock;

	auto time_start = clock.now();
	auto time_point = time_start;

	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(time_start - time_point);

	while (true) {
		
		std::unique_lock<std::mutex> locker(comm.mu_pipe);
		
			comm.cond_pipe.wait(locker, [&]() { return !comm.pipe.empty(); });
			val = comm.pipe.front();
			comm.pipe.pop_front();
			sz = comm.pipe.size(); //needed?
		
		locker.unlock();
		
		updateParameters();
		data.push_back(val);
		
		if (data.size() == parameters_state.M) {
			dtb.push_back(data);
			data.clear();
			time_start = clock.now();
		}
		else {
			time_point = clock.now();
			time_span = time_point - time_start;
			if (time_span.count() >= parameters_state.T) {
				dtb.push_back(data);
				data.clear();
				time_start = clock.now();
			}
		}

		if (dtb.size() == ptr_parameters->N) {
			for (const auto& i : dtb[0])
				outputFile << i << " ";

			outputFile << "\nParameters: R=" << ptr_parameters->R
				<< " T=" << ptr_parameters->T << " M=" << ptr_parameters->M
				<< " N=" << ptr_parameters->N << "\nTime: \n" 
				<< "******************************************" << std::endl;
			
			dtb.pop_front();
		}
	}
}

void Database::updateParameters() {
	std::lock_guard<std::mutex> locker(ptr_parameters->mu_param);
	parameters_state = *ptr_parameters;
}

Database::~Database(){
	ptr_parameters = nullptr;
}
