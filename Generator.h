#pragma once

#include "Parameters.h"
#include <vector>
#include <iostream>
#include <exception>
#include <string>
#include <chrono>
#include <array>
#include <fstream>

class GeneratorMode{
public:

	using id_size = short;
	// ----------------------------------------------------
	class InvalidId{
	};
	// ----------------------------------------------------
	
	virtual int  generateValue() = 0;
	virtual void generateSequence(CommunicationPipe& comm) = 0;
	virtual bool canChangeParameters() const = 0;
	virtual void stopGenerating() = 0;

	bool    setModeId(id_size id);
	id_size getModeId() { return id; };

protected:

	GeneratorMode() {};
	virtual ~GeneratorMode() {};

private:

	static  std::vector<id_size> ids;
	id_size id;

};

class RandomMode : public GeneratorMode {
public:

	RandomMode(id_size id, SharedParameters* ptr_params);
	~RandomMode(); 
	
	int  generateValue()                           override;
	void generateSequence(CommunicationPipe& comm) override;
	void stopGenerating()                          override;

	bool canChangeParameters() const override { return true; };

private:
	
	void updateParameters();

private:

	std::chrono::milliseconds  max_wait_duration;
	SharedParameters*          ptr_parameters;
	bool                       isGenerating;
};

class ReadingMode : public GeneratorMode {
public:

	ReadingMode(id_size id, SharedParameters* ptr_params, const std::string& filename = "Generator_StartParameters.txt");
	~ReadingMode();

	int  generateValue()                           override;
	void generateSequence(CommunicationPipe& comm) override;
	void stopGenerating()                          override;
	
	bool canChangeParameters() const override { return false; };

private:

	std::chrono::milliseconds  max_wait_duration;
	SharedParameters*          ptr_parameters;
	Parameters                 parameters_state;
	std::string                readFilename;
	bool                       isGenerating;

};


class Generator {
public:

	Generator(GeneratorMode& gm, SharedParameters* ptr_params);
	~Generator();

	void addMode(GeneratorMode& gm);
	void switchMode();
	void run(CommunicationPipe& pipe);
	bool canChangeParameters();

private:

	SharedParameters*            ptr_parameters;
	std::vector<GeneratorMode*>  generator_modes{ nullptr, nullptr };
    size_t                       curr_pos;
	size_t                       num_modes;
	std::condition_variable      cond_generate;
	bool                         isSwitched;

};
