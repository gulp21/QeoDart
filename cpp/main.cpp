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

#include "dart.hpp"

int main(int argc, char* argv[]) {
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8")); 
	
	//QLocale::system().name().left(2)
	
        QApplication a(argc, argv);
	a.setAutoSipEnabled(true);
        dart w;
        w.show();
        return a.exec();
}
