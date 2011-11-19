/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "preferences.hpp"

using namespace std;

preferences::preferences(dart *TDart, io *TIO, QDialog *parent) : myDart(TDart), myIO(TIO), QDialog(parent) {
	setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowContextHelpButtonHint | Qt::WindowMaximizeButtonHint);
	
	setupUi(this);
	
	//General
	connect(cobLanguage, SIGNAL (currentIndexChanged(int)), this, SLOT(vSettingChanged()));
	connect(lePreferedQcfLanguage, SIGNAL (textEdited(QString)), this, SLOT(vSettingChanged()));
	connect(spbMaxPlaceCount, SIGNAL (valueChanged(int)), this, SLOT(vSettingChanged()));
	connect(spbMaxTime, SIGNAL (valueChanged(int)), this, SLOT(vSettingChanged()));
	connect(cobScoreAreaMode, SIGNAL (currentIndexChanged(int)), this, SLOT(vSettingChanged()));
	connect(cobPenalty, SIGNAL (currentIndexChanged(int)), this, SLOT(vSettingChanged()));
	connect(cbResetCursor, SIGNAL (clicked()), this, SLOT(vSettingChanged()));
	connect(cbAutoNewGame, SIGNAL (clicked()), this, SLOT(vSettingChanged()));
	connect(cbAutoShowHighScores, SIGNAL (clicked()), this, SLOT(vSettingChanged()));
	connect(spbLettersPerSecond, SIGNAL (valueChanged(int)), this, SLOT(vSettingChanged()));
	//Advanced
	connect(spbDelayNextCircle, SIGNAL (valueChanged(int)), this, SLOT(vSettingChanged()));
	connect(spbDelayNextPlayer, SIGNAL (valueChanged(int)), this, SLOT(vSettingChanged()));
	connect(spbDelayNextPlace, SIGNAL (valueChanged(int)), this, SLOT(vSettingChanged()));
	connect(spbDelayNextPlaceTraining, SIGNAL (valueChanged(int)), this, SLOT(vSettingChanged()));
	connect(cbUseOurCursor, SIGNAL (clicked()), this, SLOT(vSettingChanged()));
	connect(cbShortenToolbarText, SIGNAL (clicked()), this, SLOT(vSettingChanged()));
	//buttons
	connect(buttonBox, SIGNAL (accepted()), this, SLOT(vAccepted()));
	connect(buttonBox->button(QDialogButtonBox::Reset), SIGNAL (clicked()), this, SLOT(vReset()));
	connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL (clicked()), this, SLOT(vRestoreDefaults()));
	
	vReset();
	
	cbResetCursor->setToolTip(cbResetCursor->whatsThis());
	cobScoreAreaMode->setToolTip(cobScoreAreaMode->whatsThis());
	cobPenalty->setToolTip(cobPenalty->whatsThis());
	spbLettersPerSecond->setToolTip(spbLettersPerSecond->whatsThis());
	cbUseOurCursor->setToolTip(cbUseOurCursor->whatsThis());
	
#ifdef QT_NO_CURSOR
	cbResetCursor->hide();
	cbUseOurCursor->hide();
#endif
	
	lblStatusText->setVisible(false);
	
}

preferences::~preferences() {
}

void preferences::vReset() {
	//General
	if(myDart->qsLanguage=="de") cobLanguage->setCurrentIndex(1);
	else if(myDart->qsLanguage=="en") cobLanguage->setCurrentIndex(2);
	else if(myDart->qsLanguage=="la") cobLanguage->setCurrentIndex(3);
	else cobLanguage->setCurrentIndex(0);
	lePreferedQcfLanguage->setText(myDart->qsPreferedQcfLanguage);
	spbMaxPlaceCount->setValue(myDart->iMaxPlaceCount);
	spbMaxTime->setValue(myDart->iMaxTime);
	cbResetCursor->setChecked(myDart->bResetCursor);
	cbAutoNewGame->setChecked(myDart->bAutoNewGame);
	cbAutoShowHighScores->setChecked(myDart->bAutoShowHighScores);
	cobScoreAreaMode->setCurrentIndex(myDart->iScoreAreaMode);
	cobPenalty->setCurrentIndex(myDart->iPenalty);
	spbLettersPerSecond->setValue(myDart->iLettersPerSecond);
	//Advanced
	spbDelayNextCircle->setValue(myDart->iDelayNextCircle);
	spbDelayNextPlayer->setValue(myDart->iDelayNextPlayer);
	spbDelayNextPlace->setValue(myDart->iDelayNextPlace);
	spbDelayNextPlaceTraining->setValue(myDart->iDelayNextPlaceTraining);
	cbUseOurCursor->setChecked(myDart->bUseOurCursor);
	cbShortenToolbarText->setChecked(myDart->bShortenToolbarText);
}

