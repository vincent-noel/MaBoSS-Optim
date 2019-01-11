#include <stdio.h>
#include <string.h> 
#include <iostream>		// std::cout
#include <fstream>		// std::ofstream

#include "BooleanGrammar.h"
#include "RunConfig.h"
#include "MaBEstEngine.h"

// Importing PLSA types
#include "types.h"

class CellLine {
	public:

	std::map<std::string, double> conditions;
	std::map<std::string, double> objectives;
	std::map<std::string, std::string> objectives_types;

	CellLine(std::map<std::string, double> conditions, std::map<std::string, double> objectives, std::map<std::string, std::string> objectives_types) : 
		conditions(conditions), objectives(objectives), objectives_types(objectives_types) {}
};

class OptimParameter {
	public:

	std::string name;
	double 		lowerBound;
	double 		upperBound;
	double 		initialValue;
	int 		signicantDigits;

	OptimParameter(std::string name, double lowerBound, double upperBound, double initialValue, int signicantDigits) :
		name(name), lowerBound(lowerBound), upperBound(upperBound), initialValue(initialValue), signicantDigits(signicantDigits) {}
};

class OptimStruct {
public:
	const char * network_file;
	const char * config_file;

	const std::vector<CellLine *> cellLines;
	const std::vector<OptimParameter *> optimizationParameters;

	double * parameters;

	OptimStruct(const char * network_file, const char * config_file, const std::vector<CellLine *> cellLines, const std::vector<OptimParameter *> optimizationParameters, double * parameters) :
		network_file(network_file), config_file(config_file), cellLines(cellLines), optimizationParameters(optimizationParameters), parameters(parameters) {}
};

class Optimization {

	SAType * sa;
	PArrPtr * params;
	PLSARes * res;

	long newSeed();

public:

	Optimization(const char *, const char *, std::pair<std::vector<CellLine *>, std::vector<OptimParameter *>>, double);
	~Optimization();
	std::pair<double, std::map<std::string, double>> run();
	void displaySimulationsResults(std::ostream & out);
};

