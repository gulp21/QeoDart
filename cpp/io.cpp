/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#include "dart.hpp"
#include "io.hpp"

using namespace std;

io::io(dart *TDart) : myDart(TDart) {
	bPortable=false;
	bDeterminedPortable=false;
	
	qslPlaceTypesNamesRegExps << "state" << "capitalOfState" << "country" << "capitalOfCountry" << "county" << "city" << "town";
}
 
io::~io() {
	delete settings;
}

// looks for .qcfx files and adds valid files to qlQcfxFiles;
int io::iFindQcf() {
	myDart->qlQcfxFiles.clear();
	
	QList<QDir> qlQcfDirs;
	qlQcfDirs.append(QDir(QCoreApplication::applicationDirPath()+"/qcf"));
#ifdef Q_OS_UNIX
	qlQcfDirs.append(QDir("/usr/share/QeoDart/qcf"));
	qlQcfDirs.append(QDir(QDir::homePath()+"/.config/QeoDart/qcf"));
#endif
#ifdef Q_OS_WIN32
	qlQcfDirs.append(QDir(getenv("PROGRAMFILES")));
	qlQcfDirs.append(QDir(QString(getenv("APPDATA"))+"/QeoDart/qcf"));
#endif
	
	for(int i=0; i<qlQcfDirs.count(); i++) {
		QStringList filters;
		filters << "*.qcfx";
		qlQcfDirs[i].setNameFilters(filters);
		
		qDebug() << "[i] Found" << qlQcfDirs[i].entryList() << "in" << qlQcfDirs[i].absolutePath();
		
		for(int j=0; j<qlQcfDirs[i].entryList().count(); j++) {
			QDomDocument doc("qcfx");
			QFile file(qlQcfDirs[i].absolutePath()+"/"+qlQcfDirs[i].entryList()[j]);
			
			if(iCheckQcf(file, doc)==0) {
				qcfFile f;
				f.path=file.fileName().left(file.fileName().length()-5);
				vGetMetaData(doc, f);
				qDebug() << f.path;
				
				QString suffix="";
				int c=-1;
				for(int i=2; qsGetQcfFilePath(f.mapName+suffix, c)!="NULL"; i++) {
					//: this string is appended to the name of the map when there is already a map with the same name; %1 is a number
					suffix=tr(" #%1").arg(i);
					qDebug() << "[w] there is already a map with the mapName" << f.mapName << ", trying" << f.mapName+suffix;
				}
				f.mapName+=suffix;
				
				int i=0;
				for(; i<myDart->qlQcfxFiles.count(); i++) {
					if(myDart->qlQcfxFiles[i].id==f.id) {
						qDebug() << "[W] there is already a map with the id" << f.id << "-> won't add it";
						i=myDart->qlQcfxFiles.count()+1;
					}
				}
				
				if(i==myDart->qlQcfxFiles.count()) vInsertQcfxFile(f);
			} // if(iCheckQcf)
		} // for(lQcfDirs[i].entryList())
	} // for(qlQcfDirs)
	
	if(myDart->qlQcfxFiles.count()==0) {
		qcfFile f;
		f.path="NULL";
		f.mapName="dummyfile";
		myDart->qlQcfxFiles.append(f);
		qDebug() << "[I] added dummyfile";
		return 0;
	}
	
	return myDart->qlQcfxFiles.count();
}

// adds the given qcfFile to qlQcfxFiles, alphabetically ordered
void io::vInsertQcfxFile(qcfFile &f) {
	int i=0;
	while(i<myDart->qlQcfxFiles.count()
	      && QString::localeAwareCompare(f.mapName, myDart->qlQcfxFiles[i].mapName) > 0) {
		i++;
	}
	myDart->qlQcfxFiles.insert(i, f);
}

