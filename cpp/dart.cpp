/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "dart.hpp"
#include "io.hpp"
#include "qtwin.h"
#include <QLabel>
#include <iostream>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QDebug>
#include <QTime>
#include <math.h>
#include <QDesktopWidget>
#include <time.h>
    
using namespace std;

io *clIO;

dart::dart(QMainWindow *parent) : QMainWindow(parent){
	iPaddingTop=0;
	iMarginTop=0;
	dZoomFactor=1;
	iMaxPlaceCount=10;
	iPlaceCount=0;
	iCurrentPlayer=0;
	qsCurrentPlaceType="land";
	
	iDelayNextCircle=200;
	iDelayBeforeShowingMark=500;
	iDelayBeforeNextPlayer=1000;
	
	//QtWin::enableBlurBehindWindow(this, true);
// 	QtWin::extendFrameIntoClientArea(this);
	
	srand(time(NULL));
	
	clIO = new io(this);

	setupUi(this);
	toolBar->setMovable(FALSE);
	iMarginTop=toolBar->height()+menubar->height(); //TODO put it in a suitable function
	
	resize(600,600+iMarginTop);
	
	lblMapBackground = new QLabel(this);
	lblMapBackground->setAlignment(Qt::AlignTop);
	lblMapBackground->setParent(centralwidget);
	lblMapBackground->show();
	lblMapBackground->setGeometry(0, 0, this->width(), this->width());
	lblMapBackground->setText(QString("<img src=\"/home/markus/Dokumente/GitHub/QeoDart/qcf/de/border.png\" height=\"%1\" width=\"%1\"/>").arg(600*dZoomFactor));
	
	lblMouseClickOverlay = new QMouseReleaseLabel(this);
	//mouseReleaseEvent 75 | 38 
	lblMouseClickOverlay->setParent(centralwidget); //we want the label to be placed under the toolbar
	lblMouseClickOverlay->setAlignment(Qt::AlignTop);
	lblMouseClickOverlay->show();
	lblMouseClickOverlay->setGeometry(0, 0, iGetWindowSize(), iGetWindowSize());
	qDebug()<<iGetWindowSize();
	
	clIO->iReadQcf("dummyfile");
// 	vDrawPoint(qlAllPlaces[0].x,qlAllPlaces[0].y);
// 	vShowAllPlaces();
	
	connect(actionQuit,SIGNAL (triggered()), this, SLOT(vClose()));
	connect(actionNew_Game,SIGNAL (triggered()), this, SLOT(vShowAllPlaces()));
	
	show();
	
// 	vSetNumberOfPlayers(2);
	iNumberOfPlayers=3;
	
	vSetGameMode(enLocal);
	
// 	clIO->iReadOsm("/home/markus/Dokumente/GitHub/QeoDart/cpp/test.svg");
	
}

dart::~dart(){
}

//draws distance circles around P(x|y) [unzoomed], using the saved click-coordinates of place n, iterating #count [recursion]
void dart::vDrawDistanceCircles(int x, int y, int n, int count) {
	// if(count*10 < dblGetDistance(x,y,QLscoreHistory[0][n].x(),QLscoreHistory[0][n].y())){ //TODO check all players
	if(count*10 < 55) { //TODO check all players
		mySleep(iDelayNextCircle);
		vDrawCircle(x,y,(count+1)*10,iCurrentPlayer);
		vDrawDistanceCircles(x,y,n,++count);
	}
}

void dart::vDrawCircle(int x, int y, int r, int player) {
	QLabel *circleLabel;
	circleLabel = new QCircleLabel(this,x,y,r,qlColorsOfPlayers[player],this);
	qlCircleLabels[player].append(circleLabel);
}

void dart::vRemoveAllCircles() {
	for(int i=0,max=qlCircleLabels.count();i<max;i++){
		qDebug()<<i;
		while(qlCircleLabels[i].count()!=0) {
			qDebug()<<"j"<<qlCircleLabels[i].count();
			delete qlCircleLabels[i][0];
			qlCircleLabels[i].removeAt(0);
		}
	}
}

