/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "dart.hpp"
#include "network.hpp"

using namespace std;

network::network(dart *TDart) : myDart(TDart) {
	commandServer=NULL;
	//commandSocket=null;
	
	bGameStarted=false;
	bExpectingImageData=false;
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
		vSetServerOnline();
	} else if (msgBox.clickedButton()==otherPlayerButton) {
		vConnectToServer();
	}
}

// server stuff

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

void network::vShowProgressForServer() {
	progressDialog=new QProgressDialog(tr("Waiting for connection request…"),tr("Cancel"),0,0,this);
	//connect(progressDialog, SIGNAL(canceled()), this, SLOT(vSinglePlayer())); TODO
	progressDialog->setWindowModality(Qt::WindowModal);
	progressDialog->show();
	
	//bNETProgressShown=TRUE;
	
	//dn->setServerOnline();
}

void network::vSetServerOnline() {
	if(commandServer==NULL) commandServer=new QTcpServer(this);
	commandServer->listen(QHostAddress::Any,51483);
	connect(commandServer, SIGNAL(newConnection()), this, SLOT(vNewClient()));
	
	qDebug() << "Server is online";
	
	vShowProgressForServer();
}

void network::vNewClient() {
	qDebug() << "new client request";
	
	if(bGameStarted) return;
	
	if(commandServer->hasPendingConnections()){
		qlCommandSockets.append(commandServer->nextPendingConnection());
		connect(qlCommandSockets.last(), SIGNAL(readyRead()), this, SLOT(vReadCommand()));
		connect(qlCommandSockets[0], SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(vShowConnectionError(QAbstractSocket::SocketError)));
	}
	qDebug() << "Getting connection request...";
}

// client stuff

void network::vConnectToServer(){
	if(qlCommandSockets.isEmpty() || qlCommandSockets[0]==NULL) qlCommandSockets.append(new QTcpSocket(this));
	
	connect(qlCommandSockets[0], SIGNAL(readyRead()), this, SLOT(vReadCommand()));
	connect(qlCommandSockets[0], SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(vShowConnectionError(QAbstractSocket::SocketError)));
	//connect(serverTimeout, SIGNAL(timeout()), this, SLOT(vServerTimeout()));
	
	QHostAddress ip;
	
	if(!bAskForIp(ip)){} // TODO
	
	vShowProgressForClient();
	
	qlCommandSockets[0]->connectToHost(ip, 51483); // TODO port from config
	qDebug() << "Sending request to server" << ip << endl;
	
	if(qlCommandSockets[0]->waitForConnected(10000)) {
		//progressDialog->setLabelText(tr("Waiting for connection being accepted…"));
		QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
		
		//bNETConnected=TRUE;
		
		vSendCommand(QString("NEW||%0||%1").arg(dNetworkVersion).arg(qlCommandSockets[0]->peerAddress().toString()));
		//serverTimeout->start(10000);
		
	} else {
		vShowConnectionError(QAbstractSocket::SocketTimeoutError);
	}
}

bool network::bAskForIp(QHostAddress &ip) {
	bool ok=true;
	while(ip.isNull() && ok) {
		ip = QInputDialog::getText(this, tr("Network Game"), tr("Please enter the IP address of player 1:"), QLineEdit::Normal, "127.0.0.1", &ok); // TODO load IP from setting
	}
	
	return ok;
}

void network::vServerTimeout(){
	vShowConnectionError(QAbstractSocket::SocketTimeoutError);
}

void network::vShowProgressForClient() {
	progressDialog=new QProgressDialog(tr("Waiting for connection being accepted"),tr("Cancel"),0,0,this);
	//connect(progressDialog, SIGNAL(canceled()), this, SLOT(vSinglePlayer())); TODO
	progressDialog->setWindowModality(Qt::WindowModal);
	progressDialog->show();
}

// common

void network::vSendCommand(QString command){
	//if(!bNETConnected) return;
	qDebug() << "Sending command:" << command;
	command=command+"\n";
	qlCommandSockets[0]->write(command.toAscii().data());
	qlCommandSockets[0]->flush();
}

