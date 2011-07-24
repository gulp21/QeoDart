/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "dart.hpp"
#include "io.hpp"
#include <QLabel>
#include <iostream>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QDebug>
#include <QTime>
    
using namespace std;

io *clIO;

dart::dart(QMainWindow *parent) : QMainWindow(parent){
	iPaddingTop=0;
	iMarginTop=0;
	dZoomFactor=1;
	qsCurrentPlaceType="land";
	
	clIO = new io(this);

	setupUi(this);
	toolBar->setMovable(FALSE);
	iMarginTop=toolBar->height()+menubar->height(); //TODO put it in a suitable function
	
	resize(600,600+iPaddingTop+iMarginTop);
	
	qlMapBackground = new QLabel(this);
	qlMapBackground->setAlignment(Qt::AlignTop);
	qlMapBackground->setParent(centralwidget);
	qlMapBackground->show();
	qlMapBackground->setGeometry(0, 0, this->width(), this->width());
	qlMapBackground->setText(QString("<img src=\"/home/markus/Dokumente/GitHub/QeoDart/qcf/de/border.png\" height=\"%1\" width=\"%1\"/>").arg(600*dZoomFactor));
	
	qlMouseClickOverlay = new QMouseReleaseLabel(this);
	//mouseReleaseEvent 75 | 38 
	qlMouseClickOverlay->setParent(centralwidget); //we want the label to be placed under the toolbar
	qlMouseClickOverlay->setAlignment(Qt::AlignTop);
	qlMouseClickOverlay->show();
	qlMouseClickOverlay->setGeometry(0, 0, iGetWindowSize(), iGetWindowSize());
	qDebug()<<iGetWindowSize();


	clIO->iReadQcf("dummyfile");
// 	vDrawPoint(qlAllPlaces[0].x,qlAllPlaces[0].y);
// 	vShowAllPlaces();
	
	connect(actionQuit,SIGNAL (triggered()), this, SLOT(vRemoveAllCircles()));
	connect(actionNew_Game,SIGNAL (triggered()), this, SLOT(vShowAllPlaces()));
	
	show();
	
	vSetNumberOfPlayers(2);
	
	mySleep(2000);
	
	vSetNumberOfPlayers(3);
	
	mySleep(2000);
	
	vSetNumberOfPlayers(1);
	
	mySleep(2000);
	
	vSetNumberOfPlayers(2);
	
// 	QList<QList<QString> > field;
// // 	for (int i = 0; i < 4; i++)
// // 	{
// 		QList<QString> list;
// 		field.append(list);
// // 		for (int j = 0; j < 4; j++)
// // 		{
// 			field[0].append("bla");
// 			qDebug()<<field[0][0];
// // 		}
// // 	}
	
// 	clIO->iReadOsm("/home/markus/Dokumente/GitHub/QeoDart/cpp/test.svg");
	

}

dart::~dart(){
}

//draws distance circles around P(x|y) [unzoomed], using the saved click-coordinates of place n, iterating #count [recursion]
void dart::vDrawDistanceCircles(int x, int y, int n, int count) {
	// if(count*10 < dblGetDistance(x,y,QLscoreHistory[0][n].x(),QLscoreHistory[0][n].y())){ //TODO check all players
	if(count*10 < 55) { //TODO check all players
		vDrawCircle(x,y,(count+1)*10);
		
		qDebug()<<count;
		vDrawDistanceCircles(x,y,n,++count);
	}
}

void dart::vDrawCircle(int x, int y, int r) {
	QLabel *circleLabel;
	circleLabel = new QCircleLabel(this,x,y,r,this);
	qlCircleLabels << circleLabel;
}

void dart::vRemoveAllCircles() {
	while(qlCircleLabels.count()>0) {
		delete qlCircleLabels[0];
		qlCircleLabels.removeAt(0);
	}
}

