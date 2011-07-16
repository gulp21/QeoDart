/*
QeoDart Copyright (C) 2010 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "dart.hpp"
#include <qpainter.h>
    #include <QLabel>
    
    
			const int RADIUS=10;
 
dart::dart(QMainWindow *parent) : QMainWindow(parent){
	setupUi(this);
	
	vDrawCircle(0,0,0);
}
 
dart::~dart(){
} 

QLabel *circleLabel;
QLabel *circleLabel1;

class QCircleLabel : public QLabel
{
	public:
		QCircleLabel(QWidget *parent = 0, Qt::WindowFlags f = 0) :
		QLabel(parent, f) {}
		QCircleLabel(const QString &text, QWidget *parent = 0,
		Qt::WindowFlags f = 0) : QLabel(text, parent, f) {}
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

//draws distance circles around P(x|y), using the saved click-coordinates of place n, iteratin #count [recursion]
void dart::vDrawCircle(int x, int y, int n, int count){
// 	if(count*10 < dblGetDistance(x,y,QLscoreHistory[0][n].x(),QLscoreHistory[0][n].y())){ //TODO check all players
	if(count*10 < 55){ //TODO check all players
		this->setAttribute(Qt::WA_PaintOutsidePaintEvent );
		QPainter painter(this);     painter.setPen(Qt::blue);
		painter.drawEllipse(50,50,20,20);
// 		vDrawCircle(x,y,n,count++);
		
		
		circleLabel = new QCircleLabel(this);
		
		
		circleLabel->setGeometry(40, 40, 240, 240);
		
		circleLabel1 = new QCircleLabel(this);
		
		circleLabel1->setGeometry(50, 50, 200, 200);
	}
}
