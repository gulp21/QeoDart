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
    
using namespace std;

io *clIO;

dart::dart(QMainWindow *parent) : QMainWindow(parent){
	iPaddingTop=0;
	dZoomFactor=1;
	qsCurrentPlaceType="land";
	
	clIO = new io(this);

	setupUi(this);
	toolBar->setMovable(FALSE);
	iPaddingTop=toolBar->height()+menubar->height(); //TODO put it in a suitable function
	
	resize(600,600+iPaddingTop);
	
	qlMapBackground = new QLabel(this);
	qlMapBackground->setAlignment(Qt::AlignTop);
	qlMapBackground->setParent(centralwidget);
	qlMapBackground->show();
	qlMapBackground->setGeometry(0, 0, this->width(), this->width());
	qlMapBackground->setText(QString("<img src=\"/home/markus/Dokumente/GitHub/QeoDart/qcf/de/border.png\" height=\"%1\" width=\"%1\"/>").arg(600*dZoomFactor));
	
	qlMouseClickOverlay = new QMouseReleaseLabel(this);
	qlMouseClickOverlay->setParent(centralwidget); //we want the label to be placed under the toolbar
	qlMouseClickOverlay->setAlignment(Qt::AlignTop);
	qlMouseClickOverlay->show();
	qlMouseClickOverlay->setGeometry(0, 0, iGetWindowSize(), iGetWindowSize());
	qDebug()<<iGetWindowSize();


	// circleLabel = new QCircleLabel(this);
	// circleLabel->setGeometry(50,50,50,50);

	clIO->iReadQcf("dummyfile");
// 	vDrawPoint(qlAllPlaces[0].x,qlAllPlaces[0].y);
	vShowAllPlaces();

}

dart::~dart(){
}


//draws distance circles around P(x|y), using the saved click-coordinates of place n, iterating #count [recursion]
void dart::vDrawCircle(int x, int y, int n, int count){
// if(count*10 < dblGetDistance(x,y,QLscoreHistory[0][n].x(),QLscoreHistory[0][n].y())){ //TODO check all players
if(count*10 < 55){ //TODO check all players
// this->setAttribute(Qt::WA_PaintOutsidePaintEvent );
// QPainter painter(this); painter.setPen(Qt::blue);
// painter.drawEllipse(50,50,20,20);
// if(count<7) vDrawCircle(x,y,n,count++);


// circleLabel = new QCircleLabel(this);
//
//
// circleLabel->setGeometry(40, 40, 240, 240);
//
// circleLabel1 = new QCircleLabel(this);
//
// circleLabel1->setGeometry(50, 50, 200, 200);
//

}
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
	
	
	qDebug() << "[i] iPaddingTop" << iPaddingTop << "dZoomFactor" << dZoomFactor;
}

void dart::vDrawPoint(int x, int y, QString name) {
	QLabel *qlCurrentPlace;
	qlCurrentPlace = new QPointLabel(this, name, x, y, this);
	qlPointLabels.append(qlCurrentPlace);
// 	qlCurrentPlace->setGeometry(x,y+iPaddingTop,50,50);
	qDebug() << "[i] drew point " << x << y << "+" << iPaddingTop;
}

void dart::vShowAllPlaces() {
	int max=qlCurrentTypePlaces.count();
	for(int i=0;i<max;i++){ //TODO WITH?
		vDrawPoint(qlCurrentTypePlaces[i]->x,qlCurrentTypePlaces[i]->y,qlCurrentTypePlaces[i]->name);
	}
}

void dart::vMouseClickEvent(int x, int y) {
	
}

int dart::iGetWindowSize() {
	return width()<height()-iPaddingTop ? width() : height()-iPaddingTop;
}
