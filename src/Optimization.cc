#include "Optimization.h"
#include "PSetSimulation.h"

// Importing PLSA functions 

extern "C" SAType * InitPLSA();
extern "C" PArrPtr * InitPLSAParameters(int);
extern "C" PLSARes * runPLSA();

OptimStruct * optim_data;

double score() {

	std::map<std::string, double> parameter_set;

	int i=0;
	for (const auto& param_range : optim_data->params_ranges) {
		parameter_set[param_range.first] = optim_data->parameters[i];
		i++;
	}

	PSetSimulation * simulation = new PSetSimulation(optim_data->network_file, optim_data->config_file, parameter_set);
	simulation->run();

	double score = 0;
	for (const auto& kv : optim_data->objective) {
		std::string node_name = kv.first;
		double node_score = kv.second;

		score += pow(node_score - simulation->getLastNodeDist(node_name), 2);
	}

	delete simulation;
	return score;
}

Optimization::Optimization(const char * network_file, const char * config_file, std::map<std::string, double> objective, std::map<std::string, std::pair<double, double>> params_ranges, double lambda) {
	optim_data = new OptimStruct(
		network_file, config_file, 
		params_ranges, objective, 
		(double *) malloc(params_ranges.size()*sizeof(double)) 
	);

	sa = InitPLSA();
	sa->criterion = 1e-4;
	sa->lambda = lambda;
	sa->initial_moves = 100;
	sa->freeze_count = 5;
	sa->scoreFunction = &score;

	params = InitPLSAParameters(optim_data->params_ranges.size());

	int i=0;	
	for(const auto& param_range: params_ranges) {
		char * name = (char *) param_range.first.c_str();
		std::pair<double, double> range = param_range.second;
		// std::cout << "Param " << name << ": {" << range.first << ", " << range.second << "}" << std::endl;
		params->array[i] = (ParamList) { &(optim_data->parameters[i]), 1, (Range) {range.first,range.second}, 2, name};
		i++;
	}

}

void Optimization::run(std::ostream& output) {
	
	res = runPLSA();
	// output << "Parameters :" << std::endl;

	int i=0;
	for (const auto& param_range: optim_data->params_ranges) {
		output << param_range.first << ", " << optim_data->parameters[i] << std::endl;
		i++;
	}
	output << "Error, " << res->score << std::endl;
}

Optimization::~Optimization() {
	free(res->params);
	free(params->array);
	free(res);
}