int io::iCheckQcf(QFile &file, QDomDocument &doc) {
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "[W]" << file.fileName() << "is not readable";
		return -1;
	}
	bool namespaceProcessing=false; QString errorMsg; int errorLine, errorColumn;
	if(!doc.setContent(&file, namespaceProcessing, &errorMsg, &errorLine, &errorColumn )){
		file.close();
		qDebug() << "[W] Problem reading file" << file.fileName();
		qDebug() << "     Line" << errorLine << "Column" << errorColumn << errorMsg;
		return -2;
	}
	file.close();
	
	QDomElement root = doc.documentElement();
	if(root.tagName()!="qcfx") {
		qDebug() << "[W] " << file.fileName() << "is no qcfx-file";
		qDebug() << "     root.tagName is" << root.tagName();
		return -3;
	}
	if(root.attribute("version", "NULL").left(3)!="0.2") {
		qDebug() << "[W] " << file.fileName() << "is not qcfx-version 0.2";
		qDebug() << "     version is" << root.attribute("version", "NULL");
		return -4;
	}
	return 0;
}

QStringList io::qslGetPreferedQcfLanguage() {
	QString l=myDart->qsPreferedQcfLanguage;
	return l.replace("ui",myDart->qsLanguage=="default" ? QLocale::system().name().left(2) : myDart->qsLanguage).split(",");
}

void io::vGetMetaData(QDomDocument &doc, qcfFile &file) {
	QStringList qslPreferedQcfLanguage=qslGetPreferedQcfLanguage();
	
	QDomElement docElem = doc.documentElement();
	
	QDomNode n = docElem.firstChildElement("name");
	if(!n.isNull()) {
		QDomElement e = n.toElement();
		if(!e.isNull()) {
			QString n="NONAME";
			for(int i=0; i<qslPreferedQcfLanguage.count() && n=="NONAME"; i++) {
				n=e.attribute(qslPreferedQcfLanguage[i],"NONAME");
			}
			file.mapName=n;
			file.mapShortName=e.attribute("short","NOSHORTNAME");
			if(file.mapShortName=="NOSHORTNAME") file.mapShortName=file.mapName.left(2);
			file.id=e.attribute("id","NOID_"+file.mapShortName.toUpper());
			if(file.id.startsWith("NOID_")) qDebug() << "[W] map with mapName" << file.mapName << "does not have an id, using" << file.id;
		} else {
			qDebug() << "[W] file has broken <name>";
			file.mapName="NONAME";
		}
	} else {
		qDebug() << "[W] file has no <name>";
		file.mapName="NONAME";
	}
	
	n = docElem.firstChildElement("copyright");
	if(!n.isNull()) {
		QDomElement e = n.toElement();
		if(!e.isNull()) {
			file.copyright.file=e.attribute("file","");
			file.copyright.background=e.attribute("background","");
			file.copyright.borders=e.attribute("borders","");
			file.copyright.elevations=e.attribute("elevations","");
			file.copyright.rivers=e.attribute("rivers","");
		} else {
			qDebug() << "[W] file has broken <copyright>";
		}
	} else {
		qDebug() << "[I] file has no <copyright>";
	}
}

// returns the path of the qcfx-file with the given mapname; returns "NULL" when the file does not exist
// sets index to the index of the qcfx-file in qlQcfxFiles
QString io::qsGetQcfFilePath(QString mapname, int &index) {
	for(int i=0; i<myDart->qlQcfxFiles.count(); i++) {
		if(myDart->qlQcfxFiles[i].mapName==mapname) {
			index=i;
			return myDart->qlQcfxFiles[i].path+".qcfx";
		}
	}
	
	return "NULL";
}

