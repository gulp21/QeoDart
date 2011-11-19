/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "highscores.hpp"

highScoreWindow::highScoreWindow(dart *TDart, io *TIO, QWidget *parent) : myDart(TDart), myIO(TIO), QDialog(parent) {
	setupUi(this);
	
	connect(comboBox, SIGNAL (currentIndexChanged(int)), this, SLOT(vLoadHighScores(int)));
	connect(btClose, SIGNAL (clicked()), this, SLOT(close()));
	
	for(int i=0; i<myDart->qlQcfxFiles.count(); i++) {
		comboBox->addItem(myDart->qlQcfxFiles[i].mapName);
		if(i==myDart->iCurrentQcf) comboBox->setCurrentIndex(i);
	}
	
	myIO->vLoadHighScores(comboBox->itemText(comboBox->currentIndex()));
}

highScoreWindow::~highScoreWindow() {
}

void highScoreWindow::vLoadHighScores(int index) {
	myIO->vLoadHighScores(comboBox->itemText(index));
	
	for(int i=0; i<10; i++) {
		static_cast<QLabel*>(gridLayout->itemAtPosition(i+1,1)->widget())->setText(myDart->qlHighScores[i].name);
		static_cast<QLabel*>(gridLayout->itemAtPosition(i+1,2)->widget())->setText(QString("%1").arg(myDart->qlHighScores[i].score,0,'f',1));
	}
}
