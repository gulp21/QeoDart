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

const double NETWORKVERSION=1.0;
const int NETTIMEOUT=5000;

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
		void vStopNetworking();
		
 	private:
		bool bGameStarted, bExpectingImageData;
		
		QTcpServer *commandServer;
		QList<QTcpSocket*> qlCommandSockets;
		QProgressDialog *progressDialog;
                QTimer *deleteTimer;
		
		void vGameStartQuestion();
		void vSetServerOnline();
		void vServerTimeout();
		void vConnectToServer();
		void vShowProgress(QString message);
		void vDestroyProgressDialog();
		
		bool bAskForIp(QHostAddress &ip);
		
	private slots:
		void vProgressDialogCancelled();
		void vReadCommand();
		void vNewClient();
		void vShowConnectionError(QAbstractSocket::SocketError socketError);
		void vDeleteSockets();
};
#endif //NET_HPP 
