/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "dart.hpp"

using namespace std;

dart::dart(QMainWindow *parent) : QMainWindow(parent) {
	
	qlImageLayers << "background" << "borders" << "rivers" << "elevations";
	
	qlComments << tr("Very Good!") << tr("Super!") << tr("Very Fine!")
	           << tr("Well Done!") << tr("Good!") << tr("That was good!")
	           << tr("Well.") << tr("That was OK.") << tr("That was reasonable.")
	           << tr("Oh boy!") << tr("Not really…") << tr("Not quite…")
	           << tr("Completely Wrong!") << tr("That wasn't much of a hit…") << tr("Missed completely!")
	           << tr("Read wrongly?") << tr("Clicked wrongly?") << tr("D'oh!");
	
	iPaddingTop=0;
	iMarginTop=0;
	dZoomFactor=1;
	iMaxPlaceCount=2;
	iPlaceCount=0;
	iCurrentPlayer=0;
	iAskForMode=enPositions;
	iNumberOfPlayers=2; // TODO we shouldn't change it in training mode (iNumberOfPlayersTrainingCache)
	qsCurrentPlaceType="city;state;town";
	bAcceptingClickEvent=TRUE;
	dPxToKm=1;
	iCurrentQcf=0;
	iScoreAreaMode=1;
	iTrainingPlaceNumber=-1;
        bAgainstTime=FALSE;
        iMaxTime=20;
        iGameMode=enLocal;
	qlPreferedQcfLanguage << "de" << "en" << "default";
	bResetCursor=TRUE;
	iToolMenuBarState=enBoth;
        
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(vTimeout()));
	
	iDelayNextCircle=200;
	iDelayBeforeShowingMark=500;
	iDelayBeforeNextPlayer=1000;
	iDelayBeforeNextPlace=2000;
	iDelayBeforeNextPlaceTraining=1000;
	
#ifdef Q_OS_WINCE
	// "error: unresolved external symbol time" when compiling for WinCE
	srand(GetTickCount());
#elif
	srand(time(NULL));
#endif
	
	myIO = new io(this);

	setupUi(this);
	toolBar->setMovable(FALSE);
	
	for(int i=0; i<4; i++) {
		QLabel *lblMap = new QLabel(this);
		lblMap->setAlignment(Qt::AlignTop);
		lblMap->setParent(centralwidget);
		lblMap->show();
		lblMap->setGeometry(0, 0, this->width(), this->width());
		qlMapLayers.append(lblMap);
	}
	
	lblMouseClickOverlay = new QMouseReleaseLabel(this);
	lblMouseClickOverlay->setParent(centralwidget); //we want the label to be placed under the toolbar
	lblMouseClickOverlay->setAlignment(Qt::AlignTop);
	lblMouseClickOverlay->show();
	lblMouseClickOverlay->setGeometry(0, 0, iGetWindowSize(), iGetWindowSize());
//	lblMouseClickOverlay->setCursor(QCursor(QPixmap("test.png"),1,1)); //TODO
	
	agGameMode = new QActionGroup(this);
	agGameMode->addAction(actionTraining);
	agGameMode->addAction(actionLocal);
//	agGameMode->addAction(actionNetwork);
	actionLocal->setChecked(true);
	
	agAskForMode = new QActionGroup(this);
	agAskForMode->addAction(actionPosition_of_Place);
	agAskForMode->addAction(actionName_of_Place);
	actionName_of_Place->setChecked(true);
	
	
	actionHigh_Score_List->setIcon(QIcon::fromTheme("games-highscores"));
	connect(actionConfigure,SIGNAL (triggered()), this, SLOT(vShowPreferences()));
	actionConfigure->setIcon(QIcon::fromTheme("configure"));
	connect(actionQuit,SIGNAL (triggered()), this, SLOT(vClose()));
	actionQuit->setIcon(QIcon::fromTheme("application-exit"));
	connect(actionNew_Game,SIGNAL (triggered()), this, SLOT(vNewGame()));
	actionNew_Game->setIcon(QIcon::fromTheme("document-new"));
	connect(actionFind_Place,SIGNAL (triggered()), this, SLOT(vShowAllPlaces()));
	actionFind_Place->setIcon(QIcon::fromTheme("edit-find"));
	connect(action100,SIGNAL (triggered()), this, SLOT(vResize()));
	action100->setIcon(QIcon::fromTheme("zoom-original"));
	connect(actionTraining,SIGNAL (triggered()), this, SLOT(vSetGameMode()));
	actionTraining->setIcon(QIcon::fromTheme("user-identity"));
	connect(actionNumber_of_Players,SIGNAL (triggered()), this, SLOT(vSetNumberOfPlayers()));
	connect(actionPlayers,SIGNAL (triggered()), this, SLOT(vSetNumberOfPlayers()));
	connect(actionLocal,SIGNAL (triggered()), this, SLOT(vSetGameMode()));
	actionLocal->setIcon(QIcon::fromTheme("system-users"));
	connect(actionAgainst_Time,SIGNAL (triggered()), this, SLOT(vSetAgainstTime()));
	actionAgainst_Time->setIcon(QIcon::fromTheme("player-time"));
	connect(actionName_of_Place,SIGNAL (triggered()), this, SLOT(vSetAskForMode()));
//	actionName_of_Place->setIcon(QIcon::fromTheme("user-identity"));
	connect(actionPosition_of_Place,SIGNAL (triggered()), this, SLOT(vSetAskForMode()));
