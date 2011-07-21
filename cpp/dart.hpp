/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include <qstring.h>
#include <qlist.h>
#include "myLabels.hpp"

#ifndef DART_HPP
#define DART_HPP

enum enPlaceType {state=10, capital=11};

struct place {
   int x, y, dimx, dimy;
   QString name;
};

#include "ui_mainWindow.h"
 
class dart : public QMainWindow, public Ui::MainWindow{
	Q_OBJECT

	public:
		dart (QMainWindow *parent = 0);
		~dart();
		
		QList<place> qlAllPlaces;	//contains all places of the current map
		QList<place*> qlCurrentPlaces;	//contains pointers pointing on the places in qlAllPlaces which fit the current placetype
		
		double dZoomFactor;
		
	private:
		void resizeEvent ( QResizeEvent * event );
		void vDrawCircle(int x, int y, int n, int count=0);
		void vDrawPoint(int x, int y);
		
		int iPaddingTop;
		
		QLabel *circleLabel;
		QLabel *circleLabel1;
		QLabel *MouseLabel1;
		QLabel *qlCurrentPlace;
};

const int RADIUS=10, PENWIDTH=5;

// class dart;

class QCircleLabel : public QLabel{
	
	dart *myDart;
	
	public:
		QCircleLabel( dart *TDart, QWidget *parent = 0, Qt::WindowFlags f = 0) : myDart(TDart), QLabel(parent, f) {}
		QCircleLabel(const QString &text, QWidget *parent = 0,Qt::WindowFlags f = 0) : QLabel(text, parent, f) {}
		~QCircleLabel() {}
		
		void paintEvent(QPaintEvent *event) {
			QPainter p(this);
			QPen pen;
			pen.setWidth(PENWIDTH);
			pen.setColor(QColor(0,0,255, 255-width()));
			p.setRenderHint(QPainter::Antialiasing);
			p.setPen(pen);
			p.drawEllipse(PENWIDTH*myDart->dZoomFactor, PENWIDTH*myDart->dZoomFactor, width()-2*PENWIDTH, height()-2*PENWIDTH);
			p.end();
			QLabel::paintEvent(event);
		}
};

class QPointLabel : public QLabel{
	
	dart *myDart;
	
	public:
		QPointLabel( dart *TDart, QWidget *parent = 0, Qt::WindowFlags f = 0) : myDart(TDart), QLabel(parent, f) {}
// 		io::io(dart *TDart) : myDart(TDart){
		QPointLabel(const QString &text, QWidget *parent = 0,Qt::WindowFlags f = 0) : QLabel(text, parent, f) {}
		~QPointLabel() {}
		
		void paintEvent(QPaintEvent *event) {
// 			QPainter p(this);
// 			p.setBackgroundMode(Qt::OpaqueMode);
// 			QBrush brush;
// 			brush.setColor(QColor(100,0,255,200));
// // 			p.setRenderHint(QPainter::Antialiasing);
// 			p.setBackground(brush);
// // 			p.background.color();
// 			p.drawEllipse(QPointF(0, 0), width(), height());
// 			p.end();
// 			QLabel::paintEvent(event);
//TODO Text
			QPainter p(this);
			QPen pen;
			pen.setWidth(PENWIDTH);
			pen.setColor(QColor(0,0,255, 255));
			p.setRenderHint(QPainter::Antialiasing);
			p.setPen(pen);
			p.drawEllipse(PENWIDTH*myDart->dZoomFactor, PENWIDTH*myDart->dZoomFactor, PENWIDTH*myDart->dZoomFactor, PENWIDTH*myDart->dZoomFactor);
			p.end();
			QLabel::paintEvent(event);
		}
};

class QMouseReleaseLabel : public QLabel{
	public:
		QMouseReleaseLabel(QWidget *parent = 0, Qt::WindowFlags f = 0) :
		QLabel(parent, f) {}
		~QMouseReleaseLabel() {}
		
		void mouseReleaseEvent(QMouseEvent * event) {
			cout << event->x() << " - " << event->y() << endl;
		}
}; 


#endif //DART_HPP 
