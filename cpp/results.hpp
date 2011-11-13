#ifndef DIALOGS_HPP
#define DIALOGS_HPP

#include "ui_resultWindow.h"
#include "dart.hpp"
#ifdef Q_OS_WINCE
#include <QScrollArea>
#endif

class resultWindow : public QDialog, public Ui::resultWindow {
	Q_OBJECT
	
	bool *bShowHighScores;
	dart *myDart;
	int player;
	io *myIO;
	
	public:
		resultWindow (bool &ShowHighScores, dart*, int PLayer, io*, QDialog *parent = 0);
		~resultWindow();
	private slots:
		void vClose();
	private:
		
};

#endif // DIALOGS_HPP