void dart::vSetNumberOfPlayers(int n) {
	iNumberOfPlayers=n;
	qDebug() << "[i] iNumberOfPlayers" << iNumberOfPlayers;
	if(iNumberOfPlayers>15) qDebug() << "[w] very much players";
	
	if(qlPlayerLabels.count()>n) {
		
		for(int i=qlPlayerLabels.count()-1; i>iNumberOfPlayers-1; i--) {
			
// 			QList<QLabel*> *qlPlayerLabel=qlPlayerLabels[i];
			qDebug()<<"f"<<i;
			qDebug() << "fdddfff" << qlPlayerLabels[0].count();
			for(int j=0,max=qlPlayerLabels[i].count(); j<max; j++) {
				delete qlPlayerLabels[i][j];
				qDebug()<<"ff";
			}
			qlPlayerLabels[i].clear();
// 			~qlPlayerLabels[i];			//TODO we should do it somehow, shouldn't we?
			qlPlayerLabels.removeAt(i);
			
			qlScoreHistory[i].clear();
			qlScoreHistory.removeAt(i);
		}
		
	} else if(qlPlayerLabels.count()<n) {
		
		while(qlPlayerLabels.count()<iNumberOfPlayers){
			int i=qlPlayerLabels.count();
			qDebug()<<"player labels available"<<i;
			
			QLabel *lblScore;
			lblScore = new QLabel(this);
			gridLayout->addWidget(lblScore,i,0);
			lblScore->setText(QString(tr("<span>%1 Points &#8960; %2, %3</span>")).arg(0).arg(0).arg(0));
			
			QLabel *lblRating;
			lblRating = new QLabel(this);
			gridLayout->addWidget(lblRating,i,2);
			
			QList<QLabel*> qlPlayerLabel;
			qlPlayerLabel << lblScore << lblRating;
			qlPlayerLabels.append(qlPlayerLabel);
			
			QList<QLabel*> qlCircleLabel;
			qlCircleLabels.append(qlCircleLabel);
			
			QList<scoreHistory> qlHistory;
			qlScoreHistory.append(qlHistory);
			
			qlColorsOfPlayers.append(qcGetColorOfPlayer(qlPlayerLabels.count()-1));
		}
	}
	if(iNumberOfPlayers==1) {
		gridLayout->addWidget(lblCurrentPlace,1,0);
		gridLayout->addWidget(lblCurrentPlayer,1,4);
		lblComment->setText("");
		lblComment->show();
	} else {
		gridLayout->addWidget(lblCurrentPlace,iNumberOfPlayers,0);
		gridLayout->addWidget(lblCurrentPlayer,iNumberOfPlayers,4);
		lblComment->hide();
	}
	
	gridLayout->setGeometry(QRect(0, 0, 10, 10));
	gridLayout->setSpacing(1);
	gridLayout->setContentsMargins(0,0,0,0);
	
	resizeEvent(0); // the label's font size and iPaddingTop must be recalculated
// 	/*qDebug() << "ddddd " <<*/ qlPlayerLabels[0][0]->setText("te");
}

void dart::resizeEvent(QResizeEvent *event) {
	//maybe we want to preserve the shape of the window TODO use timer?
// 	int w=this->width(), h=this->height()-toolBar->height()-menubar->height(), n=this->width()<this->height()-toolBar->height()-menubar->height()?this->width():this->height()-toolBar->height()-menubar->height();
// 	cout << w << " " << h << endl;
// 	if(w!=h) resize(n,n+toolBar->height()+menubar->height());

// 	QRect qr availableGeometry(screenNumber(this));
// 	int availableHeight=qr.height();
// 	int availableWidth=qr.width();

	dZoomFactor=iGetWindowSize()/600.0; //TODO must be calculated in another way
	
	int fontSize=20*dZoomFactor<10 ? 10 : 20*dZoomFactor;
	iPaddingTop=0;
	int max=qlPlayerLabels.count();
	for(int i=0; i<max; i++) {
		for(int j=0,max=qlPlayerLabels[i].count(); j<max; j++) {
			qlPlayerLabels[i][j]->setStyleSheet(QString("color:%2;font-size:%1px;font-family:arial,sans-serif").arg(fontSize).arg(qcGetColorOfPlayer(i).name()));
		}
	}
	vRepaintCommonLabels();
	iPaddingTop=(max+1)*(fontSize+6);
	
	dZoomFactor=iGetWindowSize()/600.0;
	
	lblMouseClickOverlay->resize(600*dZoomFactor,600*dZoomFactor);
	lblMouseClickOverlay->move(0,iPaddingTop);
	
	lblMapBackground->resize(600*dZoomFactor,600*dZoomFactor);
	lblMapBackground->setText(QString("<img src=\"/home/markus/Dokumente/GitHub/QeoDart/qcf/de/border.png\" height=\"%1\" width=\"%1\"/>").arg(600*dZoomFactor));
	lblMapBackground->move(0,iPaddingTop);
	
	gridLayoutWidget->resize(600,gridLayoutWidget->height()); //TODO not working
	
	qDebug() << "[i] iPaddingTop" << iPaddingTop << "iMarginTop" << iMarginTop << "dZoomFactor" << dZoomFactor << "fontSize" << fontSize;
}

void dart::vRepaintCommonLabels() {
	int fontSize=20*dZoomFactor<10 ? 10 : 20*dZoomFactor;
	lblCurrentPlace->setStyleSheet(QString("color:%2;font-size:%1px;font-family:arial,sans-serif").arg(fontSize).arg(qcGetColorOfPlayer(iCurrentPlayer).name()));
	lblComment->setStyleSheet(QString("color:%2;font-size:%1px;font-family:arial,sans-serif").arg(fontSize).arg(qcGetColorOfPlayer(iCurrentPlayer).name()));
	lblCurrentRound->setStyleSheet(QString("color:%2;font-size:%1px;font-family:arial,sans-serif").arg(fontSize).arg(qcGetColorOfPlayer(iCurrentPlayer).name()));
	lblCurrentPlayer->setStyleSheet(QString("color:%2;font-size:%1px;font-family:arial,sans-serif").arg(fontSize).arg(qcGetColorOfPlayer(iCurrentPlayer).name()));
}

