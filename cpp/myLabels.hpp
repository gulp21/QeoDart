#include <QLabel>
#include <qpainter.h>
#include <iostream>
#include <QMouseEvent>
#include <QPointF>
#include <QDebug>

#ifndef MYLABELS_HPP
#define MYLABELS_HPP

using namespace std;

const int RADIUS=10, PENWIDTH=5;

// class dart;

class QCircleLabel : public QLabel{

	dart *myDart;
	
	private:
		int x, y, r;
	
	public:
		QCircleLabel( dart *TDart, int X, int Y, int R, QWidget *parent = 0, Qt::WindowFlags f = 0) : myDart(TDart), x(X), y(Y), r(R), QLabel(parent, f) {setVisible(TRUE); repaint();}
		QCircleLabel(const QString &text, QWidget *parent = 0,Qt::WindowFlags f = 0) : QLabel(text, parent, f) {}
		~QCircleLabel() {}

		void paintEvent(QPaintEvent *event) {
			QPainter p(this);
			QPen pen;
			
			int a=PENWIDTH*myDart->dZoomFactor<2?2:PENWIDTH*myDart->dZoomFactor; // <2 looks bad
			pen.setWidth(a);
			pen.setColor(QColor(0,0,255, 255-r));
			p.setRenderHint(QPainter::Antialiasing);
			p.setPen(pen);
			p.drawEllipse(a, a, r*2*myDart->dZoomFactor, r*2*myDart->dZoomFactor); //x,y,w,h
// 			p.drawEllipse(QPointF(x*myDart->dZoomFactor,y*myDart->dZoomFactor+myDart->iPaddingTop),r,r);
			p.end();
			
			resize(2*r*myDart->dZoomFactor+2*PENWIDTH,2*r*myDart->dZoomFactor+2*PENWIDTH);
			move(x*myDart->dZoomFactor-a-r*myDart->dZoomFactor,y*myDart->dZoomFactor+myDart->iPaddingTop-a-r*myDart->dZoomFactor);
			
			QLabel::paintEvent(event);
		}
};

class QPointLabel : public QLabel{

	dart *myDart;

	private:
		QString name;
		int x, y;

	public:
		QPointLabel( dart *TDart, QString Name, int X, int Y, QWidget *parent = 0, Qt::WindowFlags f = 0) : myDart(TDart), name(Name), x(X), y(Y),  QLabel(parent, f) { setVisible(TRUE); repaint();}
		QPointLabel(const QString &text, QWidget *parent = 0,Qt::WindowFlags f = 0) : QLabel(text, parent, f) {}
		~QPointLabel() {}
		
		void paintEvent(QPaintEvent *event) {
			QPainter p(this);
			QPen pen;
			pen.setWidth(PENWIDTH); //TODO WORKAROUND We should setBackground
			pen.setColor(QColor(0,0,255,255));
			p.setRenderHint(QPainter::Antialiasing);
			p.setPen(pen);
			int a=PENWIDTH*myDart->dZoomFactor<2?2:PENWIDTH*myDart->dZoomFactor; // <2 looks bad
			p.drawEllipse(a,a,a,a); //x,y,w,h
			p.end();
			
			QString spaces="&nbsp;&nbsp;&nbsp;&nbsp;"; //TODO WORKAROUND don't know another way for setting padding-left
			for(int i=4;i<a;i++){
				spaces+="&nbsp;";
			}
			setText(QString("<span>%3%2</span>").arg(name).arg(spaces));
			int fontSize=myDart->dZoomFactor*PENWIDTH+3<5?5:myDart->dZoomFactor*PENWIDTH+3; // fontSize<5 is illegiable
			setFont(QFont("Arial", fontSize));
// 			qDebug()<<x<<y<<a<<spaces<<fontSize<<"ll";
			
// 			setGeometry(x*myDart->dZoomFactor,y*myDart->dZoomFactor+myDart->iPaddingTop,100*myDart->dZoomFactor,a*3);
			
			resize(100*myDart->dZoomFactor,a*3);
			move(x*myDart->dZoomFactor-a,y*myDart->dZoomFactor+myDart->iPaddingTop-a-a);
			
			QLabel::paintEvent(event);
		}
};

class QMouseReleaseLabel : public QLabel{
	dart *myDart;

	public:
		QMouseReleaseLabel(dart *TDart, QWidget *parent = 0, Qt::WindowFlags f = 0) : myDart(TDart), QLabel(parent, f) {}
		~QMouseReleaseLabel() {}

		void mouseReleaseEvent(QMouseEvent * event) {
			qDebug() << "[i] mouseReleaseEvent" << event->x() << "|" << event->y();
			//we do not accept release events which are outside of the Label or triggered by a mouse button other than LeftButton
			if(! (event->button()!=Qt::LeftButton || event->x()<0 || event->y()<0 || event->x()>width() || event->y()>height()))
				myDart->vMouseClickEvent(event->x(), event->y());
		}
};


#endif //MYLABELS_HPP 