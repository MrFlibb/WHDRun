/*
	WHDRun - WHDLoad Runner for GameBase/MAGE Amiga
	(c)2007 Markus Thiele / MAGE Project

	WHDRun is free software under the GNU General Public License,
	Version 2, which should be included in the file whdrun-license.txt
*/

#ifndef __WHDRun__Names__H
#define __WHDRun__Names__H

#include <QMap>
#include <QPair>
#include <QString>

#define __WHDRun__InfoUrlTemplate "http://whdload.de/games/%1.html"

class Names {
	public:
		static void load(QString fileName);
		static QString get(QString from);
		static QString baseName(QString from);
		static QString infoUrl(QString from);
		
	private:
		static QMap<QString, QPair<QString, QString> > m_names;
		static bool m_initialized;
};

#endif
