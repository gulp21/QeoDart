/*
QeoDart
Copyright (C) 2011 Markus Brenneis

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <stdlib.h>
// #include <QtGui>
#include <math.h>

using namespace std; 

double dblZoom=1.5;
const int RADIUS=10, BORDER=2;

//returns the distance between P(a|b) and Q(x|y); a,b,x,y should be unzoomed
double dblGetDistanceInPxBetween(int a, int b, int x,int y) {
	return sqrt( pow(a-x,2) + pow(b-y,2) ); //thx Pythagoras
}

double dblGetUnzoomed(double x) {
	return x/dblZoom;
}

double dblGetZoomed(double x) {
	return x*dblZoom;
}

//calculate the mark (German system TODO other systems) using unzoomed distance
double dblGetMark(double distance) {
	double mark=distance/10; //TODO check radius
	if(mark<4) {
		return mark<1 ? 1 : mark;
	} else {
		mark=4+(mark-4)/2;
		return mark>6 ?  6 : mark;
	}
}

//draws distance circles around P(x|y), using the saved click-coordinates of place n, iteratin #count [recursion]
// vDrawCircle(int x, int y, int n, int count){
// 	if(count*10 < dblGetDistance(x,y,QLscoreHistory[0][n].x(),QLscoreHistory[0][n].y())){ //TODO check all players
// 		QRectF rectangle(10.0, 20.0, 80.0, 60.0);
// 		QPainter painter(this);
// 		painter.drawEllipse(rectangle);
// 	}
// }

int main(){
	cout << dblGetDistanceInPxBetween(2,3,6,7) << " 5,66" << endl;
	cout << dblGetZoomed(21) << " 31.5 " << dblGetUnzoomed(31.5) << " 21" << endl;
	cout << dblGetMark(10) << " " << dblGetMark(20) << " " << dblGetMark(30) << " + " << dblGetMark(40) << " " << dblGetMark(50) << " " << dblGetMark(60) << " + " << dblGetMark(70) << " " << dblGetMark(80) << " " << dblGetMark(90) << " + " << dblGetMark(67) << " " << dblGetMark(15) << " " << dblGetMark(9) << " " << endl;
}
