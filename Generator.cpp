#include "Generator.h"

/* -------- GeneratorMode ------------- */

std::vector<GeneratorMode::id_size> GeneratorMode::ids;

bool GeneratorMode::setModeId(id_size id_) {
	for (const auto& i : ids) {
		if (i == id)
			return false;
	}
	id = id_;
	ids.push_back(id_);
	return true;
};

/* -------- RandomMode ------------ */

RandomMode::RandomMode(id_size id, SharedParameters* ptr_params){
	if (setModeId(id) != true) {
		throw InvalidId{};
	}

	if (ptr_params != nullptr ) {
		max_wait_duration = (decltype(max_wait_duration))ptr_params->R;
		ptr_parameters = ptr_params;
	}
	else
		throw; //???????

}

RandomMode::~RandomMode() {
	ptr_parameters = nullptr;
}

int RandomMode::generateValue() {
	static int i = 0;
	updateParameters();
	/* 
		...RANDOM time implementation
	*/
	std::this_thread::sleep_for(max_wait_duration);
	i++;
	return i;
}

void RandomMode::generateSequence(CommunicationPipe& comm) {
	int temp = 0;
	{
		std::lock_guard<std::mutex> locker(ptr_parameters->mu_run);
		isGenerating = true;
	}
	while (isGenerating) {
		temp = generateValue();
		std::unique_lock<std::mutex> locker(comm.mu_pipe); 
		comm.pipe.push_back(temp);
		locker.unlock();
		comm.cond_pipe.notify_one();  
	}
}

void RandomMode::updateParameters(){
	std::lock_guard<std::mutex> locker(ptr_parameters->mu_param);
	max_wait_duration = (decltype(max_wait_duration))ptr_parameters->R ;
}

void RandomMode::stopGenerating() {
	std::lock_guard<std::mutex> locker(ptr_parameters->mu_run);
	isGenerating = false;
}


/* --------- ReadingMode ---------- */

ReadingMode::ReadingMode(id_size id, SharedParameters* ptr_params, const std::string& readFile) : readFilename{readFile} {
	if (ptr_params != nullptr)
		ptr_parameters = ptr_params;
	else
		throw; //throw what?
	
	if (setModeId(id) != true) {
		throw InvalidId{};
	};

	std::ifstream inputStartParameters(readFilename);
	if (inputStartParameters.fail()) {
		std::cerr << "Can't open input file" << std::endl;
		exit(1);
	}
	
	int temp = 0;
	inputStartParameters >> temp;
	max_wait_duration = (decltype(max_wait_duration))temp;
	inputStartParameters.close();
}

int ReadingMode::generateValue() {
	static int i = 0;
	// RANDOM VALUE ?
	std::this_thread::sleep_for(std::chrono::milliseconds(max_wait_duration));
	i--;
	return i;
}

void ReadingMode::generateSequence(CommunicationPipe& comm) {
	int temp = 0;
	{
		std::lock_guard<std::mutex> locker(ptr_parameters->mu_run);
		isGenerating = true;
	}
	while (isGenerating) {
		temp = generateValue();
		std::unique_lock<std::mutex> locker(comm.mu_pipe); //COMM instead of protection
		comm.pipe.push_back(temp);
		locker.unlock();
		comm.cond_pipe.notify_one();     //COMM instead of protection
	}
}

void ReadingMode::stopGenerating() {
	std::lock_guard<std::mutex> locker(ptr_parameters->mu_run);
	isGenerating = false;
}

ReadingMode::~ReadingMode() {
	ptr_parameters = nullptr;
}


// -------------- Generator ------------------

Generator::Generator(GeneratorMode& gm, SharedParameters* ptr_params) : curr_pos{ 0 }, num_modes{ 1 } {
	if (ptr_params != nullptr)
		ptr_parameters = ptr_params;
	else
		throw;

	for (int i = 0; i < generator_modes.size(); i++) {
		generator_modes[i] = nullptr;
	}
	
	generator_modes[0] = &gm;
};

Generator::~Generator() {
	ptr_parameters = nullptr;
};

void Generator::addMode(GeneratorMode& gm) {
	if (num_modes < generator_modes.size()) {
		num_modes++;
		generator_modes[num_modes - 1] = &gm;
	}
	else {
		std::cerr << "Generator's storage is full\n";
	}
}

void Generator::switchMode() {
	// REDO
	isSwitched = false;
	generator_modes[curr_pos]->stopGenerating();
	if (curr_pos == generator_modes.size() - 1)
		curr_pos = 0;
	else
		++curr_pos;
	isSwitched = true;
	cond_generate.notify_one();
}

void Generator::run(CommunicationPipe& pipe) {
	while (true) {
		generator_modes[curr_pos]->generateSequence(pipe);
		{
			std::unique_lock<std::mutex> locker(ptr_parameters->mu_run);
			cond_generate.wait(locker, [&]() {return isSwitched == true; });
			isSwitched = false;
		}
	}
};

bool Generator::canChangeParameters() {
	return generator_modes[curr_pos]->canChangeParameters();
}


