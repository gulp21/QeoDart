/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "dart.hpp"
#include "network.hpp"

using namespace std;

network::network(dart *TDart, io *TIO) : myDart(TDart), myIO(TIO) {
	commandServer=NULL;
	//commandSocket=null;
	
	bGameStarted=false;
	bExpectingImageData=false;
	
	deleteTimer = new QTimer(this);
	connect(deleteTimer, SIGNAL(timeout()), this, SLOT(vDeleteSockets()));
	deleteTimer->setSingleShot(true);
	deleteTimer->setInterval(NETTIMEOUT+100);
}
 
network::~network() {
}

void network::vNewNetworkGame() {
	iNumberOfPlayers=1;
	
	QMessageBox msgBox;
	QPushButton *serverButton = msgBox.addButton(tr("Player 1 (Server)"), QMessageBox::AcceptRole);
	QPushButton *otherPlayerButton = msgBox.addButton(tr("Other Player"), QMessageBox::AcceptRole);
	QPushButton *cancelButton = msgBox.addButton("Cancel", QMessageBox::RejectRole);
	cancelButton->hide();
	msgBox.setWindowTitle(tr("Start Network Game"));
	msgBox.setText(tr("Do you want to be player 1 or another player?"));
	msgBox.setInformativeText(tr("When you want to play with other players over the Internet, one player (player 1) has to be the so-called server. The settings and the map used during the game are determined by player 1."));
	
	msgBox.exec();
	
	if(msgBox.clickedButton()==serverButton) {
		vSetServerOnline();
	} else if(msgBox.clickedButton()==otherPlayerButton) {
		vConnectToServer();
	} else {
		myDart->actionLocal->trigger();
	}
}

// server stuff

void network::vGameStartQuestion() {
	QMessageBox msgBox;
	QPushButton *startButton = msgBox.addButton(tr("Start Game"), QMessageBox::AcceptRole);
	msgBox.addButton(QMessageBox::Cancel);
	msgBox.setWindowTitle(tr("Waiting For Other Players"));
	msgBox.setText(tr("Do you want to start the game?"));
	msgBox.setInformativeText(tr("At the moment there are %n player(s).","",iNumberOfPlayers));
	
	msgBox.exec();
	
	if(msgBox.clickedButton()==startButton) {
		vSendCommand("GAMESTART");
		vDestroyProgressDialog();
		myDart->vNextRound();
	} else {
		//TODO cancel network
	}
}

void network::vSetServerOnline() {
	if(commandServer==NULL) commandServer=new QTcpServer(this);
	commandServer->listen(QHostAddress::Any,51483);
	connect(commandServer, SIGNAL(newConnection()), this, SLOT(vNewClient()));
	
	qDebug() << "Server is online";
	
	QList<QHostAddress> ips=QNetworkInterface::allAddresses();
	QString ip;
	qDebug() << ips;
	for(int i=0; i<ips.size(); i++) {
		if(!ips[i].toString().startsWith("127.") && !ips[i].toString().startsWith("0:0:0:0:0:0:0:")) { // TODO ipv6 not working?
			ip=QString("%1, %2").arg(ip).arg(ips[i].toString());
		}
	}
	if(ip.length()>0) ip=" ("+ip.right(ip.length()-2)+")";
	
	vShowProgress(tr("Waiting for connection request…\nThe other players must know your IP address%1.").arg(ip));
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
	if(deleteTimer->isActive()) { // this it true when commandSockets of an old connection exists; we must delete them now
		deleteTimer->stop();
		vDeleteSockets();
	}
	
	if(qlCommandSockets.isEmpty() || qlCommandSockets[0]==NULL) qlCommandSockets.append(new QTcpSocket(this));
	
	connect(qlCommandSockets[0], SIGNAL(readyRead()), this, SLOT(vReadCommand()));
	connect(qlCommandSockets[0], SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(vShowConnectionError(QAbstractSocket::SocketError)));
	//connect(serverTimeout, SIGNAL(timeout()), this, SLOT(vServerTimeout()));
	
	QHostAddress ip;
	
	if(!bAskForIp(ip)){return;} // TODO
	
	vShowProgress(tr("Waiting for connection being accepted"));
	
	qlCommandSockets[0]->connectToHost(ip, 51483); // TODO port from config
	qDebug() << "Sending request to server" << ip << endl;
	
	if(qlCommandSockets[0]->waitForConnected(NETTIMEOUT)) {
		QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
		
		vSendCommand(QString("NEW||%0||%1").arg(NETWORKVERSION).arg(qlCommandSockets[0]->peerAddress().toString()));
		//serverTimeout->start(10000); TODO?
		
	} else {
		vShowConnectionError(QAbstractSocket::SocketTimeoutError);
	}
}

