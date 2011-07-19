/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include <qstring.h>
#include <qlist.h>

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
		
	private:
		void resizeEvent ( QResizeEvent * event );
		void vDrawCircle(int x, int y, int n, int count=0);
};
#endif //DART_HPP 