void network::vReadCommand() {
	QTcpSocket *commandSocket;
	commandSocket=static_cast<QTcpSocket*>(QObject::sender());
	
	if(bExpectingImageData) { qDebug() << "expecting image"; return; }
	
	if(!commandSocket->canReadLine()) qDebug() << "ERROR: vReadCommand: Cannot read line";
	while(commandSocket->canReadLine()) {
		
		QStringList command = static_cast<QString>(commandSocket->readLine()).split("||");
		
		if(command.last().endsWith("\n")) command.last()=command.last().left(command.last().size()-1);
		if(command.last().isEmpty()) command.removeLast();
		if(command.size()==0) command.append("EMPTY");
		
		qDebug() << "Reading command:" << command;
//		else qDebug() << "Expecting data"; //we do not want to cout image-data
		
		if(command[0]==("NEW") && command.size()==3) {
			qDebug() << "Player" << iNumberOfPlayers << "tries to connect";
			
			progressDialog->setLabelText(tr("Got connection request"));
			
			QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
			
			double foreignVersion=command[1].toDouble();
			qDebug() << "foreignVersion is" << foreignVersion << "and our dNetworkVersion is" << dNetworkVersion;
			if(foreignVersion>dNetworkVersion || static_cast<int>(foreignVersion)!=static_cast<int>(dNetworkVersion)) {
				qDebug() << "Protocol version incompatible";
				progressDialog->setLabelText(tr("Got connection request: incompatible version"));
				vSendCommand("INCOMP");
				return;
			}
			
			qDebug() << "IP:" << command[2];
			
			QMessageBox msgBox;
			msgBox.addButton(QMessageBox::Yes);
			msgBox.addButton(QMessageBox::No);
			msgBox.setDefaultButton(QMessageBox::No);
			msgBox.setText(tr("Do you want to play with the player who has the IP %1?").arg(command[2]));
			msgBox.setIcon(QMessageBox::Question);
			msgBox.QMessageBox::setWindowTitle (tr("New Player"));
			
			int i=msgBox.exec();
			
			if(i!=QMessageBox::Yes){
				vSendCommand("REFUSED");
				progressDialog->setLabelText(tr("Waiting for connection request…"));
				//vStopNet();
				return;
			}
			vSendCommand("OKTOCONNECT");
			
			iNumberOfPlayers++;
			
			progressDialog->setMaximum(12);
			
			progressDialog->setLabelText(tr("Sending coordinates..."));
			progressDialog->setValue(progressDialog->value()+1);
			QString command="COORDS||NAMES||";
			for(int i=0;i<myDart->qlAllPlaces.size();i++){
				command=QString("%1%2||").arg(command).arg(myDart->qlAllPlaces[i].name);
			}
			vSendCommand(command);
			progressDialog->setValue(progressDialog->value()+1);
			command="COORDS||DIMX||";
			for(int i=0;i<myDart->qlAllPlaces.size();i++){
				command=QString("%1%2||").arg(command).arg(myDart->qlAllPlaces[i].dimx);
			}
			vSendCommand(command);
			progressDialog->setValue(progressDialog->value()+1);
			command="COORDS||DIMY||";
			for(int i=0;i<myDart->qlAllPlaces.size();i++){
				command=QString("%1%2||").arg(command).arg(myDart->qlAllPlaces[i].dimy);
			}
			vSendCommand(command);
			progressDialog->setValue(progressDialog->value()+1);
			command="COORDS||X||";
			for(int i=0;i<myDart->qlAllPlaces.size();i++){
				command=QString("%1%2||").arg(command).arg(myDart->qlAllPlaces[i].x);
			}
			vSendCommand(command);
			progressDialog->setValue(progressDialog->value()+1);
			command="COORDS||Y||";
			for(int i=0;i<myDart->qlAllPlaces.size();i++){
				command=QString("%1%2||").arg(command).arg(myDart->qlAllPlaces[i].y);
			}
			vSendCommand(command);
			
			progressDialog->setLabelText(tr("Sending images…"));
			for(int i=0;i<4;i++) {
				command="IMAGE||"+myDart->qlLayersNames[i].toUpper()+"||";
				progressDialog->setValue(progressDialog->value()+1);
				
				QString imgfile=myDart->qlQcfxFiles[myDart->iCurrentQcf].path+"/"+myDart->qlLayersNames[i]+".png";
				
				if(!QFile::exists(imgfile)){
					QPixmap pixmap(imgfile);
					QDataStream socketstream(commandSocket);
					
					QByteArray block;
					QDataStream out(&block, QIODevice::WriteOnly);
					out.setVersion(QDataStream::Qt_4_0);
					out << pixmap;
					
					qDebug() << block.size() << "is size";
					vSendCommand(QString("%1%2").arg(command).arg(block.size()));
					myDart->mySleep(100);
					socketstream << pixmap;
					commandSocket->write(block);
					commandSocket->flush();
					myDart->mySleep(100);
					vSendCommand("");
				}
			}
			
//			progressDialog->setLabelText(tr("Sende Einstellungen..."));  // TODO CONTINUE WORK HERE…
//			QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
			
//			sendCommand(QString("SETTINGS!%1#%2#%3#%4#%5#%6#%7#%8#%9#%10#%11#%12") .arg(iAskFor).arg(iGameMode).arg(runden).arg(iDifficulty).arg(itimeout).arg(btime).arg(barea) .arg(currentqcf).arg('0').arg('0').arg('0').arg(pxtokm) .toAscii().data()); //// no qcffile
			
//			actionNetwork->setChecked(TRUE);
			
//			vDisableSettings();
			
//			vNetworkReady();
			
		} else if(command[0]==("INCOMP")) {
			
			qDebug() << "incomp"; // TODO msgbox
			
		} else if(command[0]==("REFUSED")) {
			
			qDebug() << "refused"; // TODO msgbox
		
		} else if(command[0]==("OKTOCONNECT")) {
			
			progressDialog->setLabelText(tr("Connection request has been accepted."));
		
		} else if(command[0]==("COORDS") && command.size()>2) {
			
			if(command[1]==("NAMES")) {
				progressDialog->setMaximum(12);
				progressDialog->setLabelText(tr("Receiving coordinates…"));
				progressDialog->setValue(progressDialog->value()+1);
				
				myDart->qlAllPlaces.clear();
				
				for(int i=2;i<command.size();i++) {
					place p;
					p.name=command[i];
					myDart->qlAllPlaces.append(p);
				}
			} else if(command[1]==("DIMX") || command[1]==("DIMY") || command[1]==("X") || command[1]==("Y")) {
				progressDialog->setValue(progressDialog->value()+1);
				
				for(int i=2;i<command.size()&&i<myDart->qlAllPlaces.size()+2;i++) {
					if(command[1]==("DIMX")) myDart->qlAllPlaces[i-2].dimx=command[i].toInt();
					else if(command[1]==("DIMY")) myDart->qlAllPlaces[i-2].dimy=command[i].toInt();
					else if(command[1]==("X")) myDart->qlAllPlaces[i-2].x=command[i].toInt();
					else if(command[1]==("Y")) myDart->qlAllPlaces[i-2].y=command[i].toInt();
				}
			} else {
				qDebug() << "[E] broken command"; return;
			}
		
		} else if (command[0]=="IMAGE") {
					
			bExpectingImageData=true;
			
			progressDialog->setLabelText(tr("Receiving images…"));
			progressDialog->setValue(progressDialog->value()+1);
			
			int blockSize=0;
			
			QDataStream socketstream(commandSocket);
			
			blockSize=command[2].toInt();
			
			qDebug() << blockSize << "reported size";
			
			while(commandSocket->bytesAvailable() < blockSize)  myDart->mySleep(20);
			
			qDebug() << commandSocket->bytesAvailable() << "is size";
			
			
			qDebug() << "Writing image";
			QPixmap pixmap;
			
			socketstream >> pixmap;
			
			QString tempDir=QDir::tempPath();
			
			QString configPath=QCoreApplication::applicationDirPath()+"/QeoDart.conf"; // TODO should use bPartable or the like
			if(QFile::exists(configPath)) { // portable
				tempDir=QCoreApplication::applicationDirPath(); // TODO what if not writable?
			}
			
			if(!tempDir.endsWith("\\") && !tempDir.endsWith("/")) tempDir+="/";
			
			QString filename=tempDir+command[1]+".png";
			
			qDebug() << "Saving" << filename;
			
			pixmap.save("/tmp/1.png"); // TODO CONTINUE HERE not saving? Test old version
			
			bExpectingImageData=false;
			
			int i=0;
			
			if(command[1]=="BACKGROUND") i=0;
			else if(command[1]=="ELEVATIONS") i=1;
			else if(command[1]=="BORDERS") i=2;
			else if(command[1]=="RIVERS") i=3;
			else { qDebug() << "[E] broken command"; return; } // TODO remove file
			
			myDart->qlMapLayers[i]->setText(QString("<img src=\"%2.png\" height=\"%1\" width=\"%1\"/>").arg(600*myDart->dZoomFactor).arg(filename));
			
			/*
			actionFlusse->setVisible(TRUE);
			actionHohen->setVisible(TRUE);
			actionGrenzen_Kreise->setVisible(TRUE); TODO*/	
			
		} else if(command[0]!="EMPTY") {
			
			qDebug() << "[E] unknown command";
			
		}
	}
}

