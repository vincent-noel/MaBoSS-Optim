#include <stdio.h>
#include <string.h> 
#include <iostream>		// std::cout
#include <fstream>		// std::ofstream

#include "BooleanGrammar.h"
#include "RunConfig.h"
#include "MaBEstEngine.h"

// Importing PLSA types
#include "types.h"

class OptimStruct {
public:
	const char * network_file;
	const char * config_file;

	std::map<std::string, std::pair<double, double>> params_ranges;
	std::map<std::string, double> objective;

	double * parameters;

	OptimStruct(const char * network_file, const char *config_file, std::map<std::string, std::pair<double, double>> params_ranges, std::map<std::string, double> objective, double * parameters) :
		network_file(network_file), config_file(config_file), params_ranges(params_ranges), objective(objective), parameters(parameters) {}
};

class Optimization {


	SAType * sa;
	PArrPtr * params;
	PLSARes * res;

	long newSeed();
	// double score();

public:

	Optimization(const char *, const char *, std::map<std::string, double>, std::map<std::string, std::pair<double, double>>, double);
	~Optimization();
	void run(std::ostream& output);
};

