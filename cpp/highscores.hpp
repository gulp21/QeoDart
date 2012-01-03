/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#ifndef HIGHSCORES_HPP
#define HIGHSCORES_HPP

#include <QDialog>
#include "dart.hpp"
#include "ui_highscore.h"

class highScoreWindow : public QDialog, public Ui::highScoreWindow {
	Q_OBJECT
	
	dart *myDart;
	io *myIO;
	
	public:
		explicit highScoreWindow(dart*, io*, QWidget *parent = 0);
		~highScoreWindow();
	private slots:
		void vLoadHighScores(int index);
		void vClear();
};

#endif // HIGHSCORES_HPP
