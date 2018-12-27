#include <stdio.h>
#include <string.h> 
#include <iostream>		// std::cout
#include <fstream>		// std::ofstream

#include <jsoncpp/json/json.h>
#include "fast-cpp-csv-parser/csv.h"

#include "MaBEstEngine.h"
#include "Optimization.h"


const char* prog = "MaBoSSFitting";

static int usage(std::ostream& os = std::cerr)
{
  os << "\nUsage:\n\n";
  os << "  " << prog << " [-h|--help]\n\n";
  os << "  " << prog << " -c|--config CONF_FILE -p PARAMS_RANGES_FILE -f OBJECTIVE_FILE [-l|--lambda LAMBDA_VALUE] BOOLEAN_NETWORK_FILE\n\n";
  return 1;
}

std::map<std::string, std::pair<double, double>> readParamsRangesCSV(const char * ranges_filename) {
	
	std::map<std::string, std::pair<double, double>> ranges;

	io::CSVReader<3> in(ranges_filename);

	std::string name;
	double min; 
	double max;

	while(in.read_row(name, min, max)) {
		std::pair<double, double> range = {min, max};
		ranges[name] = range;
	}

	return ranges;
}


std::map<std::string, std::pair<double, double>> readParamsRangesJSON(const char * ranges_filename) {
	
	std::map<std::string, std::pair<double, double>> ranges;

	std::ifstream ranges_file(ranges_filename);

    Json::Reader reader;
    Json::Value obj;

	reader.parse(ranges_file, obj);

	for (const auto& param_range: obj["params_ranges"]) {
		std::string param_name = param_range["name"].asString();
		std::pair<double, double> range = {param_range["min"].asDouble(), param_range["max"].asDouble()};	
		ranges[param_name] = range; 
	}

	ranges_file.close();
	return ranges;
}

std::map<std::string, double> readObjectiveCSV(const char * objective_filename) {
	
	std::map<std::string, double> objective;
	io::CSVReader<2> in(objective_filename);

	std::string name;
	double prob;

	while(in.read_row(name, prob)) {
		objective[name] = prob;
	}

	return objective;
}

std::map<std::string, double> readObjectiveJSON(const char * objective_filename) {
	
	std::map<std::string, double> objective;

	std::ifstream objective_file(objective_filename);

 	Json::Reader reader;
    Json::Value obj;

	reader.parse(objective_file, obj);

	for (const auto& node_objective: obj["objective"]) {
		std::string node_name = node_objective["node"].asString();
		double node_proba = node_objective["proba"].asDouble();	
		objective[node_name] = node_proba; 
	}

	objective_file.close();
	return objective;
}

const char * getExtension(const char * filename) {

	std::string filename_str = std::string(filename);

	std::string::size_type idx = filename_str.rfind('.');

	if(idx != std::string::npos)
	{
		return filename_str.substr(idx+1).c_str();
	}
	else
	{
		return "";
	}
}

int main(int argc, const char * argv[])
{
	const char * config_file = NULL;
	const char * network_file = NULL;
	const char * result_file = NULL;
	std::map<std::string, std::pair<double, double>> params_ranges;
	std::map<std::string, double> objective;

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

			} else if (!strcmp(s, "-p") || !strcmp(s, "--parameters-ranges")) {
				if (nn == argc-1) {
					std::cerr << '\n' << prog << ": missing value after option " << s << '\n'; return usage();
				}

				const char * ranges_filename = argv[++nn];
				if (!strcmp(getExtension(ranges_filename), "csv")) {
					params_ranges = readParamsRangesCSV(ranges_filename);

				} else if (!strcmp(getExtension(ranges_filename), "json")) {
					params_ranges = readParamsRangesJSON(ranges_filename);
				}

			} else if (!strcmp(s, "-f") || !strcmp(s, "--fit-objective")) {
				if (nn == argc-1) {
					std::cerr << '\n' << prog << ": missing value after option " << s << '\n'; return usage();
				}

				const char * objective_filename = argv[++nn];
				if (!strcmp(getExtension(objective_filename), "csv")) {
					objective = readObjectiveCSV(objective_filename);

				} else if (!strcmp(getExtension(objective_filename), "json")) {
					objective = readObjectiveJSON(objective_filename);
				}

			} else if (!strcmp(s, "-l") || !strcmp(s, "--lambda")) {
				if (nn == argc-1) {
					std::cerr << '\n' << prog << ": missing value after option " << s << '\n'; return usage();
				}

				lambda = atof(argv[++nn]);

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

	if (network_file != NULL && config_file != NULL && objective.size() > 0 && params_ranges.size() > 0) {
		try {
			
			MaBEstEngine::init();
			Optimization * optim = new Optimization(network_file, config_file, objective, params_ranges, lambda);
			
			if (result_file == NULL)
				optim->run(std::cout);
			else {
				std::ofstream output;
				output.open(result_file, std::ofstream::out);
				optim->run(output);
				output.close();
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
