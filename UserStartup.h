/*
	WHDRun - WHDLoad Runner for GameBase/MAGE Amiga
	(c)2007 Markus Thiele / MAGE Project

	WHDRun is free software under the GNU General Public License,
	Version 2, which should be included in the file whdrun-license.txt
*/

#ifndef __WHDRun__UserStartup__H
#define __WHDRun__UserStartup__H

#include <QString>
#include <QStringList>
#include <QMap>

class UserStartup {
	public:
		bool open(QString fileName);
		UserStartup(QString fileName, bool* ok = NULL);
		bool save(QString fileName = QString());
		
		QString fileName() { return m_fileName; }
		QString slaveDir() { return m_slaveDir; }
		QString slaveName() { return m_slaveName; }
		QMap<QString, QString> params() { return m_params; }
		
		QString get(QString key) { return m_params[key]; }
		void set(QString key, QString value) { m_params[key] = value; }
		
		void clear();
		
	private:
		QString m_fileName;
		QString m_slaveDir;
		QString m_slaveName;
		QMap<QString, QString> m_params; // all keys in uppercase
		
		QStringList tokenize(QString line, int startIndex = 0);
};

#endif
