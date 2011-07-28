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

class QCircleLabel : public QLabel{

	private:
		dart *myDart;
		int x, y, r;
		QColor color;
	
	public:
		QCircleLabel( dart *TDart, int X, int Y, int R, QColor COlor, QWidget *parent = 0, Qt::WindowFlags f = 0) : myDart(TDart), x(X), y(Y), r(R), color(COlor), QLabel(parent, f) {
			resize(1,1); // otherwise the circles would be visible at the wrong position, as paintEvent hasn't been triggered yet
			setParent(myDart->centralwidget);
			setVisible(TRUE);
		}
		QCircleLabel(const QString &text, QWidget *parent = 0,Qt::WindowFlags f = 0) : QLabel(text, parent, f) {}
		~QCircleLabel() {}

		void paintEvent(QPaintEvent *event) {
			QPainter p(this);
			QPen pen;
			
			double penWidth=PENWIDTH*myDart->dZoomFactor<2 ? 2 : PENWIDTH*myDart->dZoomFactor; // <2 looks bad
			double circleWidth=r*2*myDart->dZoomFactor;
			
			pen.setWidth(penWidth);
			QColor c=color;
			c.setAlpha(255-r*3);
			pen.setColor(c);
			p.setRenderHint(QPainter::Antialiasing);
			p.setPen(pen);
			p.drawEllipse(QRectF(penWidth/2, penWidth/2, circleWidth, circleWidth)); //x,y,w,h
// 			p.drawEllipse(QPointF(x*myDart->dZoomFactor,y*myDart->dZoomFactor+myDart->iPaddingTop),r,r);
			p.end();
			
			resize(
				circleWidth+penWidth,
				circleWidth+penWidth
			);
			move(
				x*myDart->dZoomFactor-circleWidth/2-penWidth/2,
				y*myDart->dZoomFactor-circleWidth/2-penWidth/2+myDart->iPaddingTop
			);
			
			QLabel::paintEvent(event);
		}
};

class QPointLabel : public QLabel{

	dart *myDart;

	private:
		QString name;
		int x, y;
		QColor color;

	public:
		QPointLabel( dart *TDart, QString Name, int X, int Y, QColor COlor, QWidget *parent = 0, Qt::WindowFlags f = 0) : myDart(TDart), name(Name), x(X), y(Y), color(COlor), QLabel(parent, f) {
			resize(1,1);
			setParent(myDart->centralwidget);
			setVisible(TRUE);
		}
		QPointLabel(const QString &text, QWidget *parent = 0,Qt::WindowFlags f = 0) : QLabel(text, parent, f) {}
		~QPointLabel() {}
		
		void paintEvent(QPaintEvent *event) {
			QPainter p(this);
			QPen pen;
			
			double penWidth=PENWIDTH*myDart->dZoomFactor<2 ? 2 : PENWIDTH*myDart->dZoomFactor;
			
			pen.setWidth(penWidth); //TODO WORKAROUND We should setBackground
			pen.setColor(color);
			p.setRenderHint(QPainter::Antialiasing);
			p.setPen(pen);
// 			p.drawEllipse(penWidth/2,penWidth/2,penWidth,penWidth); //x,y,w,h
			p.drawEllipse(QRectF(penWidth/2.0,penWidth/2.0,penWidth,penWidth));
			p.end();
			
			QString spaces="&nbsp;&nbsp;&nbsp;&nbsp;"; //TODO WORKAROUND don't know another way for setting padding-left
			for(int i=4;i<penWidth;i++){
				spaces+="&nbsp;";
			}
			setText(QString("<span>%3%2</span>").arg(name).arg(spaces));
			int fontSize=myDart->dZoomFactor*PENWIDTH+3<5 ? 5 : myDart->dZoomFactor*PENWIDTH+3; // fontSize<5 is illegiable
			setFont(QFont("Arial", fontSize));
// 			qDebug()<<x<<y<<a<<spaces<<fontSize<<"ll";
			
// 			setGeometry(x*myDart->dZoomFactor,y*myDart->dZoomFactor+myDart->iPaddingTop,100*myDart->dZoomFactor,a*3);
			
// 			resize(100*myDart->dZoomFactor,a*3);
// 			move(x*myDart->dZoomFactor-a,y*myDart->dZoomFactor+myDart->iPaddingTop-a-a);
			
			resize(
				100*myDart->dZoomFactor, // text must be visible
				penWidth+penWidth
			);
			move(
				x*myDart->dZoomFactor-penWidth/2-penWidth/2,
				y*myDart->dZoomFactor-penWidth/2-penWidth/2+myDart->iPaddingTop
			);
			
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
			else if (event->button()==Qt::RightButton && event->y()<0 && event->x()<10)
				myDart->vShowCurrentPlace();
		}
};


#endif //MYLABELS_HPP 