bool network::bAskForIp(QHostAddress &ip) {
	bool ok=true;
	while(ip.isNull() && ok) {
		ip = QInputDialog::getText(this, tr("Network Game"), tr("Please enter the IP address of player 1:"), QLineEdit::Normal, myIO->settings->value("qsLastIP", "127.0.0.1").toString(), &ok);
	}
	
	if(ok && !ip.isNull()) {
		myIO->settings->setValue("qsLastIP", ip.toString());
	}
	
	return ok;
}

void network::vServerTimeout(){
	vShowConnectionError(QAbstractSocket::SocketTimeoutError);
}

// common

void network::vShowProgress(QString message) {
	progressDialog=new QProgressDialog(message,tr("Cancel"),0,0,this);
	connect(progressDialog, SIGNAL(canceled()), this, SLOT(vProgressDialogCancelled()));
	progressDialog->setWindowModality(Qt::WindowModal);
	progressDialog->show();
}

void network::vProgressDialogCancelled() {
	vStopNetworking();
	vDestroyProgressDialog();
	myDart->actionLocal->trigger();
}

void network::vSendCommand(QString command){
	if(qlCommandSockets.length()==0) {
		qDebug() << "[E] qlCommandSockets.length is 0";
		return;
	}
	
	qDebug() << "Sending command:" << command;
	command=command+"\n";
	for(int i=0; i<qlCommandSockets.size(); i++) {
		qlCommandSockets[i]->write(command.toUtf8().data());
		qlCommandSockets[i]->flush();
	}
}