int io::iReadQcf(QString mapname) {
	
	if(mapname=="dummyfile") {
		place newPlace;
		newPlace.x=0;
		newPlace.y=0;
		newPlace.dimx=0;
		newPlace.dimy=0;
		newPlace.name="DUMMYFILE";
		newPlace.placeType="everything";
		myDart->qlAllPlaces.append(newPlace);
		myDart->iCurrentQcf=0;
		
		vFillCurrentTypePlaces();
		
		return 0;
	}
	
	QString filename;
	
	// find the file with the given mapname and set iCurrentQcf to its index
	filename=qsGetQcfFilePath(mapname, myDart->iCurrentQcf);
	
	qDebug() << "[i] Reading file" << filename << "with mapName" << mapname;
	
	QDomDocument doc("qcfx");
	QFile file(filename);
	
	if(iCheckQcf(file, doc)!=0) {
		qDebug() << "[E] File" << filename << "is not valid";
		QMessageBox msgBox;
		msgBox.setText(QString(tr("An unexpected error occurred while reading %1.\nQeoDart will be quit.")).arg(filename));
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();
		return -1;
	}
	
	myDart->qlAllPlaces.clear();
	
	QList<int> qlPlaceTypeCount; // TODO is there a more flexible way?
	qlPlaceTypeCount << 0 << 0 << 0 << 0 << 0 << 0 << 0;
	
	myDart->dPxToKm=-1;
	
	QStringList qslPreferedQcfLanguage=qslGetPreferedQcfLanguage();
	
	QDomElement docElem = doc.documentElement();
	
	QDomNode n = docElem.firstChild();
	while(!n.isNull()) {
		
		QDomElement e = n.toElement(); // try to convert the node to an element
		
		if(!e.isNull()) {
			
			if(e.tagName()=="pxtokm") {
				
				myDart->dPxToKm=e.attribute("value","-1").toDouble();
					
			} else if(e.tagName()=="place") {
				
				place newPlace;
				newPlace.x=e.attribute("x","-1").toInt();
				newPlace.y=e.attribute("y","-1").toInt();
				newPlace.dimx=e.attribute("dimx","0").toInt();
				newPlace.dimy=e.attribute("dimy","0").toInt();
				newPlace.name="NONAME";
				for(int i=0; i<qslPreferedQcfLanguage.count() && newPlace.name=="NONAME"; i++) {
					if(qslPreferedQcfLanguage[i]=="default") {
						newPlace.name=e.attribute("name","NONAME");
					} else {
						newPlace.name=e.attribute("name_"+qslPreferedQcfLanguage[i],"NONAME");
					}
				}
				newPlace.placeType=e.attribute("placetype","");
				myDart->qlAllPlaces.append(newPlace);
				
				if(newPlace.name=="NONAME") {
					qDebug() << "[W] place" << myDart->qlAllPlaces.count() << "has no name";
				}
				if(newPlace.placeType=="") {
					qDebug() << "[W] place" << myDart->qlAllPlaces.count() << "has no placetype";
				}
				if(newPlace.x==-1 || newPlace.y==-1) {
					qDebug() << "[W] place" << myDart->qlAllPlaces.count() << "has incomplete coordinates";
				}
				if(newPlace.x>600 || newPlace.y>600 || newPlace.x<-1 || newPlace.y<-1) {
					qDebug() << "[W] place" << myDart->qlAllPlaces.count() << "has at least one coordinate out of range";
				}
				
				for(int i=0; i<qslPlaceTypesNamesRegExps.count(); i++) {
					if(newPlace.placeType.contains(qslPlaceTypesNamesRegExps[i])) qlPlaceTypeCount[i]++;
				}
				
			} else if(e.tagName()!="name" && e.tagName()!="copyright") {
				
				qDebug() << "[w] unknown tagName" << e.tagName();
				
			}
			
		} // if(!e.isNull())
		
		n = n.nextSibling();
		
	} // while(!n.isNull())
	
	if(myDart->dPxToKm==-1) {
		qDebug() << "[W] pxtokm is unset";
	}
	
	qDebug() << "[i] Read" << myDart->qlAllPlaces.count() << "places";
	
	if(myDart->qlAllPlaces.count()==0) {
		qDebug() << "[E] File" << filename << "contains no <place>";
		QMessageBox msgBox;
		msgBox.setText(QString(tr("The file %1 contains no <place>.\nQeoDart will be quit.")).arg(filename));
		msgBox.exec();
		return -1;
	}
	
	for(int i=0; i<myDart->qlPlaceTypesNames.count(); i++) {
		myDart->agPlaceTypes->actions()[i]->setVisible(qlPlaceTypeCount[i]>0);
		myDart->agPlaceTypes->actions()[i]->setText(QString(tr("%1 (%2)")
		                                                    .arg(myDart->qlPlaceTypesNames[i])
		                                                    .arg(qlPlaceTypeCount[i])));
	}
	
	qDebug() << "[i] current placetype is" << myDart->qsCurrentPlaceType;
	
	myDart->bPlacesSubsetActive=false;
	vFillCurrentTypePlaces();
	
	QString path=myDart->qlQcfxFiles[myDart->iCurrentQcf].path;
	
	for(int i=0; i<4; i++) {
		QFile file(QString("%1/%2.png").arg(path).arg(myDart->qlLayersNames[i]));
		
		if(!file.open(QIODevice::ReadOnly)) {
			qDebug() << "[I] No" << myDart->qlLayersNames[i] << "found";
			if(i>0) myDart->agLayers->actions()[i-1]->setVisible(false);
			myDart->qlMapLayers[i]->hide();
		} else {
			qDebug() << "[i] found" << myDart->qlLayersNames[i];
			if(i>0) myDart->agLayers->actions()[i-1]->setVisible(true);
			if(i==0) myDart->qlMapLayers[i]->show();
		}
		file.close();
	}
	
	return 0;
}

