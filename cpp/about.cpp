#include "about.hpp"

aboutWindow::aboutWindow(dart *TDart, QWidget *parent) : myDart(TDart), QDialog(parent) {
	setupUi(this);
	
	// TODO load map licence
}

aboutWindow::~aboutWindow() {
}
