/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "dart.hpp"
#include "io.hpp"
#include <QDebug>

using namespace std;
 
io::io(dart *TDart) : myDart(TDart){
	
}
 
io::~io(){
}

int io::iReadQcf(QString filename){
	qDebug() << "[i] Reading file" << filename;
	
	place one;
	one.x=50;
	one.y=50;
	one.dimx=10;
	one.dimy=10;
	one.name="bla";
	myDart->qlAllPlaces.append(one);
	
	place entry;
	qDebug() << "[i] Read" << myDart->qlAllPlaces.count() << "places";
	foreach(entry, myDart->qlAllPlaces){
		qDebug() << entry.x << entry.y << entry.dimx << entry.dimy << entry.name;
	}
	
	return 0;
}