void preferences::vRestoreDefaults() {
	//General
	cobLanguage->setCurrentIndex(0);
	lePreferedQcfLanguage->setText("ui,default,en");
	spbMaxPlaceCount->setValue(10);
	spbMaxTime->setValue(20);
	cbResetCursor->setChecked(true);
	cbAutoNewGame->setChecked(false);
	cbAutoShowHighScores->setChecked(true);
	
	cobScoreAreaMode->setCurrentIndex(1);
	cobPenalty->setCurrentIndex(1);
	
	spbLettersPerSecond->setValue(8);
	//Advanced
	spbDelayNextCircle->setValue(200);
	spbDelayNextPlayer->setValue(500);
	spbDelayNextPlace->setValue(2000);
	spbDelayNextPlaceTraining->setValue(1000);
	cbUseOurCursor->setChecked(false);
	cbShortenToolbarText->setChecked(true);
}

void preferences::vSettingChanged() {
	
	if(QObject::sender()==lePreferedQcfLanguage && lePreferedQcfLanguage->text()!=myDart->qsPreferedQcfLanguage) {
		lblStatusText->setVisible(true);
		lblStatusText->setStyleSheet("font-weight:bold; color:gray;");
		lblStatusText->setText(tr("Changing this language setting will require reloading the map."));
	} else if(myDart->bCanLoseScore()) {
		if(QObject::sender()==spbMaxPlaceCount && spbMaxPlaceCount->value()<myDart->iPlaceCount) {
			lblStatusText->setVisible(true);
			lblStatusText->setStyleSheet("font-weight:bold; color:red;");
			lblStatusText->setText(tr("Setting the number of places smaller than the current place number will start a new game automatically."));
		} else if( (QObject::sender()==spbMaxTime && spbMaxTime->value()!=myDart->iMaxTime && myDart->bAgainstTime) ||
			   (QObject::sender()==cobScoreAreaMode && cobScoreAreaMode->currentIndex()!=myDart->iScoreAreaMode) ||
			   (QObject::sender()==cobPenalty && cobPenalty->currentIndex()!=myDart->iPenalty) ||
		           (QObject::sender()==spbLettersPerSecond && spbLettersPerSecond->value()!=myDart->iLettersPerSecond)
			 ) {
			lblStatusText->setVisible(true);
			qDebug()<<"ffsf";
			lblStatusText->setStyleSheet("font-weight:bold; color:red;");
			lblStatusText->setText(tr("Chancing this setting will start a new game automatically."));
		} else {
			lblStatusText->setVisible(false);
		}
		
	} else if(QObject::sender()==cobLanguage) {
		
		lblStatusText->setVisible(true);
		lblStatusText->setStyleSheet("font-weight:bold; color:gray;");
		lblStatusText->setText(tr("A new user interface language will be completely applied after restarting QeoDart."));
	
	} else {
		lblStatusText->setVisible(false);
	}
	
}

void preferences::vAccepted() {
	bool newGameRequired=false, repaintRequired=false;
	
	
	// General
	
	QString lang="default";
	if(cobLanguage->currentText()=="Deutsch") lang="de";
	else if(cobLanguage->currentText()=="English") lang="en";
	else if(cobLanguage->currentText()=="Latina") lang="la";
	myIO->settings->setValue("qsLanguage",lang);
	
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
	myIO->settings->setValue("bAutoNewGame",cbAutoNewGame->isChecked());
	myIO->settings->setValue("bAutoShowHighScores",cbAutoShowHighScores->isChecked());
	
	if(myDart->iScoreAreaMode!=cobScoreAreaMode->currentIndex()) newGameRequired=true;
	myIO->settings->setValue("iScoreAreaMode",cobScoreAreaMode->currentIndex());
	if(myDart->iPenalty!=cobPenalty->currentIndex()) newGameRequired=true;
	myIO->settings->setValue("iPenalty",cobPenalty->currentIndex());
	
	if(myDart->iLettersPerSecond!=spbLettersPerSecond->value()) newGameRequired=true;
	myIO->settings->setValue("iLettersPerSecond",spbLettersPerSecond->value());
	
	// Advanced
	
	myIO->settings->setValue("iDelayNextCircle",spbDelayNextCircle->value());
	myIO->settings->setValue("iDelayNextPlayer",spbDelayNextPlayer->value());
	myIO->settings->setValue("iDelayNextPlace",spbDelayNextPlace->value());
	myIO->settings->setValue("iDelayNextPlaceTraining",spbDelayNextPlaceTraining->value());
	
	myIO->settings->setValue("bUseOurCursor",cbUseOurCursor->isChecked());
	myIO->settings->setValue("bShortenToolbarText",cbShortenToolbarText->isChecked());
	
	
	myIO->vLoadSettings();
	
	if(newGameRequired) {
		myDart->iPlaceCount=0; // outsmart bCanLoseScore()
		myDart->vNewGame();
	} else if(repaintRequired) {
		myDart->lblCurrentRound->setText(QString(tr("Place %1 of %2")).arg(myDart->iPlaceCount).arg(myDart->iMaxPlaceCount));
	}
}

