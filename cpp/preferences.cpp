#include "preferences.hpp"

using namespace std;

preferences::preferences(dart *TDart, io *TIO, QDialog *parent) : myDart(TDart), myIO(TIO), QDialog(parent) {
	setupUi(this);
	
	connect(cbResetCursor,SIGNAL (clicked()), this, SLOT(vSettingChanged()));
	
	//General
	QString s;
	for(int i=0; i<myDart->qlPreferedQcfLanguage.count(); i++) {
		s+=myDart->qlPreferedQcfLanguage[i]+",";
	}
	lePreferedQcfLanguage->setText(s.left(s.length()-1));
	spbNumberOfPlayers->setValue(myDart->iNumberOfPlayers);
	spbMaxTime->setValue(myDart->iMaxTime);
	cbResetCursor->setChecked(myDart->bResetCursor);
	//Advanced
	spbDelayNextCircle->setValue(myDart->iDelayNextCircle);
	spbDelayMark->setValue(myDart->iDelayMark);
	spbDelayNextPlayer->setValue(myDart->iDelayNextPlayer);
	spbDelayNextPlace->setValue(myDart->iDelayNextPlace);
	spbDelayNextPlaceTraining->setValue(myDart->iDelayNextPlaceTraining);
	
}

preferences::~preferences() {
}

void preferences::vSettingChanged() {
}

