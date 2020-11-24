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
     PSetSimulation.cc     
     Authors:
     Vincent Noel <contact@vincent-noel.fr>
     
     Date:
     Dec 2018
*/
#include "PSetSimulation.h"
#include <algorithm>    // std::max

void PSetSimulation::run() {

	// Getting list of parameters 
	SymbolTable* listOfParameters = network->getSymbolTable();
    
	// Using parameter set
	for (auto const & parameter : parameter_set) {

    // Parameter
    if (parameter.first.rfind("$", 0) == 0){
      const Symbol * param = listOfParameters->getSymbol(parameter.first);

      if (param == NULL)
        throw BNException("parameter " + (parameter.first) + " not defined in the config file");

      listOfParameters->setSymbolValue(param, parameter.second);
    }

    // Initial values
    else {
      Node * node = network->getNode(parameter.first);
      IStateGroup::setNodeProba(network, node, parameter.second); 
    }
	}

	// Running the simulation
	simulation = new MaBEstEngine(network, config);
	simulation->run(NULL);
}
const std::vector<Node *> PSetSimulation::getNodes() const {
  return network->getNodes();
}

const std::map<double, std::map<Node *, double> > PSetSimulation::getNodesDists() const {
  return simulation->getNodesDists();
}

const std::map<double, STATE_MAP<NetworkState_Impl, double> > PSetSimulation::getStatesDists() const {
  return simulation->getStateDists();
}


const STATE_MAP<NetworkState_Impl, double> PSetSimulation::getNthStateDist(int nn) {
  return simulation->getNthStateDist(nn);
}

const std::map<Node *, double> PSetSimulation::getNthNodesDist(int nn) {
  return simulation->getNthNodesDist(nn);
}

const std::map<Node *, double> PSetSimulation::getFirstNodesDist() {
  return getNthNodesDist(0);
}

const std::map<Node *, double> PSetSimulation::getLastNodesDist() {
  return getNthNodesDist(simulation->getMaxTickIndex());

}

const std::map<Node *, double> PSetSimulation::getMaxNodesDist() {
  std::map<Node*, double> result;

  const std::map<double, std::map<Node *, double>> node_dist = simulation->getNodesDists();

  std::map<double, std::map<Node *, double>>::const_iterator begin = node_dist.begin();
  std::map<double, std::map<Node *, double>>::const_iterator end = node_dist.end();

  // double max = 0;
  while(begin != end) {

    std::map<Node *, double>::const_iterator nodes_begin = begin->second.begin();
    std::map<Node *, double>::const_iterator nodes_end = begin->second.end();

    while(nodes_begin != nodes_end) {

      // First round, we need to initialise the result
      if (result.find(nodes_begin->first) == result.end()) {
        result[nodes_begin->first] = 0;
      }

      result[nodes_begin->first] = std::max(result[nodes_begin->first], nodes_begin->second);
      nodes_begin++;
    }

    begin++;
  }

  return result;
}

const double PSetSimulation::getFirstNodeDist(std::string& label) {
  return simulation->getNthNodeDist(network->getNode(label), 0);
}

const double PSetSimulation::getLastNodeDist(std::string& label) {
  
  return simulation->getNthNodeDist(network->getNode(label), simulation->getMaxTickIndex());
}

const double PSetSimulation::getMaxNodeDist(std::string& node_label) {
  
  double result = 0;
  const std::map<double, std::map<Node *, double>> node_dist = simulation->getNodesDists();

  std::map<double, std::map<Node *, double>>::const_iterator begin = node_dist.begin();
  std::map<double, std::map<Node *, double>>::const_iterator end = node_dist.end();

  Node * node = network->getNode(node_label);

  while(begin != end) {

    std::map<Node *, double> t_map = begin->second;
    result = std::max(result, t_map[node]);

    begin++;
  }

  return result;
}

const STATE_MAP<NetworkState_Impl, double> PSetSimulation::getLastStateDist() {
  return simulation->getNthStateDist(simulation->getMaxTickIndex());
}

void PSetSimulation::display(std::ostream & out) {

	const STATE_MAP<NetworkState_Impl, double> state_dist = simulation->getNthStateDist(simulation->getMaxTickIndex());

	for (const auto& pair: state_dist) {
		NetworkState state = pair.first;
		double proba = pair.second;

		out << "State : ";
		state.displayOneLine(out, network);
    out << " = " << proba << std::endl;
	}

}