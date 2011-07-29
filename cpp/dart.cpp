/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "dart.hpp"
#include "io.hpp"
#include "qtwin.h"
#include "dialogs.hpp"
#include <QLabel>
#include <iostream>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QDebug>
#include <QTime>
#include <math.h>
#include <QDesktopWidget>
#include <time.h>
#include <QDialog>

using namespace std;

io *clIO;

dart::dart(QMainWindow *parent) : QMainWindow(parent){
	
	qlImageLayers << "background" << "borders" << "rivers" << "elevations";
	
	qlComments << tr("Very Good!") << tr("Super!") << tr("Very Fine!")
	           << tr("Well Done!") << tr("Good!") << tr("That was good!")
	           << tr("Well.") << tr("That was OK.") << tr("That was reasonable.")
	           << tr("Oh boy!") << tr("Not really…") << tr("Not quite…")
	           << tr("Completely Wrong!") << tr("That wasn't much of a hit…") << tr("Missed completely!")
	           << tr("Read wrongly?") << tr("Clicked wrongly?") << tr("D'oh!");
	
	iPaddingTop=0;
	iMarginTop=0;
	dZoomFactor=1;
	iMaxPlaceCount=3;
	iPlaceCount=0;
	iCurrentPlayer=0;
	iAskForMode=enPositions;
	iNumberOfPlayers=1;
	qsCurrentPlaceType="state";
	bAcceptingClickEvent=TRUE;
	dPxToKm=1;
	iCurrentQcf=0;
	iScoreAreaMode=1;
	
	iDelayNextCircle=200;
	iDelayBeforeShowingMark=500;
	iDelayBeforeNextPlayer=1000;
	iDelayBeforeNextPlace=2000;
	
	srand(time(NULL));
	
	clIO = new io(this);

	setupUi(this);
	toolBar->setMovable(FALSE);
	iMarginTop=toolBar->height()+menubar->height(); //TODO put it in a suitable function
	
	for(int i=0; i<4; i++) {
		QLabel *lblMap = new QLabel(this);
		lblMap->setAlignment(Qt::AlignTop);
		lblMap->setParent(centralwidget);
		lblMap->show();
		lblMap->setGeometry(0, 0, this->width(), this->width());
		qlMapLayers.append(lblMap);
	}
	
	lblMouseClickOverlay = new QMouseReleaseLabel(this);
	//mouseReleaseEvent 75 | 38 
	lblMouseClickOverlay->setParent(centralwidget); //we want the label to be placed under the toolbar
	lblMouseClickOverlay->setAlignment(Qt::AlignTop);
	lblMouseClickOverlay->show();
	lblMouseClickOverlay->setGeometry(0, 0, iGetWindowSize(), iGetWindowSize());
	qDebug()<<iGetWindowSize();
	
	connect(actionQuit,SIGNAL (triggered()), this, SLOT(vClose()));
	actionQuit->setIcon(QIcon::fromTheme("application-exit"));
	connect(actionNew_Game,SIGNAL (triggered()), this, SLOT(vNewGame()));
	actionNew_Game->setIcon(QIcon::fromTheme("document-new"));
	connect(actionFind_Place,SIGNAL (triggered()), this, SLOT(vShowAllPlaces()));
	actionFind_Place->setIcon(QIcon::fromTheme("edit-find"));
	connect(action100,SIGNAL (triggered()), this, SLOT(vResize()));
	connect(actionAbout_Qt,SIGNAL (triggered()), qApp, SLOT(aboutQt()));
	
	if(clIO->iFindQcf()==0) {
		qDebug() << "[E] No valid qcfx files found, exiting";
		QMessageBox msgBox;
		msgBox.setText(tr("Sorry, no valid qcfx files could be found."));
		msgBox.setInformativeText(tr("You might want to add a file through Maps → Add map")); // TODO -> symbol
		msgBox.exec();
	} else {
		clIO->iReadQcf(qlQcfxFiles[iCurrentQcf]);
	}
	
	show();
	
	gridLayout->setSpacing(1);
	
	vSetGameMode(enLocal);
	
	vRepaintCommonLabels();
	vRepaintPlayerLabels();
	
	gridLayout->addWidget(lblCurrentRound,0,4);
	
	vResize(1); // TODO saved value?
}

dart::~dart(){
	vRemoveAllCircles();
	vRemoveAllCommonPoints();
	vSetNumberOfPlayers(0);
}

