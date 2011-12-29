/*
QeoDart
Copyright (C) 2011 Markus Brenneis

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QLibraryInfo>

#include "dart.hpp"

bool bLogging=false;

void MyOutputHandler(QtMsgType type, const char *msg);

int main(int argc, char* argv[]) {
	
#ifndef Q_OS_WINCE
	if(argc==2 && (static_cast<string>(argv[1])=="-h" || static_cast<string>(argv[1])=="--help") ) {
		qDebug() << "Options:";
		qDebug() << "  -h,  --help          prints this text; only works when it is the only command-line argument";
		qDebug() << "       --banner        shows two QeoDart banners (cannot be visible at the same time, visibility depends on window geometry)";
		qDebug() << "  -dp, --debug-places  when activating \"find place\", the rectangles describing the area are drawn around the places";
		qDebug() << "  -l,  --log           creates a log file; must be the first option";
		return 0;
	}
#endif
	
	qInstallMsgHandler(MyOutputHandler);
	
	if(argc>=2 && (static_cast<string>(argv[1])=="-l" || static_cast<string>(argv[1])=="--log") ) {
		ofstream logfile;
		logfile.open((QCoreApplication::applicationDirPath().toStdString()+"qeodart.log").c_str(), ios::app);
		if(logfile) {
			bLogging=true;
			qDebug() << "##### Logging - Application started #####";
			qDebug() << "[i] logfile" << QCoreApplication::applicationDirPath()+"qeodart.log";
		} else {
			qDebug() << "[E] logfile" << QCoreApplication::applicationDirPath()+"qeodart.log" << "could not be opened";
		}
		logfile.close();
	}
	
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8")); 
	
	QApplication a(argc, argv);
	a.setAutoSipEnabled(true);
	
//	TODO we should get dart::vRetranslate to work
	QString configPath=QCoreApplication::applicationDirPath()+"/QeoDart.conf";
	if( !QFile::exists(configPath) ) {
#ifdef Q_OS_UNIX
		configPath=QDir::homePath()+"/.config/QeoDart/QeoDart.conf";
#endif
#ifdef Q_OS_WIN32
		configPath=QString(getenv("APPDATA"))+"/QeoDart/QeoDart.conf";
#endif
	}
	QSettings *settings = new QSettings(configPath, QSettings::IniFormat);
	QString lang=settings->value("qsLanguage","default").toString();
	if(lang=="default") lang=QLocale::system().name();
	
	qDebug() << "[i] language is" << lang << ", we are running in" << QCoreApplication::applicationDirPath() << ", and Qt translations are installed in" << QLibraryInfo::location(QLibraryInfo::TranslationsPath);
	
	// Qt translations for default dialogs
	QTranslator qtTranslator;
	qtTranslator.load(QString("%1/qt_%2").arg(QLibraryInfo::location(QLibraryInfo::TranslationsPath)).arg(lang));
	a.installTranslator(&qtTranslator);
	// and when we are running on Windows, we probably do not have Qt installed, so we use our copy of the default translations
	QTranslator qtOwnTranslator;
	qtOwnTranslator.load(QString(QCoreApplication::applicationDirPath()+"/lang/qt_%1").arg(lang));
	a.installTranslator(&qtOwnTranslator);
	
	//English fallback
	QTranslator translatore;
	translatore.load(QString(QCoreApplication::applicationDirPath()+"/lang/en"));
	a.installTranslator(&translatore);
	
	QTranslator translator;
	translator.load(QString(QCoreApplication::applicationDirPath()+"/lang/%1").arg(lang));
	a.installTranslator(&translator);
	
	dart w;
	w.show();
	return a.exec();
}

void MyOutputHandler(QtMsgType type, const char *msg) {
	cout << msg << endl;
	if(bLogging) {
		ofstream logfile;
		logfile.open("qeodart.log", ios::app);
// 		logfile.open((QCoreApplication::applicationDirPath().toStdString()+"qeodart.log").c_str(), ios::app);
		if(logfile){
			switch(type){
				case QtDebugMsg:
					logfile << QTime::currentTime().toString().toAscii().data() << " Debug: " << msg << "\n";
					break;
				case QtCriticalMsg:
					logfile << QTime::currentTime().toString().toAscii().data() << " Critical: " << msg << "\n";
					break;
				case QtWarningMsg:
					logfile << QTime::currentTime().toString().toAscii().data() << " Warning: " << msg << "\n";
					break;
				case QtFatalMsg:
					logfile << QTime::currentTime().toString().toAscii().data() <<  " Fatal: " << msg << "\n";
					logfile.close();
					abort();
			}
			logfile.close();
		}
	}
}
