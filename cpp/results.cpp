/*
QeoDart Copyright (C) 2012 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "results.hpp"
#include "qtwin.h"

using namespace std;

resultWindow::resultWindow(int &HighlightHighScore, dart *TDart, int PLayer, io *TIO, QDialog *parent) : myDart(TDart), player(PLayer), myIO(TIO), QDialog(parent) {
	const int R1=15, G1=135, B1=28, R2=255, G2=25, B2=25;
	
	myIO->vLoadHighScores(myDart->qlQcfxFiles[myDart->iCurrentQcf].mapName);
	
	iHighlightHighScore=&HighlightHighScore;
	
	setParent(myDart);

	setupUi(this);
	
// allow maximizing on WinCE in order to make the whole window readable + add scrollarea
#ifdef Q_OS_WINCE
	setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint);
	setSizeGripEnabled(true);
	lblScore->setStyleSheet("font-size:12pt");
	lblMark->setStyleSheet("font-size:12pt");
	
	QScrollArea *scrollArea;
	QWidget *scrollAreaWidgetContents;
	QVBoxLayout *verticalLayout;
	scrollArea = new QScrollArea(this);
	scrollArea->setFrameShape(QFrame::NoFrame);
	scrollArea->setFrameShadow(QFrame::Plain);
	scrollArea->setWidgetResizable(true);
	scrollAreaWidgetContents = new QWidget();
	scrollAreaWidgetContents->setGeometry(QRect(0, 0, 427, 60));
	verticalLayout = new QVBoxLayout(scrollAreaWidgetContents);
	verticalLayout_2->removeWidget(lblPlaces);
	verticalLayout->addWidget(lblPlaces);
	scrollArea->setWidget(scrollAreaWidgetContents);
	verticalLayout_2->insertWidget(3,scrollArea);
#else
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
#endif
	
	if(QtWin::extendFrameIntoClientArea(this)) { // use aero glass if possible
		setWindowOpacity(1.0); // otherwise there are artifacts
	} else {
		setWindowOpacity(0.8);
	}
	
	if(myDart->iNumberOfPlayers==1) {
		lblScore->setText(QString(tr("You have a score of %n point(s) (&Oslash; %1).","",
		                             myDart->qlTotalScores[0].score))
		                  .arg(myDart->qlTotalScores[0].score/myDart->iMaxPlaceCount,0,'f',1));
	} else {
		lblScore->setText(QString(tr("Player %1 has a score of %n point(s) (&Oslash; %2).","",
		                             myDart->qlTotalScores[player].score))
		                  .arg(player+1)
		                  .arg(myDart->qlTotalScores[player].score/myDart->iMaxPlaceCount,0,'f',1));
	}
	
	lblMark->setText(QString(tr("That equals grade %1.")).arg(myDart->qlTotalScores[player].mark,0,'f',1));
	
	
	QString places;
	for(int i=0,max=myDart->qlScoreHistory[player].count(); i<max; i++) {
		// find the suitable color for the mark:
		// f(mark)=colorStart+(colorEnd-colorStart)*1/6*mark
		double x=(myDart->qlScoreHistory[player][i].mark-1)*1/6;
		int r=R1+(R2-R1)*x;
		int g=G1-(G1-G2)*x;
		int b=B1-(B1-B2)*x;
		
		QString km;
		if(myDart->qlScoreHistory[player][i].diffKm==-1) km=tr("∞");
		else km=QString("%1").arg(myDart->qlScoreHistory[player][i].diffKm,0,'f',1);
		
		places+=QString(tr("<span style=\"color:rgb(%5,%6,%7)\">%1&nbsp;(%2&nbsp;km,&nbsp;%3&nbsp;P.,&nbsp;%4)</span>, "))
		                .arg(myDart->qlPlacesHistory[i]->name)
		                .arg(km)
		                .arg(myDart->qlScoreHistory[player][i].score)
		                .arg(myDart->qlScoreHistory[player][i].mark,0,'f',1)
		                .arg(r).arg(g).arg(b);
		
	}
	
	lblPlaces->setText(places.left(places.length()-2)); // remove the last ", "
	
	// do not show lineEdit when the score is not good enough, or when too few places are selected (otherwise you could play with just one place and get a very good score), or when it is a network player
	qDebug() << (myDart->qlHighScores[9].score>=myDart->qlTotalScores[player].score && !(myDart->qlCurrentTypePlaces.count()<myDart->iMaxPlaceCount && myDart->qlCurrentTypePlaces.count()>myDart->iMaxPlaceCount)) << myDart->qlHighScores[9].score << myDart->qlTotalScores[player].score << myDart->qlCurrentTypePlaces.count() << myDart->iMaxPlaceCount << myDart->qlCurrentTypePlaces.count() << myDart->iMaxPlaceCount;
	if(myDart->qlHighScores[9].score>=myDart->qlTotalScores[player].score ||
	   (myDart->qlCurrentTypePlaces.count()<myDart->iMaxPlaceCount && myDart->qlAllPlaces.count()>myDart->iMaxPlaceCount) ||
	   (myDart->iGameMode==enNetwork && myDart->iCurrentPlayer!=player)) {
		lblName->hide();
		leName->hide();
		connect(btOk, SIGNAL(clicked()), this, SLOT(close()));
	} else {
		connect(btOk, SIGNAL(clicked()), this, SLOT(vClose()));
	}
	
//WORKAROUND for https://bugreports.qt.nokia.com/browse/QTBUG-691
//we resize the window so that the text fits (better)
#ifdef Q_OS_LINUX
	if(places.size()*7/width()*7>65) resize(width(),height()-65+places.size()*7/width()*7);
#endif
	
}

resultWindow::~resultWindow() {
}

void resultWindow::vClose() {
	double score=myDart->qlTotalScores[player].score/static_cast<double>(myDart->iMaxPlaceCount);
	QString name=leName->text();
	if(name=="") {
		close();
		return;
	}
	
	name=name.trimmed();
	while(name.contains("||")) name.replace("||", "| |");
	if(name.endsWith('|')) name.append(' ');
	
	int i=8;
	for(; i>-1 && score>myDart->qlHighScores[i].score; i--) {
		myDart->qlHighScores[i+1]=myDart->qlHighScores[i];
	}
	i++;
	
	highScoreEntry newEntry;
	newEntry.name=name;
	newEntry.score=score;
	
	myDart->qlHighScores[i]=newEntry;
	
	qDebug() << "[i] saved" << name<< score;
	
	myIO->vSaveHighScores(myDart->qlQcfxFiles[myDart->iCurrentQcf].id);
	
	*iHighlightHighScore=i;
	
	close();
}
