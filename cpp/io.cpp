/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "dart.hpp"
#include "io.hpp"
#include <QDebug>
#include <QDomDocument>
#include <QFile>

using namespace std;
 
io::io(dart *TDart) : myDart(TDart){

}
 
io::~io(){
}

int io::iReadQcf(QString filename) {
	qDebug() << "[i] Reading file" << filename;
	
	for(int i=0;i<2;i++){
		place one;
		one.x=327+50*i;
		one.y=52+20*i;
		one.dimx=10*i;
		one.dimy=10*i;
		one.name="bla very long caption chisfh dsfjslkdf sdfjsdjflsd fsjfklj sdlkjsdfl sjf sdjfksldjf sjf sdfjksdlfj skdlfj sdklfj sklfj skdfj sdkljfdskljf"+i;
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

int io::iReadOsm(QString filename) { //TODO function iValidOsm / iValidQcfx / iValidXml NOTE we should parse rendered svg
	qDebug() << "[i] Reading" << filename;
// 	QDomDocument doc("svg");
// 	QFile file(filename);
// 	if (!file.open(QIODevice::ReadOnly)) {
// 		qDebug() << "[E] iReadOsm:" << filename << "is not readable";
// 		return -1;
// 	}
// 	bool namespaceProcessing; QString errorMsg; int errorLine, errorColumn;
// 	if(!doc.setContent(&file, namespaceProcessing, &errorMsg, &errorLine,  &errorColumn )){
// 		file.close();
// 		qDebug() << "[E] iReadOsm: Problem reading file" << filename;
// 		qDebug() << "     Line" << errorLine << "Column" << errorColumn << errorMsg;
// 		return -2;
// 	}
// 	file.close();
// 	
// 	QDomElement root = doc.documentElement();
// 	if(root.tagName()!="svg") {
// 		qDebug() << "[E] iReadOsm:" << filename << "is no .svg-file";
// 		qDebug() << "     root.tagName is" << root.tagName();
// 		return -3;
// 	}
// 	
// 	QDomElement docElem = doc.documentElement();
// 	
// 	QDomNode n = docElem.firstChildElement("text"); // places are nodes
// 	while(!n.isNull()) {
// 			qDebug()<<"fff";
// 		QDomElement e = n.toElement(); // try to convert the node to an element.
// 		if(!e.isNull()) {
// // 			if(e.hasChildNodes()) { // places have child nodes
// // 				QDomNode n = e.firstChildElement("tag");
// // 				
// // 				while(!n.isNull()) { // loop through the tags
// // 					QDomElement e = n.toElement();
// // 					if(!e.isNull()) {
// // 						qDebug() << e.tagName();
// // 						
// // 						if(e.attribute("k","-1")=="name"){
// // 							qDebug() << e.attribute("v","NAME?");
// // 						} else if(e.attribute("k","-1")=="place"){
// // // 							QString place << e.attribute("v","PLACE?");
// // 							
// // 						}
// // 						
// // 					}
// // 					n = n.nextSiblingElement("tag");
// // 				}
// // 				
// // 				
// // 				
// // 				
// // 			}
// 			qDebug() << e.tagName();
// 		}
// 		n = n.nextSiblingElement("text");
// 	}
	
	
	// parsing line-by-line is probably easier than xml parsing
	
	//open input file
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { cout << "couldn't open" << filename.toStdString() << "\n"; return(2);}
	QTextStream in(&file);
	
	// loop through all lines
	while(!in.atEnd()) {
		QString line = in.readLine();
		//if there is a place label
		if(line.contains("<text")){
// 			node=line;
			
			QStringList list;
			list = line.split(line.contains("\"") ? "\"" : "\'"); // support ' as well as "
			for(int i=0, max=list.count(); i<max; i++){
				if(list[i].contains("x=")){
					qDebug() << "x" << list[++i];
				} else if(list[i].contains("y=")){
					qDebug() << "y" << list[++i];
				} else if(list[i].contains("class=")){
					int n=list[++i].indexOf("-");
					qDebug() << "placetype" << list[i].left(n);
				} else if(list[i].contains("/text")){
					int n=list[i].indexOf("<");
					qDebug() << "name" << list[i].mid(1,n-1); //remove > and </text>
				}
			} //perl ~/Dokumente/OSM/osmarender/orp/orp.pl -r osm-map-features-dart.xml test.osm
		
		}
	}
	
	
	return 0;
}