void io::vFillCurrentTypePlaces() {
	int max=myDart->qlAllPlaces.count();
	myDart->qlCurrentTypePlaces.clear();
	
	QString regexp=myDart->qsCurrentPlaceType.replace(";","|");
	if(regexp.length()==0) regexp="|";
	if(regexp[regexp.length()-1]=='|') regexp=regexp.left(regexp.length()-1);
	
	for(int i=0;i<max;i++) {
		if(myDart->qlAllPlaces[i].placeType.contains(QRegExp(regexp))) {
			myDart->qlCurrentTypePlaces.append(&(myDart->qlAllPlaces[i]));
//			qDebug() << myDart->qlAllPlaces[i].x << myDart->qlAllPlaces[i].y << myDart->qlAllPlaces[i].dimx << myDart->qlAllPlaces[i].dimy << myDart->qlAllPlaces[i].name << myDart->qlAllPlaces[i].placeType << regexp;
		}
	}
	
	myDart->vCreatePlacesSubsetsActions();
	myDart->vUpdatePlacesSubsetActive();
	
	if(myDart->bPlacesSubsetActive) {
		qDebug() << "[i] found" << myDart->qlCurrentTypePlaces.count() << "places for current place type, will remove some of them";
		for(int i=0; i<myDart->qlPlacesSubsetsActions.count(); i++) {
			if(!myDart->qlPlacesSubsetsActions[i]->isChecked()) {
				for(int j=i*10; j<i*10+10 && j<myDart->qlCurrentTypePlaces.count(); j++) {
					myDart->qlCurrentTypePlaces[j]=NULL;
				}
			}
		}
		qDebug() << "[i] removed" << myDart->qlCurrentTypePlaces.removeAll(NULL) << "places because of subset setting";
	}
	
	if(myDart->qlCurrentTypePlaces.count()==0) {
		qDebug() << "[I] there is no place for placetype" << myDart->qsCurrentPlaceType; // TODO can this happan at all?
		qDebug() << "    falling back to everything";
		myDart->vSetPlaceType("");
	} else {
		qDebug() << "[i] found" << myDart->qlCurrentTypePlaces.count() << "places for current place type";
		
		QString s=tr("%1 out of %2 places selected")
		             .arg(myDart->qlCurrentTypePlaces.count())
			     .arg(myDart->qlAllPlaces.count());
		if(myDart->qlCurrentTypePlaces.count()==myDart->qlAllPlaces.count())
			s=tr("All %1 places selected").arg(myDart->qlCurrentTypePlaces.count());
		myDart->actionNumberOfSelectedPlaces->setText(s);
		myDart->actionNumberOfSelectedPlaces2->setText(s);
	}
	
	settings->setValue("qsCurrentPlaceType", myDart->qsCurrentPlaceType);
}

