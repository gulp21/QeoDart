/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#ifndef NET_HPP
#define NET_HPP

#include <QHostAddress>
#include <QWidget>

class dart;

class network : public QWidget {
	Q_OBJECT
	
	dart *myDart;
	
	public:
		network(dart*);
		~network();
		
		int iNumberOfPlayers;
		
		void vNewNetworkGame();
 	private:
		void vGameStartQuestion();
		void vAskForServer();
};
#endif //NET_HPP 
