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
};

#endif // HIGHSCORES_HPP
