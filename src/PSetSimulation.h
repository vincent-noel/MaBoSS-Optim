/* 
   MaBoSS (Markov Boolean Stochastic Simulator)
   Copyright (C) 2011-2018 Institut Curie, 26 rue d'Ulm, Paris, France
   
   MaBoSS is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   MaBoSS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA 
*/

/*
   Module:
     PSetSimulation.h
     
     Authors:
     Vincent Noel <contact@vincent-noel.fr>
     
     Date:
     Dec 2018
*/

#include <stdio.h>
#include <string>
#include <vector>
#include "MaBEstEngine.h"
#include "RunConfig.h"
#include "BooleanGrammar.h"

class PSetSimulation {

  std::map<std::string, double> parameter_set;
  // const char * network_filename;
  // const char * config_filename;

  Network * network;
  RunConfig * config;

  MaBEstEngine * simulation;

public:

	PSetSimulation(const char * network_filename, const char * config_filename, std::map<std::string, double> parameter_set) : 
    parameter_set(parameter_set) {

    // Resetting old network variables
    // Probably should be done while we parse the network ?
    IStateGroup::reset();
    NodeDecl::reset();

		network = new Network();
		network->parse(network_filename);

		config = RunConfig::getInstance();
		config->parse(network, config_filename);

    IStateGroup::checkAndComplete(network);

	}

	~PSetSimulation() {
    delete simulation;
		delete network;
	};

  void run();
  void display(std::ostream & out);


  const STATE_MAP<NetworkState_Impl, double> getLastStateDist();
  const std::map<Node *, double> getLastNodeDist();
  const double getLastNodeDist(std::string& label);
};
