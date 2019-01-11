#include <stdio.h>
#include <string.h> 
#include <iostream>		// std::cout
#include <fstream>		// std::ofstream

#include <jsoncpp/json/json.h>

#include "MaBEstEngine.h"
#include "Optimization.h"

const char* prog = "MaBoSS-Optim";

static int usage(std::ostream& os = std::cerr)
{
  os << "\nUsage:\n\n";
  os << "  " << prog << " [-h|--help]\n\n";
  os << "  " << prog << " -c|--config CONF_FILE -s|--settings SETTINGS_FILE -o|--output RESULT_FILE [-r|--results-sim SIMULATION_RESULT_FILE ] [-l|--lambda LAMBDA_VALUE] BOOLEAN_NETWORK_FILE\n\n";
  return 1;
}

std::pair<std::vector<CellLine *>, std::vector<OptimParameter *>> readSettingsJSON(const char * settings_filename) {

	std::vector<CellLine *> cell_lines;
	std::vector<OptimParameter *> optim_parameters;


	std::ifstream settings_file(settings_filename);
    Json::Reader reader;
    Json::Value obj;

	reader.parse(settings_file, obj);

	for (const auto& cell_line: obj["cell_lines"]) {

		std::map<std::string, double> conditions;
		std::map<std::string, double> objectives;
		std::map<std::string, std::string> objectives_types;

		for (const auto& condition: cell_line["conditions"]) {
			std::string parameter_name = condition["name"].asString();
			double parameter_value = condition["value"].asDouble();

			conditions[parameter_name] = parameter_value;
		}

		for (const auto& objective: cell_line["objectives"]) {
			std::string objective_name = objective["name"].asString();
			double objective_value = objective["proba"].asDouble();
			
			std::string objective_type;
			if (objective.isMember("type"))
				objective_type = objective["type"].asString();
			else
				objective_type = std::string("end");
				
			objectives[objective_name] = objective_value;
			objectives_types[objective_name] = objective_type;
		}

		CellLine * cell_line_obj = new CellLine(conditions, objectives, objectives_types);
		cell_lines.push_back(cell_line_obj);
	}

	for (const auto& optim_param: obj["optimization_parameters"]) {

		std::string p_name = optim_param["name"].asString();
		double p_lb = optim_param["min"].asDouble();
		double p_ub = optim_param["max"].asDouble();

		double initial;
		if (optim_param.isMember("initial"))
			initial = optim_param["initial"].asDouble();
		else	
			initial = (p_lb+p_lb)/2.0;

		int digits = 3;
		if (optim_param.isMember("digits"))
			digits = optim_param["digits"].asInt();
		
		OptimParameter * parameter = new OptimParameter(p_name, p_lb, p_ub, initial, digits);
		optim_parameters.push_back(parameter);
	}

	settings_file.close();

	return std::make_pair(cell_lines, optim_parameters);
}

void writeResults(const char * result_file, std::pair<double, std::map<std::string, double>> result) {

	Json::Value root;
	root["error"] = result.first;

	Json::Value params;
	for (const auto& param: result.second) {
		Json::Value json_param;
		json_param["name"] = param.first;
		json_param["value"] = param.second;	

		params.append(json_param);
	}

	root["parameters"] = params;

	std::ofstream outputFileStream(result_file);
	outputFileStream << root;
}

int main(int argc, const char * argv[])
{
	const char * config_file = NULL;
	const char * network_file = NULL;
	const char * result_file = NULL;
	const char * result_sim_file = NULL;

	std::pair<std::vector<CellLine *>, std::vector<OptimParameter *>> optim_settings;

	double lambda = 1;

	// Parsing arguments
	for (int nn = 1; nn < argc; ++nn) {
		const char* s = argv[nn];
		
		if (s[0] == '-') {
			
			if (!strcmp(s, "--help") || !strcmp(s, "-h")) {
				return usage();
			
			} else if (!strcmp(s, "-c") || !strcmp(s, "--config")) {
				if (nn == argc-1) {
					std::cerr << '\n' << prog << ": missing value after option " << s << '\n'; return usage();
				}

				config_file = argv[++nn];
			
			} else if (!strcmp(s, "-o") || !strcmp(s, "--output")) {
				if (nn == argc-1) {
					std::cerr << '\n' << prog << ": missing value after option " << s << '\n'; return usage();
				}

				result_file = argv[++nn];

			} else if (!strcmp(s, "-s") || !strcmp(s, "--settings")) {
				if (nn == argc-1) {
					std::cerr << '\n' << prog << ": missing value after option " << s << '\n'; return usage();
				}

				const char * settings_filename = argv[++nn];
				optim_settings = readSettingsJSON(settings_filename);

			} else if (!strcmp(s, "-l") || !strcmp(s, "--lambda")) {
				if (nn == argc-1) {
					std::cerr << '\n' << prog << ": missing value after option " << s << '\n'; return usage();
				}

				lambda = atof(argv[++nn]);

			} else if (!strcmp(s, "-r") || !strcmp(s, "--results")) {
				if (nn == argc-1) {
					std::cerr << '\n' << prog << ": missing value after option " << s << '\n'; return usage();
				}

				result_sim_file = argv[++nn];

			} else {
				std::cerr << '\n' << prog << ": unknown option " << s << std::endl;
				return usage();
			}
		} else if (network_file == NULL) {
			network_file = argv[nn];
		} else {
			std::cerr << '\n' << prog << ": boolean network file is already set to " << network_file << " [" << s << "]" << std::endl;
			return usage();
		}
	}

	if (network_file != NULL && config_file != NULL && result_file != NULL){// && objective.size() > 0 && params_ranges.size() > 0) {
		try {
			
			MaBEstEngine::init();

			Optimization * optim;
			
			if (optim_settings.first.size() > 0 && optim_settings.first[0]->objectives.size() > 0 && optim_settings.second.size() > 0) {
				optim = new Optimization(network_file, config_file, optim_settings, lambda);
	
			} else {
				std::cerr << '\n' << prog << ": no optimization settings " << std::endl;
				return usage();
			}

			std::pair<double, std::map<std::string, double>> result = optim->run();
			writeResults(result_file, result);

			if (result_sim_file != NULL) {
				std::ofstream simResultsOutput(result_sim_file);
				optim->displaySimulationsResults(simResultsOutput);
				simResultsOutput.close();
			}

			delete optim;

			return 0;
		}

		catch(const BNException& e) {
			std::cerr << prog << ": " << e;
			return 1;
		}
	} else return usage();
}