int io::iReadOsm(QString filename) {
	qDebug() << "[i] Reading" << filename;
	
	// parsing line-by-line is probably easier than xml parsing
	
	//open input file
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "couldn't open" << filename;
		return(2);
	}
	QTextStream in(&file);
	
	int height=1, width=1, count=0;
	QList<place> places;
	
	// loop through all lines
	while(!in.atEnd()) {
		QString line = in.readLine();
		
		if (line.contains("map-clipping-rect")) { // map size
			QStringList list;
			list = line.split(line.contains("\"") ? "\"" : "\'"); // support ' as well as "
			for(int i=0, max=list.count(); i<max; i++){
				if(list[i].contains("height=")) {
					i++;
					height=list[i].left(list[i].length()-2).toDouble(); // remove "px"
					qDebug() << "height" << height;
				} else if(list[i].contains("width=")) {
					i++;
					width=list[i].left(list[i].length()-2).toDouble(); // remove "px"
					qDebug() << "width" << width;
				}
			}
		
		} else if(line.contains("<text")) { // if there is a place label
			QStringList list;
			list = line.split(line.contains("\"") ? "\"" : "\'"); // support ' as well as "
			
			place pl;
			
			pl.x=-1;
			pl.x=-1;
			pl.name="-1";
			pl.dimx=0;
			pl.dimy=0;
			pl.placeType="everything";
			
			for(int i=0, max=list.count(); i<max; i++){
				if(list[i].contains("x=")) {
					pl.x=list[++i].toDouble()*600/width;
					qDebug() << "x" << pl.x;
				} else if(list[i].contains("y=")) {
					pl.y=list[++i].toDouble()*600/height;
					qDebug() << "y" << pl.y;
				} else if(list[i].contains("class=")) {
					int n=list[++i].indexOf("-"); // remove "-caption"
					pl.placeType=list[i].left(n);
					qDebug() << "placetype" << pl.placeType;
				} else if(list[i].contains("/text")) {
					int n=list[i].indexOf("<");
					pl.name=list[i].mid(1,n-1); //remove > and </text>
					qDebug() << "name" << pl.name;
				}
			}
			
			if(pl.x==-1 || pl.y==-1 || pl.name=="-1") {
				qDebug() << "[E] incomplete place";
				qDebug() << "    " << pl.name << pl.x << pl.y << pl.placeType;
				return -1;
			} else if (pl.placeType=="everything") {
				qDebug() << "[W] incomplete place";
				qDebug() << "    " << pl.name << pl.x << pl.y << pl.placeType;
			}
			
			places.append(pl);
			count++;
			
		}
		
	} // while(!in.atEnd())
	
	qcfFile f;
	f.path=QDir::homePath()+"/Dokumente/GitHub/QeoDart/osm/outtest";
	iWriteQcf(places, f);
	
	return 0;
}

// writes a qcfx-file containing places to the path specified in f
int io::iWriteQcf(QList<place> &places, qcfFile &f) {
	
	qDebug() << "[i] going to write file" << f.path;
	
	QDomDocument doc("qcfx");
	QDomElement root=doc.createElement("qcfx");
	root.setAttribute("version","0.2");
	doc.appendChild(root);
	
	QDomElement cn;
	cn=doc.createElement("name");
	cn.setAttribute("default",f.mapName);
	root.appendChild(cn);
	
	cn=doc.createElement("copyright");
	cn.setAttribute("file",f.copyright.file);
	cn.setAttribute("background",f.copyright.background);
	cn.setAttribute("borders",f.copyright.borders);
	cn.setAttribute("rivers",f.copyright.rivers);
	cn.setAttribute("elevations",f.copyright.elevations);
	root.appendChild(cn);
	
	cn=doc.createElement("pxtokm");
	cn.setAttribute("value",f.pxtokm);
	
	for(int i=places.count()-1; i>-1; i--) {
		cn=doc.createElement("place");
		cn.setAttribute("x",places[i].x);
		cn.setAttribute("y",places[i].y);
		if(places[i].dimx!=0) cn.setAttribute("dimx",places[i].dimx); // we de not want to bloat the file
		if(places[i].dimy!=0) cn.setAttribute("dimy",places[i].dimy);
		cn.setAttribute("name",places[i].name);
		cn.setAttribute("placetype",places[i].placeType);
		root.appendChild(cn);
	}
	
	QFile file(f.path+".qcfx");
	if( !file.open(QIODevice::WriteOnly) ) {
		qDebug() << "[E] Cannot write file" << file.fileName();
		return -1;
	}
	
	QTextStream ts( &file );
	ts << doc.toString();
	
	file.close();
	
	return 0;
}

