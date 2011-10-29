#include "about.hpp"

aboutWindow::aboutWindow(dart *TDart, QWidget *parent) : myDart(TDart), QDialog(parent) {
	setupUi(this);
	
	lblVersion->setText(tr("Version %1").arg("2.0a"));
	
	QString date=__DATE__;
	// WORKAROUND Qt Bug? Does only accept month names in the current locale
	if((date=QDate::fromString(date,"MMM dd yyyy").toString())=="") date=__DATE__;
	
	lblCompiled->setText(tr("compiled on %1").arg(date));
	
	// TODO load map licence
}

aboutWindow::~aboutWindow() {
}