// draws a point at P(x|y) with the label name, and adds it to the list list
void dart::vDrawPoint(int x, int y, QList<QLabel*> &list, QString name) {
	QLabel *lblCurrentPlacePosition;
	lblCurrentPlacePosition = new QPointLabel(this, name, x, y, this);
	list.append(lblCurrentPlacePosition);
// 	lblCurrentPlace->setGeometry(x,y+iPaddingTop,50,50);
// 	lblCurrentPlace->setVisible(TRUE);
	qDebug() << "[i] drew point " << x << y << "+" << iMarginTop;
}

void dart::vShowAllPlaces() {
	for(int i=0, max=qlCurrentTypePlaces.count(); i<max; i++){ //TODO WITH?
		vDrawPoint(qlCurrentTypePlaces[i]->x,qlCurrentTypePlaces[i]->y,qlCircleLabels[0],qlCurrentTypePlaces[i]->name);
	}
}

void dart::vMouseClickEvent(int x, int y) {
	if(iAskForMode!=enPositions) return;
	
	x=iGetUnzoomed(x);
	y=iGetUnzoomed(y);
	vDrawPoint(x,y,qlCircleLabels[iCurrentPlayer]);
// 	vShowAllPlaces();
	scoreHistory score;
	score.x=x;
	score.y=y;
	score.diffPx=dGetDistanceInPxBetween(x,y,qlCurrentTypePlaces[iPlaceCount]->x,qlCurrentTypePlaces[iPlaceCount]->y); //TODO area
	score.diffKm=dGetDistanceInKm(score.diffPx);
	score.mark=dGetMark(score.diffPx);
	qlScoreHistory[iCurrentPlayer].append(score);
	
	mySleep(iDelayNextCircle);
	vDrawDistanceCircles(x, y, 0);
	
	if(iCurrentPlayer<iNumberOfPlayers-1) {
		iCurrentPlayer++;
		vRemoveAllCircles();
// 		delete qlPointLabels/*[iCurrentPlayer]*/[qlPointLabels.count()-1];
// 		qlPointLabels.removeAt(qlPointLabels.count()-1);
		
	}
}

int dart::iGetWindowSize() {
	return width()<height()-iMarginTop-iPaddingTop ? width() : height()-iMarginTop-iPaddingTop;
}

void dart::vClose() {
	close();
}

int dart::iGetUnzoomed(double x) {
	return x/dZoomFactor;
}

void dart::mySleep(int ms) {
	Q_ASSERT(QCoreApplication::instance());
	QTime timer;
	timer.start();
	do {
		QCoreApplication::processEvents(QEventLoop::AllEvents, ms);
		#ifdef Q_OS_UNIX
		struct timespec t;
		t.tv_sec  = 0;
		t.tv_nsec = 10000;
		nanosleep(&t,NULL);
		#endif
		#ifdef Q_OS_WIN32
		_sleep(10);
		#endif
	} while (timer.elapsed() < ms);
}

QColor dart::qcGetColorOfPlayer(int player) {
	int m=player%6, i=255/pow(2,player/6);
	QColor c=QColor(0,0,0,255);
	
	if(m==0 || m==3 || m==5) c.setBlue(i);
	if(m==1 || m==3 || m==4) c.setGreen(i);
	if(m==2 || m==4 || m==5) c.setRed(i);
	
// 	qDebug()<<c;
	
	return c;
}

void dart::vSetGameMode(enGameModes mode) {
	iGameMode=mode;
	
	vReset();
	
	switch(mode) {
		case enLocal:
			vSetNumberOfPlayers(iNumberOfPlayers);
			vNextRound();
			break;
	};
}

void dart::vReset() {
	vRemoveAllCircles();
	qlPlacesHistory.clear();
}

void dart::vNextRound() {
	int pn, i=0;
	do {
		pn = rand() % qlCurrentTypePlaces.count();
	} while(qlPlacesHistory.contains(pn) && i++<10);
	qlPlacesHistory.append(pn);
	
	qDebug() << "[i] next place:" << pn << qlCurrentTypePlaces[pn]->name << ++iPlaceCount << "/" << iMaxPlaceCount;
	
	switch(iGameMode) {
		case enLocal:
			lblCurrentPlace->setText(qlCurrentTypePlaces[pn]->name);
			lblCurrentRound->setText(QString(tr("Round %1")).arg(iPlaceCount));
			break;
	};
}

//returns the distance between P(a|b) and Q(x|y); a,b,x,y should be unzoomed
double dart::dGetDistanceInPxBetween(int a, int b, int x, int y) {
	return sqrt( pow(a-x,2) + pow(b-y,2) ); //thx Pythagoras
}

double dart::dGetDistanceInKm(int px) {
	return px*1; //TODO var
}

//calculate the mark (German system TODO other systems) using unzoomed distance
double dart::dGetMark(double distance) {
	double mark=distance/RADIUS; //TODO check radius
	if(mark<4) {
		return mark<1 ? 1 : mark;
	} else {
		mark=4+(mark-4)/2;
		return mark>6 ?  6 : mark;
	}
}
