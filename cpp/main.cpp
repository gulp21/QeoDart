/*
QeoDart
Copyright (C) 2011 Markus Brenneis

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <stdlib.h>
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QLibraryInfo>

#include "dart.hpp"

int main(int argc, char* argv[]) {
	
#ifndef Q_OS_WINCE
	if(argc==2 && ((string)argv[1]=="-h" || (string)argv[1]=="--help") ) {
		qDebug() << "Options:";
		qDebug() << "  -h, --help:          prints this text (only works when it is the only command-line argument)";
		qDebug() << "  --banner:            shows two QeoDart banners (cannot be visible at the same time, visibility depends on window geometry)";
		qDebug() << "  -dp, --debug-places: when activating \"find place\", the rectangles describing the area are drawn around the places";
		return 0;
	}
#endif

	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8")); 
	
        QApplication a(argc, argv);
	a.setAutoSipEnabled(true);
	
	QString lang=QLocale::system().name();
	
	qDebug() << "[i] system language is" << lang << ", we are running in" << QCoreApplication::applicationDirPath() << ", and Qt translations are installed in" << QLibraryInfo::location(QLibraryInfo::TranslationsPath);
	
	// Qt translations for default dialogs
	QTranslator qtTranslator;
	qtTranslator.load(QString("%1/qt_%2").arg(QLibraryInfo::location(QLibraryInfo::TranslationsPath)).arg(lang));
	a.installTranslator(&qtTranslator);
	// and when we are running on Windows, we probably do not have Qt installed, so we use our copy of the default translations
	QTranslator qtOwnTranslator;
	qtOwnTranslator.load(QString(QCoreApplication::applicationDirPath()+"/lang/qt_%1").arg(lang));
	a.installTranslator(&qtOwnTranslator);
	
	QTranslator translator;
	translator.load(QString(QCoreApplication::applicationDirPath()+"/lang/%1").arg(lang));
	a.installTranslator(&translator);
	
        dart w;
        w.show();
        return a.exec();
}