//draws distance circles using the saved click-coordinates of place n, iterating #count [recursion]
void dart::vDrawDistanceCircles(int n, int count) {
	bool drewCircle=FALSE;
	
	for(int i=0; i<iNumberOfPlayers; i++) {//draw circles for every player
		if(count*RADIUS+3*PENWIDTH < qlScoreHistory[i][n-1].diffPx) {
			vDrawCircle(qlScoreHistory[i][n-1].x,qlScoreHistory[i][n-1].y,(count+1)*RADIUS,i);
			drewCircle=TRUE;
		}
	}
	
	if(drewCircle && count<7) {
		mySleep(iDelayNextCircle);
		vDrawDistanceCircles(n,++count);
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
			delete qlCircleLabels[i][0];
			qlCircleLabels[i].removeAt(0);
		}
	}
}

void dart::vRemoveAllCommonPoints() {
	while(qlPointLabels.count()!=0){
		delete qlPointLabels[0];
		qlPointLabels.removeAt(0);
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
			
			qlScoreHistory[i].clear(); //TODO must delete sub-lists?
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
                gridLayout->addWidget(lblComment,1,2);
		lblComment->setText("");
		lblComment->show();
	} else {
		gridLayout->addWidget(lblCurrentPlace,iNumberOfPlayers,0);
		gridLayout->addWidget(lblCurrentPlayer,iNumberOfPlayers,4);
		lblComment->hide();
	}
	
// 	gridLayout->setGeometry(QRect(0, 0, 10, 10));
// 	gridLayout->setSpacing(1);
// 	gridLayout->setContentsMargins(0,0,0,0);
	gridLayout->addItem(spGridLayoutVertical,iNumberOfPlayers+1,0);
	
	resizeEvent(0); // the label's font size and iPaddingTop must be recalculated
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
	
	vRepaintPlayerLabels();
	vRepaintCommonLabels();
	int fontSize=iGetFontSize();
	iPaddingTop=(qlPlayerLabels.count()+1)*(fontSize+6);
	
	dZoomFactor=iGetWindowSize()/600.0;
	
	lblMouseClickOverlay->resize(600*dZoomFactor,600*dZoomFactor);
	lblMouseClickOverlay->move(0,iPaddingTop);
	
	vRepaintMap();
	
	gridLayoutWidget->setGeometry(QRect(0,0,600*dZoomFactor+1,111));

	qDebug() << "[i] iPaddingTop" << iPaddingTop << "iMarginTop" << iMarginTop << "dZoomFactor" << dZoomFactor << "fontSize" << fontSize;
}

void dart::vResize(double dNewZoomFactor) {
	if(QObject::sender()==action100) { // this is unneceassary as there's only 1 caller
		dNewZoomFactor=1;
	}
        
        showNormal();
	
	dZoomFactor=dNewZoomFactor;
	int fontSize=iGetFontSize();
	iPaddingTop=(qlPlayerLabels.count()+1)*(fontSize+6);
	resize(600*dZoomFactor,iMarginTop+iPaddingTop+600*dZoomFactor);
}

void dart::vRepaintMap() {
	QString path=qlQcfxFiles[iCurrentQcf].left(qlQcfxFiles[iCurrentQcf].length()-5);
	
	for(int i=0; i<4; i++) {
		qlMapLayers[i]->resize(600*dZoomFactor,600*dZoomFactor);
		qlMapLayers[i]->setText(QString("<img src=\"%2/%3.png\" height=\"%1\" width=\"%1\"/>").arg(600*dZoomFactor).arg(path).arg(qlImageLayers[i]));
		qlMapLayers[i]->move(0,iPaddingTop);
	}
}

//repaints all labels which are player-specific
void dart::vRepaintPlayerLabels() {
	int fontSize=iGetFontSize();
	for(int i=0,max=qlPlayerLabels.count(); i<max; i++) {
		for(int j=0,max=qlPlayerLabels[i].count(); j<max; j++) {
			qlPlayerLabels[i][j]->setStyleSheet(QString("color:%2;font-size:%1px;font-family:arial,sans-serif").arg(fontSize).arg(qcGetColorOfPlayer(i).name()));
		}
	}
}

