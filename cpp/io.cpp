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
	
	for(int i=0;i<2;i++){
		place one;
		one.x=50+50*i;
		one.y=50+20*i;
		one.dimx=10;
		one.dimy=10;
		one.name="bla";
		one.placeType="capitalOfLand;land";
		myDart->qlAllPlaces.append(one);
	}
	
	
	qDebug() << "[i] Read" << myDart->qlAllPlaces.count() << "places";
	qDebug() << "[i] current placetype is" << myDart->qsCurrentPlaceType;
	
	int max=myDart->qlAllPlaces.count();
	myDart->qlCurrentTypePlaces.clear();
	for(int i=0;i<max;i++){
		if(myDart->qlAllPlaces[i].placeType.contains(myDart->qsCurrentPlaceType)){
			myDart->qlCurrentTypePlaces.append(&(myDart->qlAllPlaces[i]));
			qDebug() << myDart->qlAllPlaces[i].x << myDart->qlAllPlaces[i].y << myDart->qlAllPlaces[i].dimx << myDart->qlAllPlaces[i].dimy << myDart->qlAllPlaces[i].name;
		}
	}
	
	return 0;
}
