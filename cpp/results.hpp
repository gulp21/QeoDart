/*
QeoDart Copyright (C) 2012 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#ifndef DIALOGS_HPP
#define DIALOGS_HPP

#include "ui_resultWindow.h"
#include "dart.hpp"
#ifdef Q_OS_WINCE
#include <QScrollArea>
#endif

class resultWindow : public QDialog, public Ui::resultWindow {
	Q_OBJECT
	
	int *iHighlightHighScore;
	dart *myDart;
	int player;
	io *myIO;
	
	public:
		resultWindow (int &HighlightHighScore, dart*, int PLayer, io*, QDialog *parent = 0);
		~resultWindow();
	private slots:
		void vClose();
	private:
		
};

#endif // DIALOGS_HPP