void network::vShowConnectionError(QAbstractSocket::SocketError socketError){
	//serverTimeout->blockSignals(TRUE); // TODO w/o this there will be a crash on P2->IP-OK->Refused --- We destory this class after this function, so we do not need to set it to FALSE again
//	if(spieler==0){
//		commandServer->blockSignals(TRUE);
//	}
//	commandSocket->blockSignals(TRUE);
	
//	serverTimeout->stop();
	QMessageBox connectionErrorBox;
	//qDebug() << commandSocket->QAbstractSocket::error();
	QString errstr;
	switch (socketError){
		case QAbstractSocket::RemoteHostClosedError: // TODO ->en
			errstr="RemoteHostClosedError";
			connectionErrorBox.critical(this, QString(tr("Fehler")), QString(tr("<b>Verbindung getrennt</b><br/>Die Netzwerkverbindung wurde unerwartet getrennt. Ihr Mitspieler hat vielleicht die Verbindung getrennt.<br/><br/>Die Fehlermeldung lautet: %1")).arg(errstr));
			break;
		case QAbstractSocket::ConnectionRefusedError:
			errstr="ConnectionRefusedError";
			connectionErrorBox.critical(this, QString(tr("Fehler")), QString(tr("<b>Verbindung verweigert</b><br/>Die Netzwerkverbindung kann nicht hergestellt werden. Stellen Sie sicher, dass Spieler 1 bereit ist und die Verbindung nicht abgelehnt hat.<br/><br/>Die Fehlermeldung lautet: %1")).arg(errstr));
			break;
		case QAbstractSocket::HostNotFoundError:
			errstr="HostNotFoundError";
			connectionErrorBox.critical(this, QString(tr("Fehler")), QString(tr("<b>Server nicht gefunden</b><br/>Mit den angegebenen Daten konnte kein Server gefunden werden. M&ouml;glicherweise ist der Spieler nicht bereit oder Adresse oder Port sind falsch.<br/><br/>Die Fehlermeldung lautet: %1")).arg(errstr));
			break;
		case QAbstractSocket::SocketAccessError:
			errstr="SocketAccessError";
			connectionErrorBox.critical(this, QString(tr("Fehler")), QString(tr("<b>Fehlende Zugriffsrechte</b><br/>Die vorgesehene Netzwerkoperation ist nicht m&ouml;glich, da die erforderlichen Rechte fehlen.<br/><br/>Die Fehlermeldung lautet: %1")).arg(errstr));
			break;
		case QAbstractSocket::SocketResourceError:
			errstr="SocketResourceError";
			connectionErrorBox.critical(this, QString(tr("Fehler")), QString(tr("<b>Netzwerkoperation nicht m&ouml;glich</b><br/>Die vorgesehene Netzwerkoperation ist nicht m&ouml;glich, da sie m&ouml;glicherweise vom Betriebssystem oder der verwendeten Hardware nicht unterst&uuml;tzt wird.<br/><br/>Die Fehlermeldung lautet: %1")).arg(errstr));
			break;
		case QAbstractSocket::UnsupportedSocketOperationError:
			errstr="UnsupportedSocketOperationError";
			connectionErrorBox.critical(this, QString(tr("Fehler")), QString(tr("<b>Netzwerkoperation nicht m&ouml;glich</b><br/>Die vorgesehene Netzwerkoperation ist nicht m&ouml;glich, da sie m&ouml;glicherweise vom Betriebssystem oder der verwendeten Hardware nicht unterst&uuml;tzt wird.<br/><br/>Die Fehlermeldung lautet: %1")).arg(errstr));
			break;
		case QAbstractSocket::SocketTimeoutError:
			errstr="SocketTimeoutError";
			connectionErrorBox.critical(this, QString(tr("Fehler")), QString(tr("<b>Zeit&uuml;berschreitung</b><br/>Die angeforderte Netzwerkoperation dauerte zu lange und wurde abgebrochen. M&ouml;glicherweise ist der Server oder das Netzwerk zur Zeit &uuml;berlastet, versuchen Sie es daher sp&auml;ter noch einmal.<br/><br/>Die Fehlermeldung lautet: %1")).arg(errstr));
			break;
		case QAbstractSocket::DatagramTooLargeError:
			errstr="DatagramTooLargeError";
			connectionErrorBox.critical(this, QString(tr("Fehler")), QString(tr("<b>Datenlimit &uuml;berschritten</b><br/>Die zu versendenden Daten &uuml;berschreiten die vom Betriebssystem vorgegebene Grenze.<br/><br/>Die Fehlermeldung lautet: %1")).arg(errstr));
			break;
		case QAbstractSocket::NetworkError:
			errstr="NetworkError";
			connectionErrorBox.critical(this, QString(tr("Fehler")), QString(tr("<b>Netzwerkfehler</b><br/>Es trat ein unbekannter Fehler im Netzwerk auf. Dies kann zum Beispiel ein herausgezogenes Netzwerkkabel sein. Pr&uuml;fen Sie bitte Ihr Netzwerk und versuchen es dann noch einmal.<br/><br/>Die Fehlermeldung lautet: %1")).arg(errstr));
			break;
		case QAbstractSocket::UnknownSocketError:
			errstr="UnknownSocketError";
			connectionErrorBox.critical(this, QString(tr("Fehler")), QString(tr("<b>Netzwerkoperation nicht m&ouml;glich</b><br/>Die vorgesehene Netzwerkoperation ist nicht m&ouml;glich, da sie m&ouml;glicherweise vom Betriebssystem oder der verwendeten Hardware nicht unterst&uuml;tzt wird.<br/><br/>Die Fehlermeldung lautet: %1")).arg(errstr));
			break;
		default:
			errstr="Default";
			connectionErrorBox.critical(this, QString(tr("Fehler")), QString(tr("<b>Netzwerkoperation nicht m&ouml;glich</b><br/>Die vorgesehene Netzwerkoperation ist nicht m&ouml;glich, da sie m&ouml;glicherweise vom Betriebssystem oder der verwendeten Hardware nicht unterst&uuml;tzt wird.<br/><br/>Die Fehlermeldung lautet: %1")).arg(errstr));
	}
	
//	bNETConnected=FALSE; // We do not want to send "DISCONNECTED", since we are not connected
//	vStopNet();
}
