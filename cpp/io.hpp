/*
QeoDart Copyright (C) 2011 Markus Brenneis
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
See main.cpp for details. */

#ifndef IO_HPP
#define IO_HPP

#include <QWidget>

class dart;

class io : public QWidget {
	Q_OBJECT
	
	dart *myDart;
	
	public:
		io(dart*);
		~io();
		int iReadQcf(QString filename);
		int iReadOsm(QString filename);
// 	private:
};
#endif //IO_HPP 