void io::vLoadSettings() {
	QString configPath=QCoreApplication::applicationDirPath()+"/QeoDart.conf";
	
	if(!bDeterminedPortable) {
		bDeterminedPortable=true;
		
		QFile file(configPath);
	
		if(file.exists(configPath)) {
			bPortable=true;
			qDebug() << "[i] found" << configPath << "-> portable";
			
			if(!file.open(QIODevice::ReadWrite)) {
				QMessageBox msgBox;
				msgBox.addButton(tr("Continue anyway"), QMessageBox::AcceptRole);
				QPushButton *leavePortableButton = msgBox.addButton(tr("Leave portable mode"), QMessageBox::AcceptRole);
				msgBox.setText(QString(tr("The configuration file %1 cannot be accessed by QeoDart. Please ensure that you have got read and write privileges.\nDo you want QeoDart to ignore this error and continue without saving any settings, or do you want to leave the portable mode and use a configuration file in your user directory?")).arg(configPath));
				msgBox.setIcon(QMessageBox::Warning);
				msgBox.exec();
				
				if(msgBox.clickedButton()==leavePortableButton) bPortable=false;
			} // if !ReadWrite
		} else {
			qDebug() << "[i] no" << configPath << "-> not portable";
		}
		
		file.close();
	}
	
	if( !bPortable ) {
#ifdef Q_OS_UNIX
		configPath=QDir::homePath()+"/.config/QeoDart/QeoDart.conf";
#endif
#ifdef Q_OS_WIN32
		configPath=QString(getenv("APPDATA"))+"/QeoDart/QeoDart.conf";
#endif
	}
	
	qDebug() << "[i] using conf file" << configPath;
	
	settings = new QSettings(configPath, QSettings::IniFormat);
	
	qDebug()<<settings->fileName();
	
	
	// UI
	
	myDart->dZoomFactor=settings->value("dZoomFactor",1).toDouble();
	if(myDart->dZoomFactor<=0) myDart->dZoomFactor=1;
	
	myDart->iAskForMode=static_cast<enAskForModes>(settings->value("iAskForMode",enPositions).toInt());
	if(myDart->iAskForMode!=enPositions && myDart->iAskForMode!=enNames) myDart->iAskForMode=enPositions;
	
	myDart->iNumberOfPlayers=settings->value("iNumberOfPlayers",1).toInt();
	if(myDart->iNumberOfPlayers<=0) myDart->iNumberOfPlayers=1;
	
	myDart->qsCurrentPlaceType=settings->value("qsCurrentPlaceType","").toString();
	
	myDart->bAgainstTime=settings->value("bAgainstTime",FALSE).toBool();
	
	myDart->iGameMode=static_cast<enGameModes>(settings->value("iGameMode",enLocal).toInt());
	if(myDart->iGameMode!=enFind && myDart->iGameMode!=enTraining && myDart->iGameMode!=enLocal && myDart->iGameMode!=enNetwork) myDart->iGameMode=enLocal;
	
	myDart->iToolMenuBarState=static_cast<enToolMenuBarState>(settings->value("iToolMenuBarState",enBoth).toInt());
	if(myDart->iToolMenuBarState!=enBoth && myDart->iToolMenuBarState!=enMenuBarOnly && myDart->iToolMenuBarState!=enToolBarOnly) myDart->iToolMenuBarState=enBoth;
	
	myDart->actionBorders->setChecked(settings->value("borders",true).toBool());
	myDart->actionRivers->setChecked(settings->value("rivers",true).toBool());
	myDart->actionElevations->setChecked(settings->value("elevations",true).toBool());
	
	for(int i=0; i<3; i++) {
		myDart->agLayers->actions()[i]->setChecked(settings->value(myDart->qlLayersNames[i+1],true).toBool());
	}
	
	// General
	
	if(myDart->qsLanguage!=settings->value("qsLanguage","default").toString()) {
		myDart->qsLanguage=settings->value("qsLanguage","default").toString();
		myDart->vRetranslate();
	}
	myDart->qsPreferedQcfLanguage=settings->value("qsPreferedQcfLanguage","ui,default,en").toString();
	
	myDart->iMaxPlaceCount=settings->value("iMaxPlaceCount",10).toInt();
	if(myDart->iMaxPlaceCount<=0) myDart->iMaxPlaceCount=10;
	
	myDart->iMaxTime=settings->value("iMaxTime",20).toInt();
	if(myDart->iMaxTime<=0) myDart->iMaxTime=20;
	
	myDart->bResetCursor=settings->value("bResetCursor",true).toBool();
	myDart->bAutoNewGame=settings->value("bAutoNewGame",false).toBool();
	myDart->bAutoShowHighScores=settings->value("bAutoShowHighScores",true).toBool();
	
	myDart->iScoreAreaMode=settings->value("iScoreAreaMode",1).toInt();
	if(myDart->iScoreAreaMode<0 || myDart->iScoreAreaMode>2) myDart->iScoreAreaMode=1;
	myDart->iPenalty=settings->value("iPenalty",1).toInt();
	if(myDart->iPenalty<0 || myDart->iPenalty>2) myDart->iPenalty=1;
	
	myDart->iLettersPerSecond=settings->value("iLettersPerSecond",8).toInt();
	if(myDart->iLettersPerSecond<1) myDart->iLettersPerSecond=8;
	
	
	// Advanced
	
	myDart->iDelayNextCircle=settings->value("iDelayNextCircle",200).toInt();
	myDart->iDelayNextPlayer=settings->value("iDelayNextPlayer",1000).toInt();
	myDart->iDelayNextPlace=settings->value("iDelayNextPlace",2000).toInt();
	myDart->iDelayNextPlaceTraining=settings->value("iDelayNextPlaceTraining",1000).toInt();
	
	myDart->bUseOurCursor=settings->value("bUseOurCursor",false).toBool();
	myDart->bShortenToolbarText=settings->value("bShortenToolbarText",true).toBool();
	
#ifndef Q_OS_WINCE
	if(myDart->bUseOurCursor) myDart->lblMouseClickOverlay->setCursor(QCursor(QPixmap(":/icons/cursor.png"),1,1));
	else myDart->lblMouseClickOverlay->setCursor(Qt::ArrowCursor);
#endif
}