//	actionPosition_of_Place->setIcon(QIcon::fromTheme("user-identity"));
	connect(actionCountries,SIGNAL (triggered()), this, SLOT(vSetPlaceType()));
	connect(actionCapitals_of_Countries,SIGNAL (triggered()), this, SLOT(vSetPlaceType()));
	connect(actionStates,SIGNAL (triggered()), this, SLOT(vSetPlaceType()));
	connect(actionCapitals_of_States,SIGNAL (triggered()), this, SLOT(vSetPlaceType()));
	connect(actionCounties,SIGNAL (triggered()), this, SLOT(vSetPlaceType()));
	connect(actionCities,SIGNAL (triggered()), this, SLOT(vSetPlaceType()));
	connect(actionTowns,SIGNAL (triggered()), this, SLOT(vSetPlaceType()));
	connect(actionAbout_Qt,SIGNAL (triggered()), qApp, SLOT(aboutQt()));
	connect(actionMenu_Bar,SIGNAL (triggered()), this, SLOT(vSetToolMenuBarState()));
	actionMenu_Bar->setIcon(QIcon::fromTheme("show-menu"));
	connect(actionToolbar,SIGNAL (triggered()), this, SLOT(vSetToolMenuBarState()));
	
	connect(lineEdit,SIGNAL (returnPressed()), this, SLOT(vReturnPressedEvent()));
	
	//these menus are needed for QToolButtons only and shouldn't be display in the main menus
	menubar->removeAction(menuApplication->menuAction());
	menuSettings->removeAction(menuAskForMode->menuAction());
	menuSettings->removeAction(menuPlaceType->menuAction());
	
	btApplication = new QToolButton(this);
	btApplication->setMenu(menuApplication);
	btApplication->setPopupMode(QToolButton::InstantPopup);
	btApplication->setText(tr("Game"));
	actionBtApplication=toolBar->addWidget(btApplication);
	
	toolBar->addAction(actionNew_Game);
	
	btGameMode = new QToolButton(toolBar);
	btGameMode->setMenu(menuGameMode);
	btGameMode->setPopupMode(QToolButton::InstantPopup);
	btGameMode->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	toolBar->addWidget(btGameMode);
	
	toolBar->addAction(actionPlayers);
	
	toolBar->addAction(actionAgainst_Time);
	
	btAskForMode = new QToolButton(toolBar);
	btAskForMode->setMenu(menuAskForMode);
	btAskForMode->setPopupMode(QToolButton::InstantPopup);
	btAskForMode->setToolButtonStyle(Qt::ToolButtonTextOnly);
	toolBar->addWidget(btAskForMode);
	
	menuPlace_Number = new QMenu(this);
	menuPlaceType->addMenu(menuPlace_Number);
	btPlaceType = new QToolButton(toolBar);
	btPlaceType->setMenu(menuPlaceType);
	btPlaceType->setPopupMode(QToolButton::InstantPopup);
	btPlaceType->setToolButtonStyle(Qt::ToolButtonTextOnly);
	btPlaceType->setText(tr("Place Types"));
	toolBar->addWidget(btPlaceType);
	
	btMap = new QToolButton(toolBar);
	btMap->setMenu(menuMap);
	btMap->setPopupMode(QToolButton::InstantPopup);
	btMap->setToolButtonStyle(Qt::ToolButtonTextOnly);
	toolBar->addWidget(btMap);
	
	btView = new QToolButton(toolBar);
	btView->setMenu(menuView);
	btView->setPopupMode(QToolButton::InstantPopup);
	btView->setToolButtonStyle(Qt::ToolButtonTextOnly);
	btView->setText(tr("View"));
	toolBar->addWidget(btView);
	
	
	if(myIO->iFindQcf()==0) {
		qDebug() << "[E] No valid qcfx files found, exiting";
		QMessageBox msgBox;
		msgBox.setText(tr("Sorry, no valid qcfx files could be found."));
		msgBox.setInformativeText(tr("You might want to add a file through Maps → Add map")); // TODO
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
	
	agMap = new QActionGroup(this);
	for(int i=0; i<qlQcfxFiles.count(); i++) {
		QAction *menuItem;
		menuItem = new QAction(QIcon(qlQcfxFiles[i].path+"/background.png"), qlQcfxFiles[i].mapName, this);
		menuItem->setToolTip(QString(tr("Load map of %1")).arg(qlQcfxFiles[i].mapName));
		menuItem->setCheckable(TRUE);
		connect(menuItem, SIGNAL(triggered()), this, SLOT(vReadQcf()));
		menuMap->addAction(menuItem);
		agMap->addAction(menuItem);
		if(i==0) menuItem->trigger(); // TODO setting
	}
	menuMap->addSeparator();
	menuMap->addAction(actionAdd_Map);
	
	gridLayout->setSpacing(1);
	
	vSetPlaceType(qsCurrentPlaceType);
	if(bAgainstTime) actionAgainst_Time->trigger();
	else vSetAgainstTime();
	switch(iGameMode) {
		case enTraining:
			actionTraining->trigger(); break;
		case enLocal:
		        actionLocal->trigger(); break;
	}
	switch(iAskForMode) {
		case enNames:
			actionName_of_Place->trigger(); break;
		case enPositions:
		        actionPosition_of_Place->trigger(); break;
	}
	vSetToolMenuBarState(iToolMenuBarState);
	
	vRepaintCommonLabels();
	vRepaintPlayerLabels();
	
	gridLayout->removeWidget(lblCurrentRound); // we do not want to seg fault
	gridLayout->addWidget(lblCurrentRound,0,4);
	
	vResize(1); // TODO saved value?
	
	show();
	
}

dart::~dart(){
	vRemoveAllCircles();
	vRemoveAllCommonPoints();
	//vSetNumberOfPlayers(0); // QGridLayout: Cannot add QLabel/lblTime to QGridLayout/gridLayout at row -1 column 4
        timer->stop();
        delete timer;
        delete myIO;
}

//draws distance circles using the saved click-coordinates of place n, iterating #count [recursion]
void dart::vDrawDistanceCircles(int n, int count) {
	bool drewCircle=FALSE;
	
	for(int i=0; i<iNumberOfPlayers; i++) {//draw circles for every player
		if(count*RADIUS+3*PENWIDTH < qlScoreHistory[i][n-1].diffPx) {
			vDrawCircle(qlScoreHistory[i][n-1].x,qlScoreHistory[i][n-1].y,(count+1)*RADIUS,i);
			drewCircle=TRUE;
		}
	}
	
	if(drewCircle && count<7) {
		mySleep(iDelayNextCircle);
		vDrawDistanceCircles(n,++count);
	}
}

void dart::vDrawCircle(int x, int y, int r, int player) {
	QLabel *circleLabel;
	circleLabel = new QCircleLabel(this,x,y,r,qlColorsOfPlayers[player],this);
	qlCircleLabels[player].append(circleLabel);
}

void dart::vRemoveAllCircles() {
	for(int i=0,max=qlCircleLabels.count();i<max;i++){
		qDebug()<<i;
		while(qlCircleLabels[i].count()!=0) {
			delete qlCircleLabels[i][0];
			qlCircleLabels[i].removeAt(0);
		}
	}
}

void dart::vRemoveAllCommonPoints() {
	while(qlPointLabels.count()!=0){
		delete qlPointLabels[0];
		qlPointLabels.removeAt(0);
	}
}

void dart::vTimeout() {
        if(iTimerElapsed==iMaxTime) return;
        qDebug() << ++iTimerElapsed;
        lblTime->setText(QString("%1").arg(iMaxTime-iTimerElapsed));
}

