#include "dialogs.hpp"
#include "qtwin.h"

using namespace std;

resultWindow::resultWindow(dart *TDart, int PLayer, QDialog *parent) : myDart(TDart), player(PLayer), QDialog(parent) {
	const int R1=15, G1=135, B1=28, R2=255, G2=25, B2=25;

	setParent(myDart);
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);

	setupUi(this);
	
	connect(btOk, SIGNAL(clicked()), this, SLOT(close())); // TODO we should save the name
	
	if(QtWin::enableBlurBehindWindow(this, true)) { // if we can have Aero glass…
		setWindowOpacity(1.0);
		QtWin::extendFrameIntoClientArea(this); // TODO doesn't work // …we should use it
	}
	
	if(myDart->iNumberOfPlayers==1) {
		lblScore->setText(QString(tr("<span>You have a score of %1 points (&#8960; %2).</span>")).arg(myDart->qlTotalScores[0].score).arg(myDart->qlTotalScores[0].score/myDart->iMaxPlaceCount,0,'f',1));
	} else {
		lblScore->setText(QString(tr("<span>Player %1 has a score of %2 points (&#8960; %3).</span>")).arg(player+1).arg(myDart->qlTotalScores[player].score).arg(myDart->qlTotalScores[player].score/myDart->iMaxPlaceCount,0,'f',1));
	}
	
	lblMark->setText(QString(tr("That equals grade %1.")).arg(myDart->qlTotalScores[player].mark,0,'f',1));
	
	
	
	QString places;
	for(int i=0,max=myDart->qlScoreHistory[player].count(); i<max; i++) {
		// find the suitable color for the mark:
		// f(mark)=colorStart+(colorEnd-colorStart)*1/6*mark
		double x=(myDart->qlScoreHistory[player][i].mark-1)*1/6;
		int r=R1+(R2-R1)*x;
		int g=G1-(G1-G2)*x;
		int b=B1-(B1-B2)*x;
		
		places+=QString(tr("<span style=\"color:rgb(%4,%5,%6)\">%1&nbsp;(%2,&nbsp;%3)</span>, ")).arg(myDart->qlCurrentTypePlaces[myDart->qlPlacesHistory[i]]->name).arg(myDart->qlScoreHistory[player][i].score).arg(myDart->qlScoreHistory[player][i].mark,0,'f',1).arg(r).arg(g).arg(b);
		
	}
	
	lblPlaces->setText(places.left(places.length()-2)); // remove the last ", "
	

	#ifdef Q_OS_LINUX
	//WORKAROUND for https://bugreports.qt.nokia.com/browse/QTBUG-691
	//we resize the window so that the text fits (better)
	if(places.size()*7/width()*7>65) resize(width(),height()-65+places.size()*7/width()*7);
	#endif
	
}

resultWindow::~resultWindow(){
}