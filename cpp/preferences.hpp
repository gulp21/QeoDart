/*
QeoDart Copyright (C) 2012 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

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
		void vSettingChanged();
		void vReset();
		void vRestoreDefaults();
		void vAccepted();
	private:
		
};

#endif // PREFERENCES_HPP
