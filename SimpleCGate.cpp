
/*
SimpleCGate is a sample of implementation of a software application for access to the SPECTRA market using the P2 CGate library (the Moscow Exchange: the Derivatives Market).
*/

#include "stdafx.h"

#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "DataServer.h"
#include "EventManager.h"
#include "PlazaConnector.h"
#include "StrategyManager.h"
#include "SampleBidAskStrategy.h"
#include "Database.h"
#include <functional>


int main()
{
	
	std::string c;
	simple_cgate::GateDatabase database; //contains all data from CGate
	{
		simple_cgate::EventManager eventsManager; //dispatcher for event messages (pattern "Mediator")
		{
			simple_cgate::StrategyManager algoManager(eventsManager, database);
			{
				simple_cgate::PlazaConnector plaza("./PlazaConfig.ini", eventsManager, database); //recieving and sending data from/to CGate, and connection control

				simple_cgate::DataServer dataServer(eventsManager, database); //transmit data to extern GUI, reciving control commands from GUI

				//Example of strategy's definition 
				auto sample_strategy = std::make_shared<simple_cgate::SampleBidAskStrategy>(database);
				sample_strategy->PostOrder = std::bind(&simple_cgate::PlazaConnector::AddOrder, &plaza, std::placeholders::_1); //link order's operations
				sample_strategy->MoveOrder = std::bind(&simple_cgate::PlazaConnector::MoveOrder, &plaza, std::placeholders::_1, std::placeholders::_2);
				sample_strategy->CancelOrder = std::bind(&simple_cgate::PlazaConnector::DeleteOrder, &plaza, std::placeholders::_1);
				sample_strategy->LoadConfig();
				algoManager.AddStrategy(sample_strategy, sample_strategy->GetUsedSecurity()); // registration in StrategyManager

				//plaza.Connect();


				std::cin >> c;
				plaza.Disconnect();
			}
		}
	}
	std::cin >> c;
	return 0;
	

}
