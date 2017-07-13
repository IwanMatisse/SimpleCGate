# SimpleCGate 
===========

is a sample of implementation of a software application for access to the SPECTRA market using the P2 CGate library (the Moscow Exchange: the Derivatives Market).   


## Key features:

* The dictionary of instruments which are traded in a specific trading session (futures and calendar spreads).
* Receiving of the best ask and bid prices.
* Receiving current positions, trades, orders, a variational margin and the information on funds, limits.
* Trading operations: sending, moving and deleting orders.
* Ready to add the user trading logic.
* Ready to connect with the extern GUI.

---

## Main components:

*PlazaConnector*  – provides an interaction with the P2MQRouter: connection, receiving data, sending messages and orders.

*GateDatabase* – contains dictionaries of instruments, positions, orders and trades and provides thread-safe access to them. 

*StrategyManager* – controls the pool of trade strategies.

*DataServer* – provides an interaction with the GUI (the GUI is not included in this application).

*EventsManager* – provides communication between all the other parts. It is a "message dispatcher".



The project was created in the Visual Studio 2015.

The CGate installation link: ftp://ftp.moex.com/pub/FORTS/Plaza2/CGate/
The documentation link: ftp://ftp.moex.com/pub/FORTS/Plaza2/docs/
To get a test login, please, send an email to the technical support of the Moscow Exchange: help@moex.com

---