void dart::vSetPlaceType() {
	qsCurrentPlaceType="";
	if(actionCountries->isChecked()) qsCurrentPlaceType+="country;";
	if(actionCapitals_of_Countries->isChecked()) qsCurrentPlaceType+="capitalOfCountry;";
	if(actionStates->isChecked()) qsCurrentPlaceType+="state;";
	if(actionCapitals_of_States->isChecked()) qsCurrentPlaceType+="capitalOfState;";
	if(actionCounties->isChecked()) qsCurrentPlaceType+="county;";
	if(actionCities->isChecked()) qsCurrentPlaceType+="city;";
	if(actionTowns->isChecked()) qsCurrentPlaceType+="town;";
	myIO->vFillCurrentTypePlaces();
}
void dart::vSetPlaceType(QString placetype) {
	qsCurrentPlaceType=placetype;
	if(placetype.contains("country")) actionCountries->setChecked(TRUE);
	if(placetype.contains("capitalOfCountry")) actionCapitals_of_Countries->setChecked(TRUE);
	if(placetype.contains("state")) actionStates->setChecked(TRUE);
	if(placetype.contains("capitalOfState")) actionCapitals_of_States->setChecked(TRUE);
	if(placetype.contains("county")) actionCounties->setChecked(TRUE);
	if(placetype.contains("city")) actionCities->setChecked(TRUE);
	if(placetype.contains("town")) actionTowns->setChecked(TRUE);
	myIO->vFillCurrentTypePlaces();
}

void dart::vSetAgainstTime() {
	if( bCanLoseScore() ) return;
	vSetAgainstTime(actionAgainst_Time->isChecked());	
}
// resetting timer
void dart::vSetAgainstTime(bool enable) {
        bAgainstTime=enable;
        iTimerElapsed=0;
        if(bAgainstTime) {
                lblTime->setText(QString("%1").arg(iMaxTime-iTimerElapsed));
                timer->start(1000);
        } else {
                lblTime->setText("");
                timer->stop();
        }
}

void dart::vSetNumberOfPlayers() {
	bool ok;
	int min=iCurrentPlayer+1;
	int max=iPlaceCount>1 ? iNumberOfPlayers : 99;
	int n = QInputDialog::getInt(this, tr("Set Number Of Players"), tr("Number Of Players:"), iNumberOfPlayers, min, max, 1, &ok);
	if(n>15) {
		QMessageBox msgBox;
		msgBox.setWindowTitle(tr("Warning"));
		msgBox.setText(tr("You want to play with more than 15 players.\nAlthough QeoDart theoretically supports an infinite\nnumber of players, some display problems might occur.\n\nDo you want to continue anyway?"));
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		msgBox.setIcon(QMessageBox::Warning);
		if(msgBox.exec()==QMessageBox::Cancel) return;
	}
	if(ok) vSetNumberOfPlayers(n);
}
void dart::vSetNumberOfPlayers(int n) {
	iNumberOfPlayers=n;
	qDebug() << "[i] iNumberOfPlayers" << iNumberOfPlayers;
	if(iNumberOfPlayers>15) qDebug() << "[w] very much players";
	
	actionPlayers->setText(QString(tr("Players: %1")).arg(iNumberOfPlayers));
	
	qDebug()<<qlPlayerLabels.count()<<"sssssssssss"<<n;
	
	if(qlPlayerLabels.count()>n) {
		
		for(int i=qlPlayerLabels.count()-1; i>iNumberOfPlayers-1; i--) {
			
// 			QList<QLabel*> *qlPlayerLabel=qlPlayerLabels[i];
			qDebug()<<"f"<<i;
			qDebug() << "fdddfff" << qlPlayerLabels[0].count();
			for(int j=0,max=qlPlayerLabels[i].count(); j<max; j++) {
				gridLayout->removeWidget(qlPlayerLabels[i][j]);
				delete qlPlayerLabels[i][j];
				qDebug()<<"ff";
			}
			qlPlayerLabels[i].clear();
// 			~qlPlayerLabels[i];			//TODO we should do it somehow, shouldn't we?
			qlPlayerLabels.removeAt(i);
			
			qlScoreHistory[i].clear(); //TODO must delete sub-lists?
			qlScoreHistory.removeAt(i);
			qlColorsOfPlayers.removeAt(i);
			
		}
		
	} else if(qlPlayerLabels.count()<n) {
		
		while(qlPlayerLabels.count()<iNumberOfPlayers){
			int i=qlPlayerLabels.count();
			qDebug()<<"player labels available"<<i;
			
			QLabel *lblScore;
			lblScore = new QLabel(this);
			gridLayout->addWidget(lblScore,i,0);
			lblScore->setText(QString(tr("<span>%1 Points &#8960; %2, %3</span>")).arg(0).arg(0.0,0,'f',1).arg(0.0,0,'f',1));
			
			QLabel *lblRating;
			lblRating = new QLabel(this);
			gridLayout->addWidget(lblRating,i,2);
			
			QList<QLabel*> qlPlayerLabel;
			qlPlayerLabel << lblScore << lblRating;
			qlPlayerLabels.append(qlPlayerLabel);
			
			QList<QLabel*> qlCircleLabel;
			qlCircleLabels.append(qlCircleLabel);
			
			QList<scoreHistory> qlHistory;
			qlScoreHistory.append(qlHistory);
			
			// usually we do this when starting a new game, but since we allow increasing the number of players
			// during the first round, we must add totalScore here
			while(qlTotalScores.count()<iNumberOfPlayers) {
				totalScore ts;
				ts.score=0;
				ts.mark=0;
				qlTotalScores.append(ts);
			}
			
			qlColorsOfPlayers.append(qcGetColorOfPlayer(qlPlayerLabels.count()-1));
		}
	}
	
	gridLayout->removeWidget(lblCurrentPlace);
	gridLayout->removeWidget(lblCurrentPlayer);
	gridLayout->removeWidget(lineEdit);
	gridLayout->removeWidget(lblTime);
	if(iNumberOfPlayers==1) {
		gridLayout->removeWidget(lblComment);
		gridLayout->addWidget(lblCurrentPlace,1*(iGameMode!=enTraining),0);
		gridLayout->addWidget(lblCurrentPlayer,1,4);
		gridLayout->addWidget(lblComment,1,2);
		gridLayout->addWidget(lineEdit,1*(iGameMode!=enTraining),0);
                gridLayout->addWidget(lblTime,1*(iGameMode!=enTraining),4);
		lblComment->setText("");
		lblComment->show();
                lblCurrentPlayer->hide();
	} else {
		gridLayout->addWidget(lblCurrentPlace,iNumberOfPlayers,0);
		gridLayout->addWidget(lblCurrentPlayer,iNumberOfPlayers,4);
		gridLayout->addWidget(lineEdit,iNumberOfPlayers,0);
                gridLayout->addWidget(lblTime,iNumberOfPlayers-1,4);
		lblComment->hide();
                lblCurrentPlayer->show();
	}
	
// 	gridLayout->setGeometry(QRect(0, 0, 10, 10));
// 	gridLayout->setSpacing(1);
// 	gridLayout->setContentsMargins(0,0,0,0);
	gridLayout->removeItem(spGridLayoutVertical);
	gridLayout->addItem(spGridLayoutVertical,iNumberOfPlayers+1,0);
	
	resizeEvent(0); // the label's font size and iPaddingTop must be recalculated
}

