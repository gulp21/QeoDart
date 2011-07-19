/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "dart.hpp"
#include "io.hpp"
#include <qpainter.h>
#include <QLabel>
#include <iostream>
#include <QMouseEvent>
#include <QResizeEvent>
    
using namespace std;
const int RADIUS=10;
 
dart::dart(QMainWindow *parent) : QMainWindow(parent){
	setupUi(this);
	
	resize(600,600+toolBar->height()+menubar->height());
	
	toolBar->setMovable(FALSE);
	
	vDrawCircle(0,0,0);
}
 
dart::~dart(){
} 

QLabel *circleLabel;
QLabel *circleLabel1;
QLabel *MouseLabel1;
class QCircleLabel : public QLabel {
	public:
		QCircleLabel(QWidget *parent = 0, Qt::WindowFlags f = 0) :
		QLabel(parent, f) {}
// 		QCircleLabel(const QString &text, QWidget *parent = 0,
// 		Qt::WindowFlags f = 0) : QLabel(text, parent, f) {}
		~QCircleLabel() {}
		
		void paintEvent(QPaintEvent *event)
		{
			const int PENWIDTH=5;
			QPainter p(this);
			QPen pen;
			pen.setWidth(PENWIDTH);
			pen.setColor(QColor(0,0,255, 255-width()));
			p.setRenderHint(QPainter::Antialiasing);
			p.setPen(pen);
			p.drawEllipse(PENWIDTH, PENWIDTH, width()-2*PENWIDTH, height()-2*PENWIDTH);
			p.end();
			QLabel::paintEvent(event);
		}
};

class QMouseReleaseLabel : public QLabel {
	public:
		QMouseReleaseLabel(QWidget *parent = 0, Qt::WindowFlags f = 0) :
		QLabel(parent, f) {}/*
		QMouseReleaseLabel(const QString &text, QWidget *parent = 0,
		Qt::WindowFlags f = 0) : QLabel(text, parent, f) {}*/
		~QMouseReleaseLabel() {}
		
		void mouseReleaseEvent(QMouseEvent * event) {
			cout << event->x() << " - " << event->y() << endl;
		}
};

//draws distance circles around P(x|y), using the saved click-coordinates of place n, iteratin #count [recursion]
void dart::vDrawCircle(int x, int y, int n, int count){
// 	if(count*10 < dblGetDistance(x,y,QLscoreHistory[0][n].x(),QLscoreHistory[0][n].y())){ //TODO check all players
	if(count*10 < 55){ //TODO check all players
// 		this->setAttribute(Qt::WA_PaintOutsidePaintEvent );
// 		QPainter painter(this);     painter.setPen(Qt::blue);
// 		painter.drawEllipse(50,50,20,20);
// 		if(count<7) vDrawCircle(x,y,n,count++);
		
		
// 		circleLabel = new QCircleLabel(this);
// 		
// 		
// 		circleLabel->setGeometry(40, 40, 240, 240);
// 		
// 		circleLabel1 = new QCircleLabel(this);
// 		
// 		circleLabel1->setGeometry(50, 50, 200, 200);
// 		
		MouseLabel1 = new QMouseReleaseLabel(this);
		
// 		
		
		MouseLabel1->setParent(centralwidget); //we want the images to be placed under the toolbar
		MouseLabel1->setAlignment(Qt::AlignTop);
		MouseLabel1->show();
		MouseLabel1->setGeometry(0, 0, this->width(), this->width());
// 		MouseLabel1->
	}
}

void dart::resizeEvent ( QResizeEvent * event ) {
	MouseLabel1->setText(QString("<img src=\"/home/markus/Dokumente/GitHub/QeoDart/qcf/de/border.png\" height=\"%1\" width=\"%1\"/>").arg(this->width()<this->height()-toolBar->height()-menubar->height()?this->width():this->height()-toolBar->height()-menubar->height()));
	MouseLabel1->setGeometry(0, 0, this->width(), this->height());
	//maybe we want to preserve the shape of the window
// 	int w=this->width(), h=this->height()-toolBar->height()-menubar->height(), n=this->width()<this->height()-toolBar->height()-menubar->height()?this->width():this->height()-toolBar->height()-menubar->height();
// 	cout << w << " " << h << endl;
// 	if(w!=h) resize(n,n+toolBar->height()+menubar->height());
}
