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

// looks for .qcfx files and adds valid files to qlQcfxFiles;
int io::iFindQcf() {
        myDart->qlQcfxFiles.clear();
        
        QList<QDir> qlQcfDirs;
        qlQcfDirs.append(); //get execution path; add relative acf path (if available) + $home/%userprofile% + /usr/%programfiles% [Qt API]
                        
        QDir dir(qcfdir.c_str());
        QStringList filters;
        filters << "*.qcfx";
        dir.setNameFilters(filters);
        
        qDebug() << "Found " << dir.entryList() << " in " << qcfdir.c_str();
        
        
        for(int i=0; i<dir.entryList().count(); i++){
                bool namespaceProcessing; QString errorMsg; int errorLine, errorColumn;
                QDomDocument doc("QeoDartCoordinates");
                QFile file( QString("%1%2").arg(qcfdir.c_str()).arg(dir.entryList()[i]) );
                if(!file.open(QIODevice::ReadOnly)) {
                        file.close();
                        qDebug() << "QeoDartCoordinates: Read only";
                }
                        
	myDart->qlQcfxFiles << "/home/markus/Dokumente/GitHub/QeoDart/qcf/de.qcfx"; //TODO
}

int io::iCheckQcf(QFile &file, QDomDocument &doc) {
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "[W]" << file.fileName() << "is not readable";
		return -1;
	}
	bool namespaceProcessing; QString errorMsg; int errorLine, errorColumn;
	if(!doc.setContent(&file, namespaceProcessing, &errorMsg, &errorLine,  &errorColumn )){
		file.close();
		qDebug() << "[W] Problem reading file" << file.fileName();
		qDebug() << "     Line" << errorLine << "Column" << errorColumn << errorMsg;
		return -2;
	}
	file.close();
	
	QDomElement root = doc.documentElement();
	if(root.tagName()!="coordinates") {
		qDebug() << "[W] " << file.fileName() << "is no .svg-file";
		qDebug() << "     root.tagName is" << root.tagName();
		return -3;
	}
	if(root.attribute("version", "NULL")!="0.2") {
		qDebug() << "[W] " << file.fileName() << "is not qcfx-version 0.2";
		qDebug() << "     version is" << root.attribute("version", "NULL");
		return -4;
	}
	return 0;
}

