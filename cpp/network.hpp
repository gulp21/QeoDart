/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#ifndef NET_HPP
#define NET_HPP

#include <QHostAddress>
#include <QProgressDialog>
#include <QTcpSocket>
#include <QTcpServer>
#include <QtNetwork>
#include <QWidget>

class dart;

class io;

const double dNetworkVersion=1.0;

class network : public QWidget {
	Q_OBJECT
	
	dart *myDart;
	io *myIO;
	
	public:
		network(dart*, io*);
		~network();
		
		int iNumberOfPlayers, iReceivedScores;
		
		void vNewNetworkGame();
		void vSendCommand(QString command);
		
 	private:
		bool bGameStarted, bExpectingImageData;
		
		QTcpServer *commandServer;
		QList<QTcpSocket*> qlCommandSockets;
		QProgressDialog *progressDialog;
		
		void vGameStartQuestion();
		void vSetServerOnline();
		void vServerTimeout();
		void vConnectToServer();
		void vShowProgressForServer();
		void vShowProgressForClient();
		void vDestroyProgressDialog();
		
		bool bAskForIp(QHostAddress &ip);
		
	private slots:
		void vReadCommand();
		void vNewClient();
		void vShowConnectionError(QAbstractSocket::SocketError socketError);
};
#endif //NET_HPP 