void dart::resizeEvent(QResizeEvent *event) {
	//maybe we want to preserve the shape of the window TODO use timer?
// 	int w=this->width(), h=this->height()-toolBar->height()-menubar->height(), n=this->width()<this->height()-toolBar->height()-menubar->height()?this->width():this->height()-toolBar->height()-menubar->height();
// 	cout << w << " " << h << endl;
// 	if(w!=h) resize(n,n+toolBar->height()+menubar->height());

// 	QRect qr availableGeometry(screenNumber(this));
// 	int availableHeight=qr.height();
// 	int availableWidth=qr.width();
	
	iMarginTop=toolBar->height()+menubar->height();
	
	dZoomFactor=iGetWindowSize()/600.0; //TODO must be calculated in another way
	
	vRepaintPlayerLabels();
	vRepaintCommonLabels();
	iPaddingTop=iGetPaddingTop();
	
	dZoomFactor=iGetWindowSize()/600.0;
	
	lblMouseClickOverlay->resize(600*dZoomFactor,600*dZoomFactor);
	lblMouseClickOverlay->move(0,iPaddingTop);
	
	vRepaintMap();
	
	gridLayoutWidget->setGeometry(QRect(0,0,600*dZoomFactor+1,iPaddingTop+50));

	qDebug() << "[i] iPaddingTop" << iPaddingTop << "iMarginTop" << iMarginTop << "dZoomFactor" << dZoomFactor << "fontSize" << iGetFontSize();
}

void dart::vResize(double dNewZoomFactor) {
        showNormal();
	
	dZoomFactor=dNewZoomFactor;
	iPaddingTop=iGetPaddingTop();
	resize(600*dZoomFactor,iMarginTop+iPaddingTop+600*dZoomFactor);
}

void dart::vRepaintMap() {
	QString path=qlQcfxFiles[iCurrentQcf].path;
	
	for(int i=0; i<4; i++) {
		qlMapLayers[i]->resize(600*dZoomFactor,600*dZoomFactor);
		qlMapLayers[i]->setText(QString("<img src=\"%2/%3.png\" height=\"%1\" width=\"%1\"/>").arg(600*dZoomFactor).arg(path).arg(qlImageLayers[i]));
		qlMapLayers[i]->move(0,iPaddingTop);
	}
}

//repaints all labels which are player-specific
void dart::vRepaintPlayerLabels() {
	int fontSize=iGetFontSize();
	for(int i=0,max=qlPlayerLabels.count(); i<max; i++) {
		for(int j=0,max=qlPlayerLabels[i].count(); j<max; j++) {
			qlPlayerLabels[i][j]->setStyleSheet(QString("color:%2;font-size:%1px;font-family:arial,sans-serif").arg(fontSize).arg(qlColorsOfPlayers[i].name()));
		}
	}
}

//repaint all label which are not player-specific
void dart::vRepaintCommonLabels() {
	int fontSize=iGetFontSize();
        QString stylesheet=QString("color:%2;font-size:%1px;font-family:arial,sans-serif")
                        .arg(fontSize)
                        .arg(qlColorsOfPlayers[iCurrentPlayer].name());
	lblCurrentPlace->setStyleSheet(stylesheet);
	lblComment->setStyleSheet(stylesheet);
	lblCurrentRound->setStyleSheet(stylesheet);
        lblTime->setStyleSheet(stylesheet);
	lblCurrentPlayer->setStyleSheet(stylesheet);
	lblCurrentPlayer->setText(QString(tr("Player %1")).arg(iCurrentPlayer+1));
        lineEdit->setMaximumHeight(fontSize+2);
}

int dart::iGetFontSize() {
	return 20*dZoomFactor<10 ? 10 : 20*dZoomFactor;
}

int dart::iGetPaddingTop() {
	return (qlPlayerLabels.count()+1*(iGameMode!=enTraining)) * (iGetFontSize()+6);
}

// draws a point at P(x|y) with the label name, and adds it to the list list
void dart::vDrawPoint(int x, int y, QList<QLabel*> &list, QString name, QColor color) {
	QLabel *lblCurrentPlacePosition;
	lblCurrentPlacePosition = new QPointLabel(this, name, x, y, color, this);
	list.append(lblCurrentPlacePosition);
// 	lblCurrentPlace->setGeometry(x,y+iPaddingTop,50,50);
// 	lblCurrentPlace->show();
	qDebug() << "[i] drew point" << x << y << "+" << iMarginTop;
}
void dart::vDrawPoint(int x, int y, QList<QLabel*> &list, QColor color, QString name) {
	vDrawPoint(x, y, list, name, color);
}

// draws the click positions of all players for round n
void dart::vDrawClickPositions(int n) {
	for(int i=0; i<iNumberOfPlayers; i++) { //draw circles for every player
		vDrawPoint(qlScoreHistory[i][n-1].x,qlScoreHistory[i][n-1].y,qlCircleLabels[i],qlColorsOfPlayers[i]);
	}
}

void dart::vShowAllPlaces() {
	for(int i=0, max=qlCurrentTypePlaces.count(); i<max; i++){ //TODO WITH?
		vDrawPoint(qlCurrentTypePlaces[i]->x,qlCurrentTypePlaces[i]->y,qlPointLabels,qlCurrentTypePlaces[i]->name);
	}
}

