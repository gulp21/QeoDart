/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include <qstring.h>
#include <qlist.h>
#include <QLabel>
#include <QMessageBox>
#include "ui_mainWindow.h"

#ifndef DART_HPP
#define DART_HPP

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

struct qcfFile {
	QString mapName, path; // path: e.g. qcf/de
};

enum enGameModes {
	enTraining=0,
	enLocal=1,
	enNetwork=2,
};

enum enAskForModes {
	enPositions=0,
	enNames=1,
};
 
class dart : public QMainWindow, public Ui::MainWindow {
Q_OBJECT

	public:
		dart (QMainWindow *parent = 0);
		~dart();
		
		QList<qcfFile> qlQcfxFiles;
		QList<place> qlAllPlaces; //contains all places of the current map
		QList<place*> qlCurrentTypePlaces; //contains pointers pointing on the places in qlAllPlaces which fit the current placetype
		QList<int> qlPlacesHistory;
		QList<QList<scoreHistory> > qlScoreHistory; // [player][round].scores
		QList<totalScore> qlTotalScores;
		QList<QLabel*> qlPointLabels;
		QList<QString> qlImageLayers;
		QList<QLabel*> qlMapLayers;

		double dZoomFactor, dPxToKm;
		
		int iPaddingTop; // px between toolbar and map
		int iMarginTop; // px between window decoration and toolbar
		int iNumberOfPlayers, iMaxPlaceCount, iCurrentQcf;
		int iScoreAreaMode;
		
		QString qsCurrentPlaceType;
		
		void vMouseClickEvent(int x, int y);
		void mySleep(int ms);
		void vShowCurrentPlace();

	private:
		
		bool bAcceptingClickEvent;
		
		enGameModes iGameMode;
		enAskForModes iAskForMode;
		
		int iCurrentPlayer, iPlaceCount, iTrainingPlaceNumber;
		int iDelayNextCircle, iDelayBeforeShowingMark, iDelayBeforeNextPlayer, iDelayBeforeNextPlace, iDelayBeforeNextPlaceTraining;
		
		QList<QList<QLabel*> > qlCircleLabels; //contains all circles (incl. points) for each user
// 		QList<QLabel*> qlPlayerLabel;
		QList<QList<QLabel*> > qlPlayerLabels;
		QList<QColor> qlColorsOfPlayers;
		QList<QString> qlComments;
		
		QLabel *lblMouseClickOverlay;
		
		void closeEvent(QCloseEvent *event);
		void resizeEvent(QResizeEvent *event);
		void vRepaintCommonLabels();
		void vRepaintPlayerLabels();
		void vRepaintMap();
		void vDrawCircle(int x, int y,  int r, int player);
		void vDrawDistanceCircles(int n, int count=0);
		void vDrawPoint(int x, int y, QList<QLabel*> &list, QColor color, QString name="");
		void vDrawPoint(int x, int y, QList<QLabel*> &list, QString name="", QColor color=QColor(249,199,65));
		void vDrawClickPositions(int n);
		void vRemoveAllCircles();
		void vRemoveAllCommonPoints();
		void vSetNumberOfPlayers(int player);
		void vSetGameMode(enGameModes mode);
		void vSetAskForMode(enAskForModes mode);
		void vNextRound();
		void vResetForNewGame();
		void vShowTotalScores();
		void vShowScores();
                void vShowComment();
		void vResetScoreLabels();
		void vShowResultWindows();
		
		double dGetDistanceInPxBetween(int a, int b, int x, int y);
		double dGetDistanceInPx(int a, int b, int n);
		double dGetMarkFromDistance(double distance);
		double dGetMarkFromScore(double score);
		double dGetScore(double mark);
		double dGetDistanceInKm(double px);
		double dGetAverageMarkOfPlayer(int player);
		double dGetAverageScoreOfPlayer(int player);
		
		int iGetWindowSize();
		int iGetUnzoomed(double x);
		int iGetFontSize();
		int iFindInputInList(double &f);
		
		QColor qcGetColorOfPlayer(int n);
		
		QString qsSimplifyString(QString str, int l);
		
	private slots:
		void vClose();
		void vShowAllPlaces();
		void vNewGame();
		void vResize(double dNewZoomFactor=1);
		void vSetGameMode();
		void vSetAskForMode();
		void vReturnPressedEvent();
};

#include "myLabels.hpp"

#endif //DART_HPP 