void network::vReadCommand() {
	QTcpSocket *commandSocket;
	commandSocket=static_cast<QTcpSocket*>(QObject::sender());
	
	if(!commandSocket->canReadLine()) qDebug() << "ERROR: vReadCommand: Cannot read line";
	
	while(commandSocket->canReadLine()) {
		
		QStringList command = QString::fromUtf8(commandSocket->readLine()).split("||");
		
		if(command.last().endsWith("\n")) command.last()=command.last().left(command.last().size()-1);
		if(command.last().isEmpty()) command.removeLast();
		if(command.size()==0) command.append("EMPTY");
		
		if(!bExpectingImageData) qDebug() << "Reading command:" << command;
		
		if(command[0]==("NEW") && command.size()==3) {
			qDebug() << "Player" << iNumberOfPlayers << "tries to connect";
			
			progressDialog->setLabelText(tr("Got connection request"));
			
			QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
			
			double foreignVersion=command[1].toDouble();
			qDebug() << "foreignVersion is" << foreignVersion << "and our NETWORKVERSION is" << NETWORKVERSION;
			if(foreignVersion>NETWORKVERSION || static_cast<int>(foreignVersion)!=static_cast<int>(NETWORKVERSION)) {
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
				
				if(QFile::exists(imgfile)){
					QPixmap pixmap(imgfile);
					QDataStream socketstream(commandSocket);
					
					QByteArray block;
					QDataStream out(&block, QIODevice::WriteOnly);
					out.setVersion(QDataStream::Qt_4_0);
					out << pixmap;
					
					qDebug() << block.size() << "is size";
					vSendCommand(QString("%1%2").arg(command).arg(block.size()));
					myDart->mySleep(200);
					socketstream << pixmap;
					commandSocket->write(block);
					commandSocket->flush();
					myDart->mySleep(200);
					vSendCommand("");
				}
			}
			
			progressDialog->setLabelText(tr("Sende Einstellungen..."));
			QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
			
			vSendCommand(QString("SETTINGS||%1||%2||%3||%4||%5||%6||%7||%8||%9")
			             .arg(myDart->iAskForMode)
			             .arg(myDart->iMaxPlaceCount)
			             .arg(myDart->qsCurrentPlaceType) // TODO is this necessary?
			             .arg(myDart->iMaxTime).arg(myDart->bAgainstTime)
			             .arg(myDart->iScoreAreaMode)
			             .arg(myDart->agLayers->actions()[0]->isChecked() && myDart->agLayers->actions()[0]->isVisible())
			             .arg(myDart->agLayers->actions()[1]->isChecked() && myDart->agLayers->actions()[1]->isVisible())
			             .arg(myDart->agLayers->actions()[2]->isChecked() && myDart->agLayers->actions()[2]->isVisible())
			             .toAscii().data()); // TODO send qcffile info
			
//			actionNetwork->setChecked(TRUE);
			
//			vDisableSettings(); TODO
			
//			vNetworkReady();
			
			vSendCommand(QString("PLAYERS||%1").arg(iNumberOfPlayers));
			myDart->vSetNumberOfPlayers(iNumberOfPlayers);
			
			vGameStartQuestion();
			
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
			
			QString filename=myIO->qsGetTempDir()+command[1]+".png";
			
			qDebug() << "Saving" << filename;
			qDebug() << "successfull?" << pixmap.save(filename);
			
			int i=0;
			
			if(command[1]=="BACKGROUND") i=0;
			else if(command[1]=="ELEVATIONS") i=1;
			else if(command[1]=="BORDERS") i=2;
			else if(command[1]=="RIVERS") i=3;
			else {
				qDebug() << "[E] broken command";
				QFile(filename).remove();
				return;
			}
			
			myDart->qlMapLayers[i]->setText("");
			myDart->qlMapLayers[i]->setText(QString("<img src=\"%2\" height=\"%1\" width=\"%1\"/>").arg(600*myDart->dZoomFactor).arg(filename));
			
			/*
			actionFlusse->setVisible(TRUE);
			actionHohen->setVisible(TRUE);
			actionGrenzen_Kreise->setVisible(TRUE); TODO*/	
			
		} else if(command[0]=="SETTINGS" && command.size()>=10) {
			
			bExpectingImageData=false;
			
			qDebug() << "Reading command:" << command;
			
			progressDialog->setLabelText(tr("Receiving settings…"));
			progressDialog->setValue(progressDialog->value()+1);
			
			myDart->iCurrentPlayer=1; // must set it here in order to avoid sending NEXTPLACE when changing askForMode
			
			if(command[1].toInt()<0 || command[1].toInt()>1) {
				qDebug() << "[E] iAskForMode out of range (" << command[1] << ")";
				command[1]="0";
			}
			myDart->iAskForMode=static_cast<enAskForModes>(command[1].toInt());
			myDart->vSetAskForMode(myDart->iAskForMode);
			myDart->iMaxPlaceCount=command[2].toInt();
			myDart->qsCurrentPlaceType=command[3];
			myDart->iMaxTime=command[4].toInt();
			myDart->bAgainstTime=command[5]=="1";
			myDart->vSetAgainstTime(myDart->bAgainstTime);
			myDart->iScoreAreaMode=command[6].toInt();
			myDart->qlMapLayers[1]->setVisible(command[7]=="1");
			myDart->qlMapLayers[2]->setVisible(command[8]=="1");
			myDart->qlMapLayers[3]->setVisible(command[9]=="1");
			
			myIO->vFillCurrentTypePlaces();
			
		} else if(command[0]=="PLAYERS" && command.size()==2) {
			
			iNumberOfPlayers=command[1].toInt();
			
			progressDialog->setLabelText(tr("Waiting for game start… (%n player(s) at the moment)","",iNumberOfPlayers));
			progressDialog->setValue(11);
			
			myDart->vSetNumberOfPlayers(iNumberOfPlayers);
			myDart->iCurrentPlayer=1;//TODO
			
		} else if(command[0]=="GAMESTART") {
			
			vDestroyProgressDialog();
			
		} else if(command[0]=="NEXTPLACE" && command.size()==2) {
			
			myDart->qlPlacesHistory.append(myDart->qlCurrentTypePlaces[command[1].toInt()]);
			myDart->vNextRound();
			iReceivedScores=0;
			
		} else if(command[0]=="SCORE" && command.size()>=9) {
			
			scoreHistory score;
			
		        score.diffPx=command[2].toDouble();
			score.diffPxArea=command[3].toDouble();
			score.diffKm=command[4].toDouble();
			score.score=command[5].toDouble();
			score.mark=command[6].toDouble();
			score.x=command[7].toDouble();
			score.y=command[8].toDouble();
			
			myDart->vAddScoreForPlayer(command[1].toInt(),score);
			
			iReceivedScores++;
			
			if(iReceivedScores==iNumberOfPlayers || (iReceivedScores+1==iNumberOfPlayers && myDart->qlScoreHistory[myDart->iCurrentPlayer].size()==myDart->iPlaceCount) ) myDart->vShowResults();
			
		} else if(command[0]!="EMPTY" && !bExpectingImageData) {
			
			qDebug() << "[E] unknown or broken command";
			
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
	connectionErrorBox.setIcon(QMessageBox::Critical);
	//qDebug() << commandSocket->QAbstractSocket::error();
	QString errstr;
	switch (socketError){
		case QAbstractSocket::RemoteHostClosedError:
			errstr="RemoteHostClosedError";
			connectionErrorBox.setWindowTitle(tr("Connection terminated"));
			connectionErrorBox.setText(tr("The network connection has been closed unexpectedly. Maybe another player closed the connection."));
			break;
		case QAbstractSocket::ConnectionRefusedError:
			errstr="ConnectionRefusedError";
			connectionErrorBox.setWindowTitle(tr("Connection refused"));
			connectionErrorBox.setText(tr("The network connection cannot be set up. Make sure that player 1 is ready, and did not refuse the connection."));
			break;
		case QAbstractSocket::HostNotFoundError:
			errstr="HostNotFoundError";
			connectionErrorBox.setWindowTitle(tr("Server not found"));
			connectionErrorBox.setText(tr("No server with the given specifications could be found. Maybe the other player is not ready yet or address or port are wrong."));
			break;
		case QAbstractSocket::SocketAccessError:
			errstr="SocketAccessError";
			connectionErrorBox.setWindowTitle(tr("Missing permissions"));
			connectionErrorBox.setText(tr("The planned network operation is not possible, bacuse necessary permissions are missing."));
			break;
		case QAbstractSocket::SocketResourceError:
			errstr="SocketResourceError";
			connectionErrorBox.setWindowTitle(tr("Network operation not possible"));
			connectionErrorBox.setText(tr("The planned network operation is not possible, because is may be not supported by the operating system, or the used hardware."));
			break;
		case QAbstractSocket::UnsupportedSocketOperationError:
			errstr="UnsupportedSocketOperationError";
			connectionErrorBox.setWindowTitle(tr("Network operation not possible"));
			connectionErrorBox.setText(tr("The planned network operation is not possible, because is may be not supported by the operating system, or the used hardware."));
			break;
		case QAbstractSocket::SocketTimeoutError:
			errstr="SocketTimeoutError";
			connectionErrorBox.setWindowTitle(tr("Timeout"));
			connectionErrorBox.setText(tr("The requested network operation took too long and has been canceled. Maybe the server or the network is overloaded currently, please try again later."));
			break;
		case QAbstractSocket::DatagramTooLargeError:
			errstr="DatagramTooLargeError";
			connectionErrorBox.setWindowTitle(tr("Data limit exceeded"));
			connectionErrorBox.setText(tr("The data to be sent exceedes the limit given by the operating system."));
			break;
		case QAbstractSocket::NetworkError:
			errstr="NetworkError";
			connectionErrorBox.setWindowTitle(tr("Network error"));
			connectionErrorBox.setText(tr("An unknown error occured in the network. This might be for example an unplugged network cable. Please check you network and try again."));
			break;
		case QAbstractSocket::UnknownSocketError:
			errstr="UnknownSocketError";
			connectionErrorBox.setWindowTitle(tr("Network operation not possible"));
			connectionErrorBox.setText(tr("The planned network operation is not possible, because is may be not supported by the operating system, or the used hardware."));
			break;
		default:
			errstr="Default";
			connectionErrorBox.setWindowTitle(tr("Network operation not possible"));
			connectionErrorBox.setText(tr("The planned network operation is not possible, because is may be not supported by the operating system, or the used hardware."));
	}
	
	connectionErrorBox.setInformativeText(tr("The error message is: %1").arg(errstr));
	connectionErrorBox.exec();
	
	vStopNetworking();
	
	myDart->actionLocal->trigger();
}

void network::vDestroyProgressDialog() {
	if(progressDialog!=NULL) {
		progressDialog->blockSignals(true); // otherwise we trigger cancelled() which would call vStopNetworking
		progressDialog->close();
		delete progressDialog;
	}
	progressDialog=NULL;
}

void network::vStopNetworking() {
	vSendCommand("DISCONNECT||ERROR");
	
	deleteTimer->start(); // we must delay it, otherwise waitForConnected crashes
	
	delete commandServer;
	commandServer=NULL;
	vDestroyProgressDialog();
	myIO->vLoadSettings();
}

void network::vDeleteSockets() {
	qDebug() << "[i] Delete Sockets";
	while(qlCommandSockets.size()>0) {
		delete qlCommandSockets.last();
		qlCommandSockets.removeLast();
	}
}
