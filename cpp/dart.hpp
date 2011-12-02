/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#ifndef DART_HPP
#define DART_HPP

#include "ui_mainWindow.h"
#include <iostream>
#include <math.h>
#include <QDebug>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDialog>
#include <QDir>
#include <QInputDialog>
#include <qlist.h>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QTime>
#include <QTimer>
#include <QToolButton>
#include <QTranslator>
#include <qstring.h>
#include <QUrl>
#include <time.h>

#ifdef Q_OS_WINCE 
#include <winbase.h>
#endif

#include "io.hpp"
#include "highscores.hpp"
#include "about.hpp"
#include "results.hpp"
#include "preferences.hpp"

struct place {
	int x, y, dimx, dimy;
	QString name, placeType;
};

struct scoreHistory {
	int x, y, score;
	double diffPx, diffPxArea, diffKm, mark;
};

struct totalScore {
	double mark, score;
};

struct qcfCopyright {
	QString file, background, elevations, rivers, borders;
};

struct qcfFile {
	QString mapName, mapShortName, id, path; // path: e.g. qcf/de
	int pxtokm;
	qcfCopyright copyright;
};

struct highScoreEntry {
	QString name;
	double score; // TODO important settings
};

enum enGameModes {
	enFind=3,
	enTraining=0,
	enLocal=1,
	enNetwork=2,
};

enum enAskForModes {
	enPositions=0,
	enNames=1,
};

enum enToolMenuBarState {
	enBoth=0,
	enToolBarOnly=1,
	enMenuBarOnly=2,
};
 
class dart : public QMainWindow, public Ui::MainWindow {
Q_OBJECT

	public:
		dart (QMainWindow *parent = 0);
		~dart();
		
		QList<qcfFile> qlQcfxFiles;
		QList<place> qlAllPlaces; //contains all places of the current map
		QList<place*> qlCurrentTypePlaces; //contains pointers pointing to the places in qlAllPlaces which fit the current placetype
		QList<place*> qlPlacesHistory;
		QList<QList<scoreHistory> > qlScoreHistory; // [player][round].scores
		QList<totalScore> qlTotalScores;
		QList<QLabel*> qlPointLabels;
		QList<QLabel*> qlDebugPlaceLabels;
		QList<QString> qlLayersNames;
		QList<QString> qlPlaceTypesNames;
		QList<QLabel*> qlMapLayers;
		QList<highScoreEntry> qlHighScores;
		QList<QAction*> qlPlacesSubsetsActions;
		
		QLabel *lblMouseClickOverlay;
		
		QActionGroup *agLayers, *agPlaceTypes;
		
		enGameModes iGameMode;
		enAskForModes iAskForMode;
		enToolMenuBarState iToolMenuBarState;
		
		bool bAgainstTime, bResetCursor, bShortenToolbarText, bUseOurCursor, bAutoNewGame, bAutoShowHighScores, bPlacesSubsetActive;
		
		double dZoomFactor, dPxToKm;
		
		int iPaddingTop; // px between toolbar and map
		int iMarginTop; // px between window decoration and toolbar
		int iNumberOfPlayers, iMaxPlaceCount, iPlaceCount, iCurrentQcf, iMaxTime, iLettersPerSecond;
		int iScoreAreaMode; // 0=point only; 1=25%; 2=100%
		int iPenalty; // 0=no; 1=severe; 1=spelling
		int iDelayNextCircle, iDelayNextPlayer, iDelayNextPlace, iDelayNextPlaceTraining;
		
		QString qsCurrentPlaceType, qsLanguage, qsPreferedQcfLanguage;
		
		void vMouseClickEvent(int x, int y);
		void mySleep(int ms);
		void vShowCurrentPlace();
		void vSetPlaceType(QString placetype);
		void vRepaintCommonLabels();
		void vRetranslate();
		void vCreatePlacesSubsetsActions();
		void vUpdatePlacesSubsetActive();
		
		bool bCanLoseScore();

	private:
		
		io *myIO;
		
		bool bAcceptingClickEvent, bAcceptingResizeEvent, bGaveHint;
		
		int iCurrentPlayer, iTimerElapsed;
		
		place *pTrainingPlaceNumber;
		
		QList<QList<QLabel*> > qlCircleLabels; //contains all circles (incl. points) for each user
// 		QList<QLabel*> qlPlayerLabel;
		QList<QList<QLabel*> > qlPlayerLabels;
		QList<QColor> qlColorsOfPlayers;
		QList<QString> qlComments;
		
		QTimer *timer, *resizeTimer;
		
		QActionGroup *agGameMode, *agAskForMode, *agMap;
		
		QToolButton *btMap, *btGameMode, *btAskForMode, *btPlaceType, *btView, *btApplication;
		
		QAction *actionBtApplication, *actionBtAskForMode;
		
		QMenu *menuSubset;
		
		void closeEvent(QCloseEvent *event);
		void resizeEvent(QResizeEvent *event);
		void vRepaintPlayerLabels();
		void vRepaintMap();
		void vDrawCircle(int x, int y, int r, int player);
		void vDrawDistanceCircles(int n);
		void vDrawPoint(int x, int y, QList<QLabel*> &list, QColor color, QString name="");
		void vDrawPoint(int x, int y, QList<QLabel*> &list, QString name="", QColor color=QColor(249,199,65));
		void vDrawClickPositions(int n);
		void vDrawDebugPlace(int i);
		void vRemoveAllCircles();
		void vRemoveAllCommonPoints();
		void vSetNumberOfPlayers(int player);
		void vSetGameMode(enGameModes mode);
		void vSetAskForMode(enAskForModes mode);
		void vSetAgainstTime(bool enable);
		void vNextRound();
		void vResetForNewGame();
		void vShowTotalScores();
		void vShowScores();
		void vShowComment();
		void vResetScoreLabels();
		void vShowResultWindows();
		void vSetToolMenuBarState(enToolMenuBarState state);
		void vUpdateActionsIsCheckedStates();
		void vFindPlaceAround(int x, int y);
		void vShowAllPlaces();
		void vOpenLinkInBrowser(QUrl link);
		
		double dGetDistanceInPxBetween(int a, int b, int x, int y);
		double dGetDistanceInPx(int a, int b, int n);
		double dGetMarkFromDistance(double distance);
		double dGetMarkFromScore(double score);
		double dGetScore(double mark);
		double dGetDistanceInKm(double px);
		double dGetAverageMarkOfPlayer(int player);
		double dGetAverageScoreOfPlayer(int player);
		double dGetZoomed(int x);
		
		int iGetWindowSize();
		int iGetUnzoomed(double x);
		int iGetFontSize();
		int iGetPaddingTop();
		int iFindInputInList(double &f);
		
		bool bNewGameIsSafe();
		
		QColor qcGetColorOfPlayer(int n);
		
		QString qsSimplifyString(QString str, int l);
		
	private slots:
		void vClose();
		void vResize(double dNewZoomFactor=1);
		void vSetGameMode();
		void vSetAskForMode();
		void vReturnPressedEvent();
		void vTextEditedEvent();
		void vTimeout();
		void vSetPlaceType();
		void vSetToolMenuBarState();
		void vSetNumberOfPlayers();
		void vSetAgainstTime();
		void vShowPreferences();
		void vShowHighScores();
		void vShowAboutWindow();
		void vToolbarOverflow();
		void vToggleMapLayer();
		void vGiveHint();
		void vAddMap();
		void vReportBug();
		void vPlacesSubsetClicked();
		
	public slots:
		void vReadQcf();
		void vNewGame();
};

#include "myLabels.hpp"
#endif //DART_HPP 
