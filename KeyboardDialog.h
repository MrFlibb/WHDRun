/*
	WHDRun - WHDLoad Runner for GameBase/MAGE Amiga
	(c)2007 Markus Thiele / MAGE Project

	WHDRun is free software under the GNU General Public License,
	Version 2, which should be included in the file whdrun-license.txt
*/

#ifndef __WHDRun__KeyboardDialog__H
#define __WHDRun__KeyboardDialog__H

#include "KeyboardWidget.h"

#include <QDialog>
#include <QString>

class KeyboardDialog: public QDialog {
	Q_OBJECT
	
	public:
		KeyboardDialog(QWidget* parent = NULL);
	
	signals:
		void selected(QString);
	
	private:
		KeyboardWidget* m_keyboard;
};

#endif
