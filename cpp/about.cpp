/*
QeoDart Copyright (C) 2012 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "about.hpp"

aboutWindow::aboutWindow(dart *TDart, QWidget *parent) : myDart(TDart), QDialog(parent) {
	
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMinimizeButtonHint);
	
	setupUi(this);
	
	lblVersion->setText(tr("Version %1").arg("2.0a"));
	
	QString date=__DATE__;
	// WORKAROUND Qt Bug? Does only accept month names in the current locale
	if((date=QDate::fromString(date,"MMM dd yyyy").toString())=="") date=__DATE__;
	
	lblCompiled->setText(tr("Date of compilation: %1").arg(date));
	
	QString s;
	qcfFile *f; f=&(myDart->qlQcfxFiles[myDart->iCurrentQcf]);
	if(f->copyright.file!="") s+=tr("<b>The file (%1.qcfx) itself:</b><br/>%2<br/><br/>").arg(f->path).arg(f->copyright.file);
	if(f->copyright.background!="") s+=tr("<b>The background image:</b><br/>%1<br/><br/>").arg(f->copyright.borders);
	if(f->copyright.borders!="") s+=tr("<b>The image with the borders:</b><br/>%1<br/><br/>").arg(f->copyright.borders);
	if(f->copyright.rivers!="") s+=tr("<b>The image with the rivers:</b><br/>%1<br/><br/>").arg(f->copyright.rivers);
	if(f->copyright.elevations!="") s+=tr("<b>The image with the elevations:</b><br/>%1<br/><br/>").arg(f->copyright.elevations);
	if(s!="") {
		s.prepend(tr("<p>The following licences apply to your current map (%1):</p>").arg(f->mapName));
		s.remove(QRegExp("<br/><br/>$")); // remove unnecessary <br/><br/> at the end of the string
	} else {
		s=tr("No licensing information is available for the map \"%1\" (%2).").arg(f->mapName).arg(f->path);
	}
	s=s.replace("\\n","<br/>");
	lblMapLicense->setText(s);
}

aboutWindow::~aboutWindow() {
}
