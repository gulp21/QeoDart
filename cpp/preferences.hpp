#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP

#include "ui_preferences.h"
#include "dart.hpp"

class preferences : public QDialog, public Ui::preferences {
	Q_OBJECT
	
	dart *myDart;
	io *myIO;
	
	public:
		preferences (dart*, io*, QDialog *parent = 0);
		~preferences();
	private slots:
	private:
		
};

#endif // PREFERENCES_HPP
