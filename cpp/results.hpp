#ifndef DIALOGS_HPP
#define DIALOGS_HPP

#include "ui_resultWindow.h"
#include "dart.hpp"

class resultWindow : public QDialog, public Ui::resultWindow {
	Q_OBJECT
	
	dart *myDart;
	int player;
	
	public:
		resultWindow (dart*, int PLayer, QDialog *parent = 0);
		~resultWindow();
	private slots:
	private:
		
};

#endif // DIALOGS_HPP
