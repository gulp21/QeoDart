#include "preferences.hpp"

using namespace std;

preferences::preferences(dart *TDart, io *TIO, QDialog *parent) : myDart(TDart), myIO(TIO), QDialog(parent) {
	setupUi(this);
}

preferences::~preferences(){
}

