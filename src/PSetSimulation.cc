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

void PSetSimulation::run() {

	// Getting list of parameters 
	SymbolTable* listOfParameters = SymbolTable::getInstance();

	// Using parameter set
	for (auto const & parameter : parameter_set) {
		const Symbol * param = listOfParameters->getSymbol(parameter.first);

		if (param == NULL)
			throw BNException("parameter " + (parameter.first) + " not defined in the config file");

		listOfParameters->setSymbolValue(param, parameter.second);
	}

	// Running the simulation
	simulation = new MaBEstEngine(network, config);
	simulation->run(NULL);

}

const STATE_MAP<NetworkState_Impl, double> PSetSimulation::getLastStateDist() {
  return simulation->getLastStateDist();
}

const std::map<Node *, double> PSetSimulation::getLastNodeDist() {
  const STATE_MAP<NetworkState_Impl, double> last_state_dist = simulation->getLastStateDist();

  std::map<Node *, double> last_node_dist;
	const std::vector<Node*>& nodes = network->getNodes();

  for (const auto& node : nodes) {

      double dist = 0.0;
      for (const auto& pair : last_state_dist) {
        NetworkState state = pair.first;
        dist += pair.second * ((double) state.getNodeState(node));
      }
      last_node_dist[node] = dist;
  }

  return last_node_dist;
}

const double PSetSimulation::getLastNodeDist(std::string& label) {
  const STATE_MAP<NetworkState_Impl, double> last_state_dist = simulation->getLastStateDist();
  Node * node = network->getNode(label);

  double dist = 0.0;
  for (const auto& pair : last_state_dist) {
    NetworkState state = pair.first;
    dist += pair.second * ((double) state.getNodeState(node));
  }

  return dist;
}

void PSetSimulation::display(std::ostream & out) {

	const STATE_MAP<NetworkState_Impl, double> state_dist = simulation->getLastStateDist();

	for (const auto& pair: state_dist) {
		NetworkState state = pair.first;
		double proba = pair.second;

		out << "State : ";
		state.displayOneLine(out, network);
    out << " = " << proba << std::endl;
	}

}