void dart::vMouseClickEvent(int x, int y) {
	if(!bAcceptingClickEvent) return;
	bAcceptingClickEvent=FALSE;
	if(iAskForMode!=enPositions) return;
	
	x=iGetUnzoomed(x);
	y=iGetUnzoomed(y);
	vDrawPoint(x,y,qlCircleLabels[iCurrentPlayer],qlColorsOfPlayers[iCurrentPlayer]);

	scoreHistory score;
	score.x=x;
	score.y=y;
	score.diffPxArea=dGetDistanceInPx(x,y,iPlaceCount-1); // respects area // TODO what about shown distance?
	score.diffPx=dGetDistanceInPxBetween(x,y,qlCurrentTypePlaces[qlPlacesHistory[iPlaceCount-1]]->x,qlCurrentTypePlaces[qlPlacesHistory[iPlaceCount-1]]->y);
	score.diffKm=dGetDistanceInKm(score.diffPxArea);
	score.mark=dGetMarkFromDistance(score.diffPxArea);
	score.score=dGetScore(score.mark);
        if(bAgainstTime) {
                score.score*=1-static_cast<double>(iTimerElapsed)/iMaxTime;
                score.mark=dGetMarkFromScore(score.score);
        }
	qlScoreHistory[iCurrentPlayer].append(score);
	
	if(! (iGameMode==enTraining && iPlaceCount>=5) ) {
		qlTotalScores[iCurrentPlayer].score+=score.score;
		qlTotalScores[iCurrentPlayer].mark=dGetAverageMarkOfPlayer(iCurrentPlayer);
	}
	
	
	qDebug() << "Score for Player" << iCurrentPlayer << score.diffPx << "px" << score.diffPxArea << "px (area)" << score.diffKm << "km" << score.score << "p"  << score.mark << "TOTAL" << qlTotalScores[iCurrentPlayer].score << "p" << qlTotalScores[iCurrentPlayer].mark;
	
	if(iCurrentPlayer<iNumberOfPlayers-1) { // next player
		
		mySleep(iDelayBeforeNextPlayer);
		
		if(bResetCursor) QCursor::setPos(QWidget::x()+5,QWidget::y()+iPaddingTop+iMarginTop+10);
		
		iCurrentPlayer++;
		vRemoveAllCircles();
		qDebug()<<"f";
		vRepaintPlayerLabels();
		vRepaintCommonLabels();
                vSetAgainstTime(bAgainstTime);
		
		bAcceptingClickEvent=TRUE;
		
	} else { // show results
		
		iCurrentPlayer=0;
		
		vRemoveAllCircles();
		vDrawClickPositions(iPlaceCount);
		mySleep(iDelayNextCircle);
		vDrawDistanceCircles(iPlaceCount, 0);
		
		// show real position
		vShowCurrentPlace();
		
		vShowScores();
                
                vShowComment();
		
		if(iGameMode==enTraining) mySleep(iDelayBeforeNextPlaceTraining);
		else mySleep(iDelayBeforeNextPlace);
                
                lblComment->setText("");
		
		vShowTotalScores();
		
		vRemoveAllCircles();
		vRemoveAllCommonPoints();
		
		if(iGameMode!=enTraining) vResetScoreLabels();
		
		vRepaintCommonLabels();
		
		vNextRound();
		
	}
}

void dart::vShowComment() {
        if(iNumberOfPlayers==1 && iGameMode!=enTraining) {
		// we shoudln't use the saved mark here as inappropriate comments could be shown
		// (e.g "clicked wrongly", although clicked correctly, but bad time)
		double mark=dGetMarkFromDistance(qlScoreHistory[0][iPlaceCount-1].diffKm);
                int i = rand() % 3 + 3*(static_cast<int>(mark)-1);
                qDebug() << "[i]" << mark << "comment #" << i;
                lblComment->setText(qlComments[i]);
        }
}

void dart::vShowResultWindows() {
	bAcceptingClickEvent=FALSE;
	
	for(int i=0,max=qlPlacesHistory.count(); i<max; i++) {
		vDrawPoint(qlCurrentTypePlaces[qlPlacesHistory[i]]->x, qlCurrentTypePlaces[qlPlacesHistory[i]]->y, qlPointLabels, qlCurrentTypePlaces[qlPlacesHistory[i]]->name);
	}
	
	for(int i=0; i<iNumberOfPlayers; i++) {
		resultWindow dialog(this,i);
		dialog.exec();
	}
	
	iPlaceCount=0; // needed for quit?-dialog
}

void dart::vResetScoreLabels() {
	for(int i=0; i<iNumberOfPlayers; i++) { // reset score labels of each player
		qlPlayerLabels[i][1]->setText("");
	}
}

void dart::vShowCurrentPlace() {
	if(iAskForMode==enPositions) vDrawPoint(qlCurrentTypePlaces[qlPlacesHistory[iPlaceCount-1]]->x,qlCurrentTypePlaces[qlPlacesHistory[iPlaceCount-1]]->y,qlPointLabels);
	else vDrawPoint(qlCurrentTypePlaces[qlPlacesHistory[iPlaceCount-1]]->x,qlCurrentTypePlaces[qlPlacesHistory[iPlaceCount-1]]->y,qlPointLabels, qlCurrentTypePlaces[qlPlacesHistory[iPlaceCount-1]]->name);
}

void dart::vShowScores() {
	for(int i=0; i<iNumberOfPlayers; i++) { // show score for each player
		QString km, score;
		if(qlScoreHistory[i][iPlaceCount-1].diffKm==-1) km=tr("∞");
		else km=QString("%1").arg(qlScoreHistory[i][iPlaceCount-1].diffKm,0,'f',1);
		if(iGameMode!=enTraining) score=QString(" +%1").arg(qlScoreHistory[i][iPlaceCount-1].score);
		qlPlayerLabels[i][1]->setText(QString(tr("Missed by %1 km (%2)%3"))
		                              .arg(km)
		                              .arg(qlScoreHistory[i][iPlaceCount-1].mark,0,'f',1)
		                              .arg(score));
	}
}

void dart::vShowTotalScores() {
	for(int i=0; i<qlPlayerLabels.count(); i++) { // show score for each player
		qlPlayerLabels[i][0]->setText(QString(tr("<span>%1 Points &#8960; %2, %3</span>")).arg(qlTotalScores[i].score).arg(dGetAverageScoreOfPlayer(i),0,'f',1).arg(qlTotalScores[i].mark,0,'f',1));
	}
}

int dart::iGetWindowSize() {
	return (width()<height()-iMarginTop-iPaddingTop) ? width() : (height()-iMarginTop-iPaddingTop);
}

void dart::vClose() {
	close();
}

void dart::closeEvent(QCloseEvent *event) {
	if(iPlaceCount>1 && iGameMode!=enTraining) {
		QMessageBox msgBox;
		msgBox.setWindowTitle(tr("Quit QeoDart"));
		msgBox.setText(tr("Do you really want to quit QeoDart?"));
		msgBox.setInformativeText(tr("Your current score will be lost."));
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		if(msgBox.exec()==QMessageBox::Yes) event->accept();
		else event->ignore();
	} else {
		event->accept();
	}
}

int dart::iGetUnzoomed(double x) {
	return x/dZoomFactor;
}

void dart::mySleep(int ms) {
	Q_ASSERT(QCoreApplication::instance());
	QTime timer;
	timer.start();
	do {
		QCoreApplication::processEvents(QEventLoop::AllEvents, ms);
#ifdef Q_OS_UNIX
		struct timespec t;
		t.tv_sec  = 0;
		t.tv_nsec = 10000;
		nanosleep(&t,NULL);
#endif
#ifdef Q_OS_WIN32
		_sleep(10);
#endif
	} while (timer.elapsed() < ms);
}