//repaint all label which are not player-specific
void dart::vRepaintCommonLabels() {
	int fontSize=iGetFontSize();
	lblCurrentPlace->setStyleSheet(QString("color:%2;font-size:%1px;font-family:arial,sans-serif").arg(fontSize).arg(qcGetColorOfPlayer(iCurrentPlayer).name()));
	lblComment->setStyleSheet(QString("color:%2;font-size:%1px;font-family:arial,sans-serif").arg(fontSize).arg(qcGetColorOfPlayer(iCurrentPlayer).name()));
	lblCurrentRound->setStyleSheet(QString("color:%2;font-size:%1px;font-family:arial,sans-serif").arg(fontSize).arg(qcGetColorOfPlayer(iCurrentPlayer).name()));
	lblCurrentPlayer->setStyleSheet(QString("color:%2;font-size:%1px;font-family:arial,sans-serif").arg(fontSize).arg(qcGetColorOfPlayer(iCurrentPlayer).name()));
	lblCurrentPlayer->setText(QString(tr("Player %1")).arg(iCurrentPlayer+1));
}

int dart::iGetFontSize() {
	return 20*dZoomFactor<10 ? 10 : 20*ZoomFactor;
}

// draws a point at P(x|y) with the label name, and adds it to the list list
void dart::vDrawPoint(int x, int y, QList<QLabel*> &list, QString name, QColor color) {
	QLabel *lblCurrentPlacePosition;
	lblCurrentPlacePosition = new QPointLabel(this, name, x, y, color, this);
	list.append(lblCurrentPlacePosition);
// 	lblCurrentPlace->setGeometry(x,y+iPaddingTop,50,50);
// 	lblCurrentPlace->setVisible(TRUE);
	qDebug() << "[i] drew point " << x << y << "+" << iMarginTop;
}
void dart::vDrawPoint(int x, int y, QList<QLabel*> &list, QColor color, QString name) {
	vDrawPoint(x, y, list, name, color);
}

// draws the click positions of all players for round n
void dart::vDrawClickPositions(int n) {
	for(int i=0; i<iNumberOfPlayers; i++) { //draw circles for every player
		vDrawPoint(qlScoreHistory[i][n-1].x,qlScoreHistory[i][n-1].y,qlCircleLabels[i],qlColorsOfPlayers[i]);
	}
}

void dart::vShowAllPlaces() {
	for(int i=0, max=qlCurrentTypePlaces.count(); i<max; i++){ //TODO WITH?
		vDrawPoint(qlCurrentTypePlaces[i]->x,qlCurrentTypePlaces[i]->y,qlPointLabels,qlCurrentTypePlaces[i]->name);
	}
}

void dart::vMouseClickEvent(int x, int y) {
	if(!bAcceptingClickEvent) return;
	bAcceptingClickEvent=FALSE;
	if(iAskForMode!=enPositions) return;
	
	x=iGetUnzoomed(x);
	y=iGetUnzoomed(y);
	vDrawPoint(x,y,qlCircleLabels[iCurrentPlayer],qlColorsOfPlayers[iCurrentPlayer]);
// 	vShowAllPlaces();
	scoreHistory score;
	score.x=x;
	score.y=y;
	score.diffPxArea=dGetDistanceInPx(x,y,iPlaceCount-1); // respects area // TODO what about shown distance?
	score.diffPx=dGetDistanceInPxBetween(x,y,qlCurrentTypePlaces[qlPlacesHistory[iPlaceCount-1]]->x,qlCurrentTypePlaces[qlPlacesHistory[iPlaceCount-1]]->y);
	score.diffKm=dGetDistanceInKm(score.diffPxArea);
	score.mark=dGetMarkFromDistance(score.diffPxArea);
	score.score=dGetScore(score.mark);
	qlScoreHistory[iCurrentPlayer].append(score);
	
	qlTotalScores[iCurrentPlayer].score+=score.score;
	qlTotalScores[iCurrentPlayer].mark=dGetAverageMarkOfPlayer(iCurrentPlayer);
	
	qDebug() << "Score for Player" << iCurrentPlayer << score.diffPx << "px" << score.diffPxArea << "px (area)" << score.diffKm << "km" << score.score << "p"  << score.mark << "TOTAL" << qlTotalScores[iCurrentPlayer].score << "p" << qlTotalScores[iCurrentPlayer].mark;
	
	if(iCurrentPlayer<iNumberOfPlayers-1) { // next player
		
		mySleep(iDelayBeforeNextPlayer);
		iCurrentPlayer++;
		vRemoveAllCircles();
		qDebug()<<"f";
		vRepaintPlayerLabels();
		vRepaintCommonLabels();
		
		bAcceptingClickEvent=TRUE;
		
	} else { // show results
		
		iCurrentPlayer=0;
		
		vRemoveAllCircles();
		vDrawClickPositions(iPlaceCount);
		mySleep(iDelayNextCircle);
		vDrawDistanceCircles(iPlaceCount, 0);
		
		// show real position
		vShowCurrentPlace();
		
		vShowScores();
                
                vShowComment();
		
		mySleep(iDelayBeforeNextPlace);
                
                lblComment->setText("");
		
		vShowTotalScores();
		
		vRemoveAllCircles();
		vRemoveAllCommonPoints();
		
		vResetScoreLabels();
		
		vRepaintCommonLabels();
		
		vNextRound();
		
	}
}

