/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "dart.hpp"
#include "io.hpp"
#include <QDebug>
#include <QDir>

using namespace std;

io::io(dart *TDart) : myDart(TDart){

}
 
io::~io(){
}

// looks for .qcfx files and adds valid files to qlQcfxFiles;
int io::iFindQcf() {
	myDart->qlQcfxFiles.clear();
	
	QList<QDir> qlQcfDirs;
	qlQcfDirs.append(QDir(QCoreApplication::applicationDirPath ()+"/qcf"));
	qlQcfDirs.append(QDir(QDir::homePath()+"/.config/QeoDart/qcf"));
	#ifdef Q_OS_UNIX
	qlQcfDirs.append(QDir("/usr/share/QeoDart/qcf"));
	#endif
	#ifdef Q_OS_WIN32
	qlQcfDirs.append(QDir(getenv("PROGRAMFILES"));
	#endif
	
	for(int i=0; i<qlQcfDirs.count(); i++) {
		QStringList filters;
		filters << "*.qcfx";
		qlQcfDirs[i].setNameFilters(filters);
		
		qDebug() << "[i] Found" << qlQcfDirs[i].entryList() << "in" << qlQcfDirs[i].absolutePath();
		
		for(int j=0; j<qlQcfDirs[i].entryList().count(); j++){
//			bool namespaceProcessing; QString errorMsg; int errorLine, errorColumn;
//			QDomDocument doc("QeoDartCoordinates");
//			QFile file( QString("%1%2").arg(qcfdir.c_str()).arg(dir.entryList()[i]) );
//			if(!file.open(QIODevice::ReadOnly)) {
//				file.close();
//				qDebug() << "QeoDartCoordinates: Read only";
//			}
			
			QDomDocument doc("coordinates");
			QFile file(qlQcfDirs[i].absolutePath()+"/"+qlQcfDirs[i].entryList()[j]);
			
			if(iCheckQcf(file, doc)==0) {
				qcfFile f;
				f.path=file.fileName().left(file.fileName().length()-5);
				f.mapName=qsGetMapName(doc);
				qDebug() << f.path;
				myDart->qlQcfxFiles << f;
			}
		}
	}
	
	
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

QString io::qsGetMapName(QDomDocument &doc) {
	QDomElement docElem = doc.documentElement();
	
	QDomNode n = docElem.firstChildElement("name");
	if(!n.isNull()) {
		QDomElement e = n.toElement();
		if(!e.isNull()) {
			if(e.tagName()=="name") {
				return e.attribute("default","NONAME"); // TODO lang through setting
			}
		} else {
			qDebug() << "[W] file has broken <name>";
			return "NONAME";
		}
	} else {
		qDebug() << "[W] file has no <name>";
		return "NONAME";
	}
}

int io::iReadQcf(QString mapname) {
	
	QString filename;
	
	//find the file with the given mapname in the list
	for(int i=0; i<myDart->qlQcfxFiles.count(); i++) {
		if(myDart->qlQcfxFiles[i].mapName==mapname) {
			filename=myDart->qlQcfxFiles[i].path+".qcfx";
			myDart->iCurrentQcf=i;
		}
	}
	
	qDebug() << "[i] Reading file" << filename << "with mapName" << mapname;
	
	QDomDocument doc("coordinates");
	QFile file(filename);
	
	if(iCheckQcf(file, doc)!=0) {
		qDebug() << "[E] File" << filename << "is not valid";
		QMessageBox msgBox;
		msgBox.setText(QString(tr("An unexpected error occured while reading %1. QeoDart will be quit.")).arg(filename));
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();
		myDart->close(); // TODO doesn't work
		return -1;
	}
	
	myDart->qlAllPlaces.clear();
	
	QDomElement docElem = doc.documentElement();
	
	QDomNode n = docElem.firstChild();
	while(!n.isNull()) {
		
		QDomElement e = n.toElement(); // try to convert the node to an element
		
		if(!e.isNull()) {
			
			if(e.tagName()=="pxtokm") {
				
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
				
			} else if(e.tagName()!="name") {
				
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
	
	QString path=myDart->qlQcfxFiles[myDart->iCurrentQcf].path;
	
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
//			qDebug() << myDart->qlAllPlaces[i].x << myDart->qlAllPlaces[i].y << myDart->qlAllPlaces[i].dimx << myDart->qlAllPlaces[i].dimy << myDart->qlAllPlaces[i].name;
		}
	}
	
	if(myDart->qlCurrentTypePlaces.count()==0) {
		if(myDart->qsCurrentPlaceType!="everything") {
			qDebug() << "[i] there is no place for placetype" << myDart->qsCurrentPlaceType;
			qDebug() << "    falling back to everything";
			myDart->qsCurrentPlaceType="everything";
			vFillCurrentTypePlaces();
		} else {
			qDebug() << "[E] No place in list";
			myDart->close();
		}
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
