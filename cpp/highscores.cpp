/*
QeoDart Copyright (C) 2012 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "highscores.hpp"

highScoreWindow::highScoreWindow(int highlightHighScore, dart *TDart, io *TIO, QWidget *parent) : iHighlightHighScore(highlightHighScore), myDart(TDart), myIO(TIO), QDialog(parent) {
	setupUi(this);
	
	connect(btClear, SIGNAL (clicked()), this, SLOT(vClear()));
	btClear->setIcon(QIcon::fromTheme("edit-clear", QIcon(":/icons/oxygen/edit-clear.png")));
	connect(btClose, SIGNAL (clicked()), this, SLOT(close()));
	btClose->setIcon(QIcon::fromTheme("window-close", QIcon(":/icons/oxygen/window-close.png")));
	
	for(int i=0; i<myDart->qlQcfxFiles.count(); i++) {
		comboBox->addItem(myDart->qlQcfxFiles[i].mapName);
	}
	
	comboBox->setCurrentIndex(-1); // TODO WORKAROUND just make sure that current index will be changed, so that the signal is emitted (we should use mapId or name for highlighting new highscores)
	//addItem should not trigger currentIndexChanged
	connect(comboBox, SIGNAL (currentIndexChanged(int)), this, SLOT(vLoadHighScores(int)));
	
	comboBox->setCurrentIndex(myDart->iCurrentQcf);
	
	myIO->vLoadHighScores(comboBox->itemText(comboBox->currentIndex()));
}

highScoreWindow::~highScoreWindow() {
}

/*!
  Reloads the highscores of the selected map using myIO::vLoadHighScores and displays them.
  @param index the index of the map in comboBox and qlQcfxFiles
  @see io::vLoadHighScores(QString mapName)
  */
void highScoreWindow::vLoadHighScores(int index) {
	myIO->vLoadHighScores(comboBox->itemText(index));
	
	for(int i=0; i<10; i++) {
		QString style=(iHighlightHighScore==i ? "font-weight:bold" : "font-weight:normal");
		static_cast<QLabel*>(gridLayout->itemAtPosition(i+1,1)->widget())->setText(myDart->qlHighScores[i].name);
		static_cast<QLabel*>(gridLayout->itemAtPosition(i+1,1)->widget())->setStyleSheet(style);
		static_cast<QLabel*>(gridLayout->itemAtPosition(i+1,2)->widget())->setText(QString("%1").arg(myDart->qlHighScores[i].score,0,'f',1));
		static_cast<QLabel*>(gridLayout->itemAtPosition(i+1,2)->widget())->setStyleSheet(style);
	}
	
	iHighlightHighScore=-1;
}

/*!
  * Clears the currently shown high scores
  */
void highScoreWindow::vClear() {
	QMessageBox msgBox;
	msgBox.setWindowTitle(tr("Clear High Scores"));
	msgBox.setText(tr("Do you really want to clear the high scores for the map of %1?").arg(myDart->qlQcfxFiles[comboBox->currentIndex()].mapName));
	msgBox.setInformativeText(tr("This action cannot be undone."));
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Cancel);
	if(msgBox.exec()!=QMessageBox::Yes) return;
	
	for(int i=0; i<myDart->qlHighScores.length(); i++) {
		myDart->qlHighScores[i].name="---";
		myDart->qlHighScores[i].score=0;
	}
	
	myIO->vSaveHighScores(myDart->qlQcfxFiles[comboBox->currentIndex()].id);
	
	qDebug() << "[i] cleared" << myDart->qlQcfxFiles[comboBox->currentIndex()].id;
	
	vLoadHighScores(comboBox->currentIndex());
}
