#ifndef ABOUT_HPP
#define ABOUT_HPP

#include <QDialog>
#include "dart.hpp"
#include "ui_about.h"

class aboutWindow : public QDialog, public Ui::aboutWindow {
	Q_OBJECT
	
	dart *myDart;
	
	public:
		explicit aboutWindow(dart*, QWidget *parent = 0);
		~aboutWindow();
};

#endif // ABOUT_HPP