void dart::vShowComment() {
        if(iNumberOfPlayers==1) {
		int i = rand() % 3 + 3*(static_cast<int>(qlScoreHistory[0][iPlaceCount-1].mark)-1);
		qDebug() << qlScoreHistory[0][iPlaceCount-1].mark << i;
                lblComment->setText(qlComments[i]);
        }
}

void dart::vShowResultWindows() { // TODO all players
	bAcceptingClickEvent=FALSE;
	
	for(int i=0,max=qlPlacesHistory.count(); i<max; i++) {
		vDrawPoint(qlCurrentTypePlaces[qlPlacesHistory[i]]->x, qlCurrentTypePlaces[qlPlacesHistory[i]]->y, qlPointLabels, qlCurrentTypePlaces[qlPlacesHistory[i]]->name);
	}
	
	for(int i=0; i<iNumberOfPlayers; i++) {
		resultWindow dialog(this,i);
		dialog.exec();
	}
	
	iPlaceCount=0; // needed for quit?-dialog
}

void dart::vResetScoreLabels() {
	for(int i=0; i<iNumberOfPlayers; i++) { // reset score labels of each player
		qlPlayerLabels[i][1]->setText("");
	}
}

void dart::vShowCurrentPlace() {
	vDrawPoint(qlCurrentTypePlaces[qlPlacesHistory[iPlaceCount-1]]->x,qlCurrentTypePlaces[qlPlacesHistory[iPlaceCount-1]]->y,qlPointLabels);
}

void dart::vShowScores() {
	for(int i=0; i<iNumberOfPlayers; i++) { // show score for each player
		qlPlayerLabels[i][1]->setText(QString(tr("Missed by %1 km (%2) +%3")).arg(qlScoreHistory[i][iPlaceCount-1].diffKm,0,'f',1).arg(qlScoreHistory[i][iPlaceCount-1].mark,0,'f',1).arg(qlScoreHistory[i][iPlaceCount-1].score));
	}
}

void dart::vShowTotalScores() {
	for(int i=0; i<qlPlayerLabels.count(); i++) { // show score for each player
		qlPlayerLabels[i][0]->setText(QString(tr("<span>%1 Points &#8960; %2, %3</span>")).arg(qlTotalScores[i].score).arg(dGetAverageScoreOfPlayer(i),0,'f',1).arg(qlTotalScores[i].mark,0,'f',1));
	}
}

int dart::iGetWindowSize() {
	return width()<height()-iMarginTop-iPaddingTop ? width() : height()-iMarginTop-iPaddingTop;
}

void dart::vClose() {
	close();
}

