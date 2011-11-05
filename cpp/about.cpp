#include "about.hpp"

aboutWindow::aboutWindow(dart *TDart, QWidget *parent) : myDart(TDart), QDialog(parent) {
	setupUi(this);
	
	lblVersion->setText(tr("Version %1").arg("2.0a"));
	
	QString date=__DATE__;
	// WORKAROUND Qt Bug? Does only accept month names in the current locale
	if((date=QDate::fromString(date,"MMM dd yyyy").toString())=="") date=__DATE__;
	
	lblCompiled->setText(tr("compiled on %1").arg(date));
	
	QString s;
	qcfFile *f; f=&(myDart->qlQcfxFiles[myDart->iCurrentQcf]);
	if(f->copyright.file!="") s+=tr("<b>The file (%1.qcfx) itself:</b><br/>%2<br/><br/>").arg(f->path).arg(f->copyright.file);
	if(f->copyright.background!="") s+=tr("<b>The background image:</b><br/>%2<br/><br/>").arg(f->copyright.borders);
	if(f->copyright.borders!="") s+=tr("<b>The image with the borders:</b><br/>%2<br/><br/>").arg(f->copyright.borders);
	if(f->copyright.rivers!="") s+=tr("<b>The image with the rivers:</b><br/>%2<br/><br/>").arg(f->copyright.rivers);
	if(f->copyright.elevations!="") s+=tr("<b>The image with the elevations:</b><br/>%2<br/><br/>").arg(f->copyright.elevations);
	if(s!="")
		s.prepend(tr("<p>The following licences apply to your current map (%1):</p>").arg(f->mapName));
	else
		s=tr("No licensing information is available for the map \"%1\" (%2).").arg(f->mapName).arg(f->path);
	s=s.replace("\\n","<br/>");
	lblMapLicense->setText(s);
}

aboutWindow::~aboutWindow() {
}
