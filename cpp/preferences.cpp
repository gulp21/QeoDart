#include "preferences.hpp"

using namespace std;

preferences::preferences(dart *TDart, io *TIO, QDialog *parent) : myDart(TDart), myIO(TIO), QDialog(parent) {
	setupUi(this);
	
	connect(cbResetCursor,SIGNAL (clicked()), this, SLOT(vSettingChanged()));
}

preferences::~preferences() {
}

void preferences::vSettingChanged() {
}

