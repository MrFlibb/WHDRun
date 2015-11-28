/*
	WHDRun - WHDLoad Runner for GameBase/MAGE Amiga
	(c)2007 Markus Thiele / MAGE Project

	WHDRun is free software under the GNU General Public License,
	Version 2, which should be included in the file whdrun-license.txt
*/

#ifndef __WHDRun__ConfigWidget__H
#define __WHDRun__ConfigWidget__H

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QWidget>

class ConfigWidget: public QWidget {
	Q_OBJECT
	
	public:
		ConfigWidget(QString gameFileName, QString slaveName, QWidget* parent = NULL);
		
	public slots:
		void setSwitch(int state);
		void setKey(QString key);
		void setLine(QString value);
		
	private:
		void initSwitch(QCheckBox* checkBox);
		void initKey(QLabel* label, QString defaultValue);
		void initLine(QLineEdit* lineEdit, QString defaultValue);

		QString m_dataPath;
		QString m_baseKey;
		QString m_iniPath;
	
		ConfigWidget();
};

#endif