QColor dart::qcGetColorOfPlayer(int player) {
	int m=player%6, i=255/pow(2,player/6);
	QColor c=QColor(0,0,0,255);
	
	if(m==0 || m==3 || m==5) c.setBlue(i);
	if(m==1 || m==3 || m==4) c.setGreen(i);
	if(m==2 || m==4 || m==5) c.setRed(i);
	
	return c;
}

void dart::vSetGameMode() {
	if( bCanLoseScore() ) {
		QMessageBox msgBox;
		msgBox.setWindowTitle(tr("Chance Game Mode"));
		msgBox.setText(tr("When you change the game mode, your current score will be lost.\nDo you want to continue?"));
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		if(msgBox.exec()==QMessageBox::Cancel) return;
	}
	
	btGameMode->setText(QString("%1").arg(static_cast<QAction*>(QObject::sender())->text()));
	btGameMode->setIcon(static_cast<QAction*>(QObject::sender())->icon());
	
	if(QObject::sender()==actionTraining) {
		vSetGameMode(enTraining);
	} else if(QObject::sender()==actionLocal) {
		vSetGameMode(enLocal);
	} else {
		qDebug() << "[E] vSetGameMode: unknown sender";
	}
}
void dart::vSetGameMode(enGameModes mode) {
	switch(iGameMode) {
		case enTraining:
			lblCurrentRound->show();
			lblCurrentPlayer->show();
			qlPlayerLabels[0][0]->show();
			actionNumber_of_Players->setEnabled(TRUE);
			actionPlayers->setEnabled(TRUE);
			vResetScoreLabels();
			break;
		case enLocal:
			break;
	}
	
	iGameMode=mode;
	
	vResetForNewGame();
	
	switch(iGameMode) {
		case enTraining:
			lblCurrentRound->hide();
			lblCurrentPlayer->hide();
			qlPlayerLabels[0][0]->hide();
			actionNumber_of_Players->setEnabled(FALSE);
			actionPlayers->setEnabled(FALSE);
			
			vSetNumberOfPlayers(1);
			break;
		case enLocal:
			vSetNumberOfPlayers(iNumberOfPlayers);
			break;
	}
	
	vNextRound();
}

void dart::vSetAskForMode() {
	if( bCanLoseScore() ) {
		QMessageBox msgBox;
		msgBox.setWindowTitle(tr("Chance Mode"));
		msgBox.setText(tr("When you change this setting, your current score will be lost.\nDo you want to continue?"));
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		if(msgBox.exec()==QMessageBox::Cancel) return;
	}
	
	btAskForMode->setText(QString(tr("Ask for: %1").arg(static_cast<QAction*>(QObject::sender())->text())));
	
	if(QObject::sender()==actionName_of_Place) {
		vSetAskForMode(enNames);
	} else if(QObject::sender()==actionPosition_of_Place) {
		vSetAskForMode(enPositions);
	} else {
		qDebug() << "[E] vSetAskForMode: unknown sender";
	}
}
void dart::vSetAskForMode(enAskForModes mode) {
	iAskForMode=mode;
	
	vResetForNewGame();
	
	switch(iAskForMode) {
		case enPositions:
			lineEdit->hide();
			lblCurrentPlace->show();
			break;
		case enNames:
			lineEdit->show();
			lblCurrentPlace->hide();
			break;
	}
	
	vSetNumberOfPlayers(iNumberOfPlayers);
	
	vNextRound();
}

void dart::vResetForNewGame() {
	vRemoveAllCircles();
	vRemoveAllCommonPoints();
	qlPlacesHistory.clear();
	
	for(int i=qlScoreHistory.count()-1; i>-1; i--) {
		qlScoreHistory[i].clear(); //TODO must delete sub-lists?
	}
	
	qlTotalScores.clear();
	for(int i=0; i<iNumberOfPlayers; i++) {
		totalScore ts;
		ts.score=0;
		ts.mark=0;
		qlTotalScores.append(ts);
	}
	iPlaceCount=0;
	
	vShowTotalScores();
//	vRepaintCommonLabels();
}

void dart::vNewGame() {
	if( bCanLoseScore() ) {
		QMessageBox msgBox;
		msgBox.setWindowTitle(tr("New Game"));
		msgBox.setText(tr("When you start a new game, your current score will be lost.\nDo you want to continue?"));
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		if(msgBox.exec()==QMessageBox::Cancel) return;
	}
	
	iCurrentPlayer=0;
	vSetNumberOfPlayers(iNumberOfPlayers);
	vSetGameMode(iGameMode);
}

void dart::vNextRound() {
	iCurrentPlayer=0;
	
	if(iPlaceCount==iMaxPlaceCount && iGameMode!=enTraining) {
		vShowResultWindows();
		return;
	}
	
	int pn=-1;
	
	if(iGameMode==enTraining && iPlaceCount>=5) {
		qDebug() << "Revise";
		for(int i=0; i<qlScoreHistory[0].count() && pn==-1; i++) {
			qDebug() << qlScoreHistory[0][i].mark << qlTotalScores[0].mark;
			if(qlScoreHistory[0][i].mark>=4 || (qlScoreHistory[0][i].mark>2 && qlScoreHistory[0][i].mark>qlTotalScores[0].mark) ) {
				qDebug() << qlCurrentTypePlaces[qlPlacesHistory[i]]->name;
				
				if(iPlaceCount==5 && i==4) {
					//we shouldn't ask for the last place immediatly
					//so we keep it in mind
					iTrainingPlaceNumber=qlPlacesHistory[i];
				} else {
					qlScoreHistory[0][i].mark=0;
					pn=qlPlacesHistory[i];
				}
			} //if (badscore)
		} // for (scorehistory)
		if(pn==-1) {
			vResetForNewGame();
		}
	}
	
	if(pn<=-1) {
		int i=0;
		do {
			qDebug() << qlCurrentTypePlaces.count();
			pn = rand() % qlCurrentTypePlaces.count();
		} while(qlPlacesHistory.contains(pn) && i++<10);
		
		if(iGameMode==enTraining && iTrainingPlaceNumber!=-1 && iPlaceCount==2) {
			pn=iTrainingPlaceNumber;
			iTrainingPlaceNumber=-1;
		}
	}
	
	iPlaceCount++;
	qlPlacesHistory.append(pn);
	
	qDebug() << "[i] next place:" << pn << qlCurrentTypePlaces[pn]->name << iPlaceCount << "/" << iMaxPlaceCount;
	
	switch(iAskForMode) {
		case enPositions:
			bAcceptingClickEvent=TRUE;
			lblCurrentPlace->setText(qlCurrentTypePlaces[pn]->name);
			break;
		case enNames:
			vDrawPoint(qlCurrentTypePlaces[pn]->x, qlCurrentTypePlaces[pn]->y, qlPointLabels);
			lineEdit->clear();
			lineEdit->setStyleSheet("");
			lineEdit->setEnabled(TRUE);
			lineEdit->setFocus(Qt::OtherFocusReason);
			break;
	};
	
	switch(iGameMode) {
		case enTraining:
			break;
		case enLocal:
			lblCurrentRound->setText(QString(tr("Place %1 of %2")).arg(iPlaceCount).arg(iMaxPlaceCount));
			break;
	};
        
        if(bAgainstTime) vSetAgainstTime(TRUE);
}

