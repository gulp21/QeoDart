#include "preferences.hpp"

using namespace std;

preferences::preferences(dart *TDart, io *TIO, QDialog *parent) : myDart(TDart), myIO(TIO), QDialog(parent) {
	setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowContextHelpButtonHint | Qt::WindowMaximizeButtonHint);
	
	setupUi(this);
	
	//General
	connect(lePreferedQcfLanguage, SIGNAL (textEdited(QString)), this, SLOT(vSettingChanged()));
	connect(spbMaxPlaceCount, SIGNAL (valueChanged(int)), this, SLOT(vSettingChanged()));
	connect(spbMaxTime, SIGNAL (valueChanged(int)), this, SLOT(vSettingChanged()));
	connect(cobScoreAreaMode, SIGNAL (currentIndexChanged(int)), this, SLOT(vSettingChanged()));
	connect(cbResetCursor, SIGNAL (clicked()), this, SLOT(vSettingChanged()));
	//buttons
	connect(buttonBox, SIGNAL (accepted()), this, SLOT(vAccepted()));
	connect(buttonBox->button(QDialogButtonBox::Reset), SIGNAL (clicked()), this, SLOT(vReset()));
	connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL (clicked()), this, SLOT(vRestoreDefaults()));
	
	vReset();
	
	cbResetCursor->setToolTip(cbResetCursor->whatsThis());
	cobScoreAreaMode->setToolTip(cobScoreAreaMode->whatsThis());
	spbLettersPerSecond->setToolTip(spbLettersPerSecond->whatsThis());
//	cbCursor TODO->setToolTip(cbCursor->whatsThis());
	
	lblStatusText->setVisible(false);
	
}

preferences::~preferences() {
}

void preferences::vReset() {
	//General
	lePreferedQcfLanguage->setText(myDart->qsPreferedQcfLanguage);
	spbMaxPlaceCount->setValue(myDart->iMaxPlaceCount);
	spbMaxTime->setValue(myDart->iMaxTime);
	cbResetCursor->setChecked(myDart->bResetCursor);
	cobScoreAreaMode->setCurrentIndex(myDart->iScoreAreaMode);
	//Advanced
	spbDelayNextCircle->setValue(myDart->iDelayNextCircle);
	spbDelayNextPlayer->setValue(myDart->iDelayNextPlayer);
	spbDelayNextPlace->setValue(myDart->iDelayNextPlace);
	spbDelayNextPlaceTraining->setValue(myDart->iDelayNextPlaceTraining);
}

void preferences::vRestoreDefaults() {
	//General
	lePreferedQcfLanguage->setText("ui,default,en");
	spbMaxPlaceCount->setValue(10);
	spbMaxTime->setValue(20);
	cbResetCursor->setChecked(true);
	
	cobScoreAreaMode->setCurrentIndex(1);
	
	//spbLettersPerSecond->setValue(2);
	//Advanced
	spbDelayNextCircle->setValue(200);
	spbDelayNextPlayer->setValue(500);
	spbDelayNextPlace->setValue(2000);
	spbDelayNextPlaceTraining->setValue(1000);
}

void preferences::vSettingChanged() {
	
	if(QObject::sender()==lePreferedQcfLanguage && lePreferedQcfLanguage->text()!=myDart->qsPreferedQcfLanguage) {
		lblStatusText->setVisible(true);
		lblStatusText->setStyleSheet("font-weight:bold; color:gray;");
		lblStatusText->setText(tr("Changing this language setting will require reloading the map."));
	} else if(myDart->bCanLoseScore()) {
		qDebug()<<"fff";
		if(QObject::sender()==spbMaxPlaceCount && spbMaxPlaceCount->value()<myDart->iPlaceCount) {
			lblStatusText->setVisible(true);
			lblStatusText->setStyleSheet("font-weight:bold; color:red;");
			lblStatusText->setText(tr("Setting the number of places smaller than the current place number will start a new game automatically."));
		} else if( (QObject::sender()==spbMaxTime && spbMaxTime->value()!=myDart->iMaxTime && myDart->bAgainstTime) ||
			   (QObject::sender()==cobScoreAreaMode && cobScoreAreaMode->currentIndex()!=myDart->iScoreAreaMode) ||
		           (QObject::sender()==spbLettersPerSecond /*&& spbMaxTime->value()!=myDart->iMaxTime TODO*/)
			 ) {
			lblStatusText->setVisible(true);
			qDebug()<<"ffsf";
			lblStatusText->setStyleSheet("font-weight:bold; color:red;");
			lblStatusText->setText(tr("Chancing this setting will start a new game automatically."));
		} else {
			lblStatusText->setVisible(false);
		}
	
	} else {
		lblStatusText->setVisible(false);
	}
	
}

void preferences::vAccepted() {
	bool newGameRequired=false, repaintRequired=false;
	
	
	// General
	
	myDart->qsPreferedQcfLanguage=lePreferedQcfLanguage->text();
	if(myDart->qsPreferedQcfLanguage[myDart->qsPreferedQcfLanguage.length()-1]!=',') myDart->qsPreferedQcfLanguage+=",";
	if(!myDart->qsPreferedQcfLanguage.contains("ui,")) myDart->qsPreferedQcfLanguage+="ui,";
	if(!myDart->qsPreferedQcfLanguage.contains("default,")) myDart->qsPreferedQcfLanguage+="default,";
	if(!myDart->qsPreferedQcfLanguage.contains("en,")) myDart->qsPreferedQcfLanguage+="en,";
	myIO->settings->setValue("qsPreferedQcfLanguage",myDart->qsPreferedQcfLanguage);
	
	if(myDart->iPlaceCount>spbMaxPlaceCount->value()) newGameRequired=true;
	else if(myDart->iMaxPlaceCount!=spbMaxPlaceCount->value()) repaintRequired=true;
	myIO->settings->setValue("iMaxPlaceCount",spbMaxPlaceCount->value());
	
	if(myDart->iMaxTime!=spbMaxTime->value() && myDart->bAgainstTime) newGameRequired=true;
	myIO->settings->setValue("iMaxTime",spbMaxTime->value());
	
	myIO->settings->setValue("bResetCursor",cbResetCursor->isChecked());
	
	if(myDart->iScoreAreaMode!=cobScoreAreaMode->currentIndex()) newGameRequired=true;
	myIO->settings->setValue("iScoreAreaMode",cobScoreAreaMode->currentIndex());
	
	
	// Advanced
	
	myIO->settings->setValue("iDelayNextCircle",spbDelayNextCircle->value());
	myIO->settings->setValue("iDelayNextPlayer",spbDelayNextPlayer->value());
	myIO->settings->setValue("iDelayNextPlace",spbDelayNextPlace->value());
	myIO->settings->setValue("iDelayNextPlaceTraining",spbDelayNextPlaceTraining->value());
	
	
	myIO->vLoadSettings();
	
	if(newGameRequired) {
		myDart->iPlaceCount=0; // outsmart bCanLoseScore()
		myDart->vNewGame();
	} else if(repaintRequired) {
		myDart->lblCurrentRound->setText(QString(tr("Place %1 of %2")).arg(myDart->iPlaceCount).arg(myDart->iMaxPlaceCount));
	}
}

