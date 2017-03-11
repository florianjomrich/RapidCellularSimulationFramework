# RapidCellularSimulationFramework

The Rapid Cellular Simulation Framework (RACE) allows the easy creation of realisitic LTE network simulations for automotive and other mobile communication scenarios.
RACE therefore intelligently combines the capabilites of the network simulator ns3 with the traffic simulation tool SUMO. 
The celltower configuration in ns3 is based upon a data set representing the LTE celltower infrastructure of Canada provided by the Canadian government (http://sms-sgs.ic.gc.ca/eic/site/sms-sgs-prod.nsf/eng/h_00010.html).
RACE provides an easy to use graphical user interface which enables a highly simplified simulation setup and high customization possibilities.

RACE automatically configures all parameters required for the simulation in ns3 and therefore is a tool-set especially designed for rapid prototyping requirements.

Acknowledgements
If you enjoyed using RACE, please acknowledge us in your publication(s) by referring to:

@inproceedings{jomrich2017race,
author = {Jomrich, Florian and Wankhede, Tushar and Rueckelt, Tobias and Burgstahler, Daniel and B{\"{o}}hnstedt, Doreen and Steinmetz, Ralf},
booktitle = {In Proceedings of the International Conference on Networked Systems (NetSys)},
title = {{Rapid Cellular Network Simulation Framework for Automotive Scenarios (RACE Framework)}},
year = {2017},
month = {March.},
location = {Goettingen, Germany}
} 
