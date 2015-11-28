/*
	WHDRun - WHDLoad Runner for GameBase/MAGE Amiga
	(c)2007 Markus Thiele / MAGE Project

	WHDRun is free software under the GNU General Public License,
	Version 2, which should be included in the file whdrun-license.txt
*/

#ifndef __WHDRun__ConfigDialog__H
#define __WHDRun__ConfigDialog__H

#include <QCheckBox>
#include <QDialog>
#include <QString>

class ConfigDialog: public QDialog {
	Q_OBJECT

	public:
		ConfigDialog(QString gameFileName, QString slaveName, bool atStartup = true, QWidget* parent = NULL);
		
	public slots:
		void setGameNeverAgain(int state);
		void setGlobalNeverAgain(int state);
		
		void maybeExec();
	
	private:
		void initGameNeverAgain(QCheckBox* checkBox);
		void initGlobalNeverAgain(QCheckBox* checkBox);

		QString m_dataPath;
		QString m_baseKey;
		QString m_iniPath;
		
		ConfigDialog();
};

#endif
