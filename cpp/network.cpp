/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "dart.hpp"
#include "network.hpp"

using namespace std;

network::network(dart *TDart) : myDart(TDart) {
}
 
network::~network() {
}

void network::vNewNetworkGame() {
	iNumberOfPlayers=1;
	
	QMessageBox msgBox;
	QPushButton *serverButton = msgBox.addButton(tr("Player 1 (Server)"), QMessageBox::AcceptRole);
	QPushButton *otherPlayerButton = msgBox.addButton(tr("Other Player"), QMessageBox::AcceptRole);
	msgBox.setWindowTitle(tr("Start Network Game"));
	msgBox.setText(tr("Do you want to be player 1 or another player?"));
	msgBox.setInformativeText(tr("When you want to play with other players over the Internet, one player (player 1) has to be the so-called server. The settings and the map used during the game are determined by player 1."));
	
	msgBox.exec();
	
	if (msgBox.clickedButton()==serverButton) {
		vGameStartQuestion();
	} else if (msgBox.clickedButton()==otherPlayerButton) {
		vAskForServer();
	}
}

void network::vGameStartQuestion() {
	QMessageBox msgBox;
	QPushButton *startButton = msgBox.addButton(tr("Start Game"), QMessageBox::AcceptRole);
	msgBox.addButton(QMessageBox::Cancel);
	msgBox.setWindowTitle(tr("Waiting For Other Players"));
	msgBox.setText(tr("Do you want to start the game?"));
	msgBox.setInformativeText(tr("At the moment there are %n player(s)","",iNumberOfPlayers));
	
	msgBox.exec();
	
	//TODO
}

void network::vAskForServer() {
	bool ok=true;
	QHostAddress ip;
	while(ip.isNull() && ok) {
		ip = QInputDialog::getText(this, tr("Network Game"), tr("Please enter the IP address of player 1:"), QLineEdit::Normal, "192.168.0.1", &ok); // TODO load IP from setting
	}
	
	if(ok);
	
	//TODO
}