int io::iReadQcf(QString filename) {
	qDebug() << "[i] Reading file" << filename;
	
	/*for(int i=0;i<2;i++){
		place one;
		one.x=327+50*i;
		one.y=52+20*i;
		one.dimx=10*i;
		one.dimy=10*i;
		one.name="bla very long caption chisfh dsfjslkdf sdfjsdjflsd fsjfklj sdlkjsdfl sjf sdjfksldjf sjf sdfjksdlfj skdlfj sdklfj sklfj skdfj sdkljfdskljf"+i;
		one.placeType="capitalOfLand;land";
		myDart->qlAllPlaces.append(one);
	}*/
	
	
	QDomDocument doc("coordinates");
	QFile file(filename);
	
	if(iCheckQcf(file, doc)!=0) {
		qDebug() << "[E] File" << filename << "is not valid";
		QMessageBox msgBox;
		msgBox.setText(QString(tr("An unexpected error occured while reading %1. QeoDart will be quit.")).arg(filename));
		msgBox.exec();
		myDart->close(); // TODO doesn't work
	}
	
	myDart->qlAllPlaces.clear();
	
	QDomElement docElem = doc.documentElement();
	
	QDomNode n = docElem.firstChild();
	while(!n.isNull()) {
		
		QDomElement e = n.toElement(); // try to convert the node to an element
		
		if(!e.isNull()) {
			
			if(e.tagName()=="name") {
				
			} else if(e.tagName()=="pxtokm") {
				
				myDart->dPxToKm=e.attribute("value","-1").toDouble();
				if(myDart->dPxToKm==-1) {
					qDebug() << "[W] pxtokm is unset, using fallback 1";
					myDart->dPxToKm=1;
				}
				
			} else if(e.tagName()=="author") {
				
			} else if(e.tagName()=="place") {
				
				place newPlace;
				newPlace.x=e.attribute("x","-1").toInt();
				newPlace.y=e.attribute("y","-1").toInt();
				newPlace.dimx=e.attribute("dimx","0").toInt();
				newPlace.dimy=e.attribute("dimy","0").toInt();
				newPlace.name=e.attribute("name","NONAME");
				newPlace.placeType=e.attribute("placetype","everything");
				myDart->qlAllPlaces.append(newPlace);
				
				if(newPlace.name=="NONAME") {
					qDebug() << "[W] place" << myDart->qlAllPlaces.count() << "has no name";
				}
				if(newPlace.placeType=="everything") {
					qDebug() << "[W] place" << myDart->qlAllPlaces.count() << "has no placetype";
				}
				if(newPlace.x==-1 || newPlace.y==-1) {
					qDebug() << "[W] place" << myDart->qlAllPlaces.count() << "has incomplete coordinates";
				}
				
			} else {
				
				qDebug() << "[w] unknown tagName" << e.tagName();
				
			}
			
		} // if(!e.isNull())
		
		n = n.nextSibling();
		
	} // while(!n.isNull())
	
	
	qDebug() << "[i] Read" << myDart->qlAllPlaces.count() << "places";
	
	if(myDart->qlAllPlaces.count()==0) {
		qDebug() << "[E] File" << filename << "contains no <place>";
		QMessageBox msgBox;
		msgBox.setText(QString(tr("The file %1 contains no <place>. QeoDart will be quit.")).arg(filename));
		msgBox.exec();
		myDart->close(); // TODO through return?
	}
	
	qDebug() << "[i] current placetype is" << myDart->qsCurrentPlaceType;
	
	vFillCurrentTypePlaces();
	
	QString path=myDart->qlQcfxFiles[myDart->iCurrentQcf].left(myDart->qlQcfxFiles[myDart->iCurrentQcf].length()-5);
	
	for(int i=0; i<4; i++) {
		QFile file(QString("%1/%2.png").arg(path).arg(myDart->qlImageLayers[i]));
		
		if(!file.open(QIODevice::ReadOnly)) {
			qDebug() << "[I] No" << myDart->qlImageLayers[i] << "found";
			myDart->qlMapLayers[i]->hide(); // TODO disable the show-button in ui
		} else {
			qDebug() << "[i] found" << myDart->qlImageLayers[i];
			myDart->qlMapLayers[i]->show();
		}
		file.close();
	}
	
	return 0;
}

void io::vFillCurrentTypePlaces() {
	int max=myDart->qlAllPlaces.count();
	myDart->qlCurrentTypePlaces.clear();
	
	for(int i=0;i<max;i++) {
		if(myDart->qlAllPlaces[i].placeType.contains(myDart->qsCurrentPlaceType) || myDart->qsCurrentPlaceType=="everything") {
			myDart->qlCurrentTypePlaces.append(&(myDart->qlAllPlaces[i]));
			qDebug() << myDart->qlAllPlaces[i].x << myDart->qlAllPlaces[i].y << myDart->qlAllPlaces[i].dimx << myDart->qlAllPlaces[i].dimy << myDart->qlAllPlaces[i].name;
		}
	}
	
	if(myDart->qlCurrentTypePlaces.count()==0) {
		qDebug() << "[i] there is no place for placetype" << myDart->qsCurrentPlaceType;
		qDebug() << "    falling back to everything";
		myDart->qsCurrentPlaceType="everything";
		vFillCurrentTypePlaces();
	}
}

int io::iReadOsm(QString filename) { //TODO function iValidOsm / iValidQcfx / iValidXml NOTE we should parse rendered svg
	qDebug() << "[i] Reading" << filename;
	
	
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
