/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

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

class QCircleLabel : public QLabel {

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
			if(x==-1 && y==-1) return;
			
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

class QPointLabel : public QLabel {

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
			if(x==-1 && y==-1) return;
			
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
			
			setStyleSheet(QString("color:rgba(0,0,0,%1)").arg(color.alpha()));
			//QPainter::setRenderHint(QPainter::renderHints() | QPainter::TextAntialiasing);
			
			resize(
				(20+name.length()*7)*myDart->dZoomFactor, // text must be visible
				3*penWidth
			);
			move(
				x*myDart->dZoomFactor-penWidth/2-penWidth/2,
				y*myDart->dZoomFactor-penWidth/2-penWidth/2+myDart->iPaddingTop
			);
			
			QLabel::paintEvent(event);
		}
};

class QMouseReleaseLabel : public QLabel {
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

class QRectangleLabel : public QLabel {

	private:
		dart *myDart;
		int x, y, a, b, opacity;
	
	public:
		// center P(M1,M2), half width Dimx, half height Dimy; calculates upper left corner
		QRectangleLabel(dart *TDart, int M1, int M2, int Dimx, int Dimy, QWidget *parent = 0, Qt::WindowFlags f = 0) : myDart(TDart), x(M1), y(M2), a(Dimx), b(Dimy), QLabel(parent, f) {
			resize(1,1);
			setParent(myDart->centralwidget);
			x=x-a/2;
			y=y-b/2;
			opacity=150;
			setVisible(true);
		}
		// center P(M1,M2); calculates quadrant, used for hint
		QRectangleLabel(dart *TDart, int X, int Y, QWidget *parent = 0, Qt::WindowFlags f = 0) : myDart(TDart), x(X), y(Y), QLabel(parent, f) {
			resize(1,1);
			setParent(myDart->centralwidget);
			x>=300 ? x=300 : x=0;
			y>=300 ? y=300 : y=0;
			a=300;
			b=300;
			opacity=255;
			setVisible(true);
		}
		QRectangleLabel(const QString &text, QWidget *parent = 0,Qt::WindowFlags f = 0) : QLabel(text, parent, f) {}
		~QRectangleLabel() {}

		void paintEvent(QPaintEvent *event) {
			QPainter p(this);
			QPen pen;
			
			pen.setWidth(10);
			QColor c=QColor(255,182,19,opacity);
			pen.setColor(c);
			p.setRenderHint(QPainter::Antialiasing);
			p.setPen(pen);
//			p.drawRect(x*myDart->dZoomFactor, y*myDart->dZoomFactor, a*myDart->dZoomFactor, b*myDart->dZoomFactor); //x,y,w,h
			
			p.drawRect(0, 0, a*myDart->dZoomFactor, b*myDart->dZoomFactor); 
			p.end();
			
			resize(
				a*myDart->dZoomFactor,
				b*myDart->dZoomFactor
			);
			move(
				x*myDart->dZoomFactor,
				y*myDart->dZoomFactor+myDart->iPaddingTop
			);
			
			QLabel::paintEvent(event);
		}
};


#endif //MYLABELS_HPP 