//returns the distance between P(a|b) and Q(x|y); a,b,x,y should be unzoomed
double dart::dGetDistanceInPxBetween(int a, int b, int x, int y) {
	if( (a==-1 && b==-1) || (x==-1 && y==-1) ) return -1;
	return sqrt( pow(a-x,2) + pow(b-y,2) ); //thx Pythagoras
}

//returns the distance between P(a|b) and place #n [>=0], respecting iScoreAreaMode
double dart::dGetDistanceInPx(int a, int b, int n) {
	if(a==-1 && b==-1) return -1;
	
	int x=qlCurrentTypePlaces[qlPlacesHistory[n]]->x;
	int y=qlCurrentTypePlaces[qlPlacesHistory[n]]->y;
	
	if(iAskForMode==enNames) return dGetDistanceInPxBetween(a,b,x,y);
	
	int dim2x=qlCurrentTypePlaces[qlPlacesHistory[n]]->dimx*(iScoreAreaMode/2.0)/2;
	int dim2y=qlCurrentTypePlaces[qlPlacesHistory[n]]->dimx*(iScoreAreaMode/2.0)/2;
	
	if(a>x+dim2x) a-=dim2x;
	else if(a<x-dim2x) a+=dim2x;
	else a=x;
	
	if(b>y+dim2y) b-=dim2y;
	else if(b<y-dim2y) b+=dim2y;
	else b=y;
	
	return dGetDistanceInPxBetween(a,b,x,y);
}

double dart::dGetDistanceInKm(double px) {
	if(px==-1) return -1;
	return px*dPxToKm;
}

//calculate the mark (German system TODO other systems) using unzoomed distance in px
double dart::dGetMarkFromDistance(double distance) {
	if(distance==-1) return 6;
	
	if(distance>1) distance--; // a difference of 1px is OK 
	else distance=0;

	double mark=distance/RADIUS+1;
	if(mark<4) {
		return mark<1 ? 1 : mark;
	} else {
		mark=4+(mark-4)/2;
		return mark>6 ?  6 : mark;
	}
}

double dart::dGetScore(double mark) {
	double score;
	cout<<-16.66*mark+116.66; // I really don't know why, but w/o this line the function never returns 100
	if(mark<4) score=-16.66*mark+116.66;
	else score=-25*mark+150;
	if(score<0) score=0;
	return score;
}

double dart::dGetMarkFromScore(double score) {
        double mark;
	if(score>=50) {
                mark=(score-116.66)/-16.66;
                return mark<1 ? 1 : mark;
	} else {
                mark=(score-150)/-25;
                return mark>6 ? 6 : mark;
        }
}

double dart::dGetAverageMarkOfPlayer(int player) {
	return dGetMarkFromScore(dGetAverageScoreOfPlayer(player));
}

double dart::dGetAverageScoreOfPlayer(int player) {
	if(iPlaceCount<1) return 0;
	return qlTotalScores[player].score/iPlaceCount;
}

// looks for lineEdit->text() in the list of places; returns the index for place in qlAllPlaces
int dart::iFindInputInList(double &f) {
	QString input=lineEdit->text();
	qDebug() << "[i] input" << input;
	
	int index=-1;
	
	for(int l=0; (l<3 && index==-1); l++) {
		qDebug() << "l" << l;
		
		if(l==0) f=1;
		else if(l==1) f=0.75;
		else if(l==2) f=0.5; //TODO setting
		
		input=qsSimplifyString(input, l);
		
		for(int i=0,max=qlAllPlaces.count(); (i<max && index==-1); i++) {
			if(qsSimplifyString(qlAllPlaces[i].name,l)==input) index=i;
		}
	}
	
	if(index!=-1) qDebug() << "[i] found" << qlAllPlaces[index].name << f;
	else qDebug() << "[i] not found" << f;
	
	return index;
}

QString dart::qsSimplifyString(QString str, int l) {
	if(l>=0) {
		str=str.toLower();
		str=str.replace("am ", "a");
		str=str.replace("an ", "a");
		str=str.replace("der ", "d");
		str=str.replace(" ", "");
		
		if(l>=1) {
			str=str.replace("-", "");
			str=str.replace("_", "");
			str=str.replace("_", "");
			str=str.replace(".", "");
			str=str.replace(QRegExp("([aou])e"), "\\1");
			str=str.replace(QRegExp(QString::fromUtf8("[äáàã]")), "a");
			str=str.replace(QRegExp(QString::fromUtf8("[éèẽ]")), "e");
			str=str.replace(QRegExp(QString::fromUtf8("[íìĩ]")), "i");
			str=str.replace(QRegExp(QString::fromUtf8("[öṏø]")), "o");
			str=str.replace(QRegExp(QString::fromUtf8("[üǘǜ]")), "u");
			str=str.replace(QRegExp(QString::fromUtf8("[ßś]")), "s");
			str=str.replace("ss", "s");
			
			if(l>=2) {
				str=str.replace("ai", "ei");
				str=str.replace("ay", "ei");
				str=str.replace("ie", "i");
				str=str.replace("d", "t");
				str=str.replace("ck", "k");
				str=str.replace("c", "k");
				str=str.replace("kh", "ch");
				str=str.replace("g", "k");
				str=str.replace("w", "v");
				str=str.replace("f", "v");
                                //remove double letters and lengthening
                                for(int i=0; i<str.length()-1; i++) {
                                        if(
                                           str[i]==str[i+1] ||
                                           (QString(str[i]).contains(QRegExp("[aeiou]")) && str[i+1]=='h') ||
                                           (QString(str[i]).contains(QRegExp("[aeou]")) && str[i+1]=='r') ||
					   (QString(str[i]).contains(QRegExp("[ou]")) && str[i+1]=='i')
                                          ) {
                                                str=str.remove(i+1,1);
                                                i--;
                                        }
                                }
                                qDebug()<<str;
			} // 2
		} // 1
	} // 0
	
	return str;
}

