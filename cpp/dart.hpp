/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include <qstring.h>
#include <qlist.h>
#include <QLabel>

#ifndef DART_HPP
#define DART_HPP

struct place {
   int x, y, dimx, dimy;
   QString name, placeType;
};

#include "ui_mainWindow.h"
 
class dart : public QMainWindow, public Ui::MainWindow {
Q_OBJECT

	public:
		dart (QMainWindow *parent = 0);
		~dart();

		QList<place> qlAllPlaces; //contains all places of the current map
		QList<place*> qlCurrentTypePlaces; //contains pointers pointing on the places in qlAllPlaces which fit the current placetype

		double dZoomFactor;
		int iPaddingTop;
		QString qsCurrentPlaceType;
		
		void vMouseClickEvent(int x, int y);

	private:
		QList<QLabel*> qlPointLabels;
		QList<QLabel*> qlCircleLabels;
		
		void resizeEvent ( QResizeEvent * event );
		void vDrawCircle(int x, int y, int n, int count=0);
		void vDrawDistanceCircles();
		void vDrawPoint(int x, int y, QString name="");
		
		int iGetWindowSize();
		int iGetUnzoomed(double x);

		QLabel *qlMouseClickOverlay;
		QLabel *qlMapBackground;
	
	private slots:
		void vClose();
		void vShowAllPlaces();
};

#include "myLabels.hpp"

#endif //DART_HPP 