void dart::closeEvent(QCloseEvent *event) {
	if(iPlaceCount>1 ) {
		QMessageBox msgBox;
		msgBox.setWindowTitle(tr("Quit QeoDart"));
		msgBox.setText(tr("Do you really want to quit QeoDart?"));
		msgBox.setInformativeText(tr("Your current score will be lost."));
		msgBox.setStandardButtons(QMessageBox::Yes	 | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		if(msgBox.exec()==QMessageBox::Yes) event->accept();
		else event->ignore();
	} else {
		event->accept();
	}
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
	
	vResetForNewGame();
	
	switch(mode) {
		case enLocal:
			vSetNumberOfPlayers(iNumberOfPlayers);
			vNextRound();
			break;
	};
}

void dart::vResetForNewGame() {
	vRemoveAllCircles();
	vRemoveAllCommonPoints();
	qlPlacesHistory.clear();
	
	for(int i=qlScoreHistory.count()-1; i>-1; i--) {
		qlScoreHistory[i].clear(); //TODO must delete sub-lists?
	}
	
	qlTotalScores.clear();
	for(int i=0; i<iNumberOfPlayers; i++) {
		totalScore ts;
		ts.score=0;
		ts.mark=0;
		qlTotalScores.append(ts);
	}
	iPlaceCount=0;
	
	vShowTotalScores();
}

void dart::vNewGame() {
	vSetNumberOfPlayers(iNumberOfPlayers);
	vSetGameMode(iGameMode);
}

void dart::vNextRound() {
	iCurrentPlayer=0;
	
	if(iPlaceCount==iMaxPlaceCount) {
		qDebug() << "What shall we do?";
		vShowResultWindows();
		return;
	}
	
	int pn, i=0;
	do {
		pn = rand() % qlCurrentTypePlaces.count();
	} while(qlPlacesHistory.contains(pn) && i++<10);
	qlPlacesHistory.append(pn);
	
	qDebug() << "[i] next place:" << pn << qlCurrentTypePlaces[pn]->name << ++iPlaceCount << "/" << iMaxPlaceCount;
	
	switch(iAskForMode) {
		case enPositions:
			bAcceptingClickEvent=TRUE;
			break;
	}
	
	switch(iGameMode) {
		case enLocal:
			lblCurrentPlace->setText(qlCurrentTypePlaces[pn]->name);
			lblCurrentRound->setText(QString(tr("Place %1 of %2")).arg(iPlaceCount).arg(iMaxPlaceCount));
			break;
	};
}

//returns the distance between P(a|b) and Q(x|y); a,b,x,y should be unzoomed
double dart::dGetDistanceInPxBetween(int a, int b, int x, int y) {
	qDebug()<<"!!!!"<<a<<b<<x<<y << sqrt( pow(a-x,2) + pow(b-y,2) );
	return sqrt( pow(a-x,2) + pow(b-y,2) ); //thx Pythagoras
}

//returns the distance between P(a|b) and place #n [>=0], respecting iScoreAreaMode
double dart::dGetDistanceInPx(int a, int b, int n) {
	int x=qlCurrentTypePlaces[qlPlacesHistory[n]]->x;
	int y=qlCurrentTypePlaces[qlPlacesHistory[n]]->y;
	int dim2x=qlCurrentTypePlaces[qlPlacesHistory[n]]->dimx*(iScoreAreaMode/2.0)/2;
	int dim2y=qlCurrentTypePlaces[qlPlacesHistory[n]]->dimx*(iScoreAreaMode/2.0)/2;
	
	if(a>x+dim2x) a-=dim2x;
	else if(a<x-dim2x) a+=dim2x;
	else a=x;
	
	if(b>y+dim2y) b-=dim2y;
	else if(b<y-dim2y) b+=dim2y;
	else b=y;
	
	return dGetDistanceInPxBetween(a,b,x,y);
}

double dart::dGetDistanceInKm(double px) {
	return px*dPxToKm;
}

//calculate the mark (German system TODO other systems) using unzoomed distance in px
double dart::dGetMarkFromDistance(double distance) {
	if(distance>1) distance--; // a difference of 1px is OK 
	else distance=0;

	double mark=distance/RADIUS;
	if(mark<4) {
		return mark<1 ? 1 : mark;
	} else {
		mark=4+(mark-4)/2;
		return mark>6 ?  6 : mark;
	}
}

double dart::dGetScore(double mark) {
	double score;
	cout<<-16.66*mark+116.66; // I really don't know why, but w/o this line the function never returns 100
	if(mark<4) score=-16.66*mark+116.66;
	else score=-25*mark+150;
	if(score<0) score=0;
	return score;
}

double dart::dGetMarkFromScore(double score) {
	if(score>=50) return (score-116.66)/-16.66;
	else return (score-150)/-25;
}

double dart::dGetAverageMarkOfPlayer(int player) {
	return dGetMarkFromScore(dGetAverageScoreOfPlayer(player));
}

double dart::dGetAverageScoreOfPlayer(int player) {
	if(iPlaceCount<1) return 0;
	return qlTotalScores[player].score/iPlaceCount;
}