void io::vLoadHighScores(QString mapName) {
	myDart->qlHighScores.clear();
	
	for(int i=0; i<10; i++) {
		QStringList list=settings->value(QString("Highscores/%1.%2").arg(qsGetIdFromMapName(mapName)).arg(i),"---||0").toString().split("||");
		qDebug() << list;
		if(list.count()<2) list=QString("---||0").split("||");
		
		highScoreEntry entry;
		entry.name=list[0];
		entry.score=list[1].toDouble();
		myDart->qlHighScores.append(entry);
		
		qDebug() << entry.name << "R highsc" << entry.score;
	}
}

void io::vSaveHighScores(QString id) {
	for(int i=0; i<10; i++) {
		settings->setValue(QString("Highscores/%1.%2").arg(id).arg(i),
		                   QString("%1||%2").arg(myDart->qlHighScores[i].name).arg(myDart->qlHighScores[i].score));
		
		qDebug() << myDart->qlHighScores[i].name << "W highsc" << myDart->qlHighScores[i].score;
	}
}

// returns the id of the map with the given mapName
QString io::qsGetIdFromMapName(QString mapName) {
	for(int i=0; i<myDart->qlQcfxFiles.count(); i++) {
		if(myDart->qlQcfxFiles[i].mapName==mapName) return myDart->qlQcfxFiles[i].id;
	}
	return "NULL";
}