void dart::vReturnPressedEvent() { // TODO split (net!)
	if(iAskForMode!=enNames) {
		qDebug() << "[E] vReturnPressedEvent";
		return;
	}
	lineEdit->setEnabled(FALSE);
	
	double f;
	int iIndexOfPlace=iFindInputInList(f);
	
	lineEdit->setStyleSheet("color:black");
	if(f<1) {
		lineEdit->setStyleSheet("text-decoration:underline;color:black");
		if(f<0.75) {
			lineEdit->setStyleSheet("text-decoration:underline;color:red");
			if(iIndexOfPlace==-1) {
				lineEdit->setStyleSheet("text-decoration:underline;color:red;background:black");
			}
		}
	}
	
	int x,y;
	if(iIndexOfPlace!=-1) {
		x=qlAllPlaces[iIndexOfPlace].x;
		y=qlAllPlaces[iIndexOfPlace].y;
	} else {
		x=-1;
		y=-1;
	}

	scoreHistory score;
	score.x=x;
	score.y=y;
	score.diffPxArea=dGetDistanceInPx(x,y,iPlaceCount-1); // respects area // TODO what about shown distance?
	score.diffPx=dGetDistanceInPxBetween(x,y,qlCurrentTypePlaces[qlPlacesHistory[iPlaceCount-1]]->x,qlCurrentTypePlaces[qlPlacesHistory[iPlaceCount-1]]->y);
	score.diffKm=dGetDistanceInKm(score.diffPxArea);
	score.mark=dGetMarkFromDistance(score.diffPxArea);
        score.score=dGetScore(score.mark)*f;
        if(bAgainstTime) {
                //typing against time can be hard, so there are some bonusSeconds
                double bonusSeconds=lineEdit->text().length()/8.0;
                iTimerElapsed-=bonusSeconds;
                if(iTimerElapsed<0) iTimerElapsed=0;
                score.score*=1-static_cast<double>(iTimerElapsed)/iMaxTime;
        }
	score.mark=dGetMarkFromScore(score.score);
        
	qlScoreHistory[iCurrentPlayer].append(score);
	
	if(! (iGameMode==enTraining && iPlaceCount>=5) ) {
		qlTotalScores[iCurrentPlayer].score+=score.score;
		qlTotalScores[iCurrentPlayer].mark=dGetAverageMarkOfPlayer(iCurrentPlayer);
	}
	
	
	qDebug() << "Score for Player" << iCurrentPlayer << score.diffPx << "px" << score.diffPxArea << "px (area)" << score.diffKm << "km" << score.score << "p"  << score.mark << "TOTAL" << qlTotalScores[iCurrentPlayer].score << "p" << qlTotalScores[iCurrentPlayer].mark;
	
	if(iCurrentPlayer<iNumberOfPlayers-1) { // next player
		
		mySleep(iDelayBeforeNextPlayer);
		iCurrentPlayer++;
		vRemoveAllCircles();
		qDebug()<<"f";
		vRepaintPlayerLabels();
		vRepaintCommonLabels();
		
		lineEdit->clear();
		lineEdit->setStyleSheet("");
		lineEdit->setEnabled(TRUE); //c//
		lineEdit->setFocus(Qt::OtherFocusReason);
                
                vSetAgainstTime(bAgainstTime);
		
	} else { // show results
		
		iCurrentPlayer=0;
		
		vRemoveAllCircles();
		vDrawClickPositions(iPlaceCount);
		mySleep(iDelayNextCircle);
		vDrawDistanceCircles(iPlaceCount, 0);
		
		// show real position
		vShowCurrentPlace();
		
		vShowScores();
                
                vShowComment();
		
		if(iGameMode==enTraining) mySleep(iDelayBeforeNextPlaceTraining);
		else mySleep(iDelayBeforeNextPlace);
                
                lblComment->setText("");
		
		vShowTotalScores();
		
		vRemoveAllCircles();
		vRemoveAllCommonPoints();
		
		if(iGameMode!=enTraining) vResetScoreLabels();
		
		vRepaintCommonLabels();
		
		vNextRound();
		
	}
	//cE//
}

void dart::vReadQcf() {
	if(iPlaceCount>1 || iCurrentPlayer!=0) {
		QMessageBox msgBox;
		msgBox.setWindowTitle(tr("Chance Map"));
		msgBox.setText(tr("When you change the map, your current score will be lost.\nDo you want to continue?"));
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		if(msgBox.exec()==QMessageBox::Cancel) return;
	}
	
	if(myIO->iReadQcf(static_cast<QAction*>(QObject::sender())->text())!=0) exit(-1);
	
	btMap->setText(QString(tr("Map: %1")).arg(static_cast<QAction*>(QObject::sender())->text()));
	
	vRepaintMap();
	vResetForNewGame();
	vNextRound();
}

void dart::vSetToolMenuBarState() {
	if (actionMenu_Bar->isChecked() && actionToolbar->isChecked()) iToolMenuBarState=enBoth;
	else if (actionMenu_Bar->isChecked() && !actionToolbar->isChecked()) iToolMenuBarState=enMenuBarOnly;
	else if (!actionMenu_Bar->isChecked() && actionToolbar->isChecked()) iToolMenuBarState=enToolBarOnly;
	else { actionToolbar->trigger(); return; }
	
	switch(iToolMenuBarState) {
		case enBoth:
			toolBar->removeAction(actionBtApplication);
			menuApplication->setVisible(FALSE);
			menubar->setVisible(TRUE);
			toolBar->setVisible(TRUE);
			break;
			
		case enToolBarOnly:
			toolBar->insertAction(actionNew_Game, actionBtApplication);
			menubar->setVisible(FALSE);
			toolBar->setVisible(TRUE);
			break;
			
		case enMenuBarOnly:
			menubar->setVisible(TRUE);
			toolBar->setVisible(FALSE);
			break;
	}
	
	resizeEvent(0);
}
void dart::vSetToolMenuBarState(enToolMenuBarState state) {
	iToolMenuBarState=state;
	
	switch(iToolMenuBarState) {
		case enBoth:
			actionMenu_Bar->trigger();
			actionToolbar->trigger(); break;
		case enToolBarOnly:
			actionToolbar->trigger(); break;
		case enMenuBarOnly:
			actionMenu_Bar->trigger(); break;
	}
}

bool dart::bCanLoseScore() {
	return ( (iPlaceCount>1 || iCurrentPlayer!=0) && iGameMode!=enTraining );
}

void dart::vShowPreferences() {
	preferences dialog(this,myIO);
	dialog.exec();
}