void dart::vSetNumberOfPlayers(int n) {
	iNumberOfPlayers=n;
	qDebug() << "[i] iNumberOfPlayers" << iNumberOfPlayers;
	
	if(qlPlayerLabels.count()>n) {
		
		for(int i=qlPlayerLabels.count()-1; i>iNumberOfPlayers-1; i--) {
			
// 			QList<QLabel*> *qlPlayerLabel=qlPlayerLabels[i];
			qDebug()<<"f"<<i;
			qDebug() << "fdddfff" << qlPlayerLabels[0].count();
			for(int j=0,max=qlPlayerLabels[i].count(); j<max; j++) {
				
				qDebug()<<"ff";
				delete qlPlayerLabels[i][j];
				qDebug()<<"ff";
			}
			qlPlayerLabels[i].clear();
// 			~qlPlayerLabels[i];			//TODO we should do it somehow, shouldn't we?
			qlPlayerLabels.removeAt(i);
		}
		
	} else if(qlPlayerLabels.count()<n) {
		
		while(qlPlayerLabels.count()<iNumberOfPlayers){
			
			qDebug()<<qlPlayerLabels.count();
			
			QLabel *qlPlayer;
			qlPlayer = new QLabel(this);
			
			gridLayout->addWidget(qlPlayer);
			qlPlayer->setText(QString(tr("Player %1")).arg(qlPlayerLabels.count()+1));
			
			QList<QLabel*> qlPlayerLabel;
			qlPlayerLabel << qlPlayer;
			
			for(int i=0,max=qlPlayerLabel.count(); i<max; i++) {
				qlPlayerLabel[i]->setStyleSheet(QString("color:blue;font-size:%1px;font-family:arial,sans-serif").arg(20*dZoomFactor));
				qlPlayerLabel[i]->show();
			}
			
			qlPlayerLabels.append(qlPlayerLabel);
			qDebug() << "ddd " << qlPlayerLabels[0].count();
		}
	}
	gridLayout->setGeometry(QRect(0, 0, 10, 10));
	gridLayout->setSpacing(1);
	gridLayout->setContentsMargins(0,0,0,0);
// 	/*qDebug() << "ddddd " <<*/ qlPlayerLabels[0][0]->setText("te");
}

void dart::resizeEvent(QResizeEvent *event) {
	//maybe we want to preserve the shape of the window TODO use timer?
// 	int w=this->width(), h=this->height()-toolBar->height()-menubar->height(), n=this->width()<this->height()-toolBar->height()-menubar->height()?this->width():this->height()-toolBar->height()-menubar->height();
// 	cout << w << " " << h << endl;
// 	if(w!=h) resize(n,n+toolBar->height()+menubar->height());
	dZoomFactor=iGetWindowSize()/600.0;
	
	qlMouseClickOverlay->resize(600*dZoomFactor,600*dZoomFactor);
	qlMapBackground->resize(600*dZoomFactor,600*dZoomFactor);
	qlMapBackground->setText(QString("<img src=\"/home/markus/Dokumente/GitHub/QeoDart/qcf/de/border.png\" height=\"%1\" width=\"%1\"/>").arg(600*dZoomFactor));
	
	int fontSize=20*dZoomFactor<10 ? 10 : 20*dZoomFactor;
	for(int i=0,max=qlPlayerLabels.count(); i<max; i++) {
			
		for(int j=0,max=qlPlayerLabels[i].count(); j<max; j++) {
			qlPlayerLabels[i][j]->setStyleSheet(QString("color:blue;font-size:%1px;font-family:arial,sans-serif").arg(fontSize));
		}
	}
	
	qDebug() << "[i] iPaddingTop" << iPaddingTop << "iMarginTop" << iMarginTop << "dZoomFactor" << dZoomFactor;
}

void dart::vDrawPoint(int x, int y, QString name) {
	QLabel *qlCurrentPlace;
	qlCurrentPlace = new QPointLabel(this, name, x, y, this);
	qlPointLabels.append(qlCurrentPlace);
// 	qlCurrentPlace->setGeometry(x,y+iPaddingTop,50,50);
// 	qlCurrentPlace->setVisible(TRUE);
	qDebug() << "[i] drew point " << x << y << "+" << iMarginTop;
}

void dart::vShowAllPlaces() {
	for(int i=0, max=qlCurrentTypePlaces.count(); i<max; i++){ //TODO WITH?
		vDrawPoint(qlCurrentTypePlaces[i]->x,qlCurrentTypePlaces[i]->y,qlCurrentTypePlaces[i]->name);
	}
}

void dart::vMouseClickEvent(int x, int y) {
	vDrawPoint(iGetUnzoomed(x),iGetUnzoomed(y),"s"); //TODO function get unzoomed
// 	vShowAllPlaces();
	
	vDrawPoint(44,56);
	vDrawDistanceCircles(iGetUnzoomed(x), iGetUnzoomed(y), 0);
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
