#include "Optimization.h"
#include "PSetSimulation.h"

// Importing PLSA functions 

extern "C" SAType * InitPLSA();
extern "C" PArrPtr * InitPLSAParameters(int);
extern "C" PLSARes * runPLSA();

OptimStruct * optim_data;

double score() {

	double score = 0;
	int i_cond = 0;
	for (const auto& cell_line : optim_data->cellLines) {

		std::map<std::string, double> parameter_set;
		
		// First, the parameters to "configure" the cell line : conditions
		for (const auto& param : cell_line->conditions)
			parameter_set[param.first] = param.second;

		// Second, the optimization parameters
		int i = 0;
		for (const auto& optim_param : optim_data->optimizationParameters) {
			parameter_set[optim_param->name] = optim_data->parameters[i];
			i++;
		}

		PSetSimulation * simulation = new PSetSimulation(optim_data->network_file, optim_data->config_file, parameter_set);
		simulation->run();

		double sub_score = 0;
		for (const auto& objective : cell_line->objectives) {
			std::string node_name = objective.first;
			double node_score = objective.second;
		
			// If type is informed, and it's max
			if (cell_line->objectives_types[node_name] == "max") {
				sub_score += pow(node_score - simulation->getMaxNodeDist(node_name), 2);

			// Else, we suppose it's last time point
			} else {
				sub_score += pow(node_score - simulation->getLastNodeDist(node_name), 2);

			}
		}
		score += sub_score;
		i_cond++;

		delete simulation;
	}

	return score;
}

Optimization::Optimization(const char * network_file, const char * config_file, std::pair<std::vector<CellLine *>, std::vector<OptimParameter *>> settings, double lambda) {

	optim_data = new OptimStruct(
		network_file, config_file, 
		settings.first, settings.second,
		(double *) malloc(settings.second.size()*sizeof(double)) 
	);

	sa = InitPLSA();
	sa->criterion = 1e-4;
	sa->lambda = lambda;
	sa->initial_moves = 100;
	sa->freeze_count = 5;
	sa->scoreFunction = &score;

	params = InitPLSAParameters(settings.second.size());

	int i=0;	
	for(const auto& param: settings.second) {
		char * name = (char *) param->name.c_str();
		params->array[i] = (ParamList) { &(optim_data->parameters[i]), param->initialValue, (Range) {param->lowerBound,param->upperBound}, param->signicantDigits, name};
		i++;
	}	

}


std::pair<double, std::map<std::string, double>> Optimization::run() {
	
	res = runPLSA();

	std::map<std::string, double> result_parameters;

	int i=0;
	for (const auto& param: optim_data->optimizationParameters) {
		result_parameters[param->name] = optim_data->parameters[i];
		i++;
	}

	return std::make_pair(res->score, result_parameters);
}

Optimization::~Optimization() {
	delete optim_data;
	free(res->params);
	free(res);
}