/*
QeoDart Copyright (C) 2010 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#ifndef DART_HPP
#define DART_HPP
 
#include "ui_mainWindow.h"
 
class dart : public QMainWindow, public Ui::MainWindow{
	Q_OBJECT

	public:
		dart (QMainWindow *parent = 0);
		~dart();
		
		void vDrawCircle(int x, int y, int n, int count=0);
};
#endif //DART_HPP 
