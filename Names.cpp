/*
	WHDRun - WHDLoad Runner for GameBase/MAGE Amiga
	(c)2007 Markus Thiele / MAGE Project

	WHDRun is free software under the GNU General Public License,
	Version 2, which should be included in the file whdrun-license.txt
*/

#include "Names.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QStringList>

bool Names::m_initialized = false;
QMap<QString, QPair<QString, QString> > Names::m_names;

void Names::load(QString fileName) {
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly)) return;
	QTextStream in(&file);
	
	while(!in.atEnd()) {
		QString line = in.readLine();
		QStringList parts = line.split("\t");
		if(parts.size() < 3) continue;
		m_names[parts[0].toLower()] = QPair<QString, QString>(parts[1], parts[2]);
	}
}

QString Names::get(QString from) {
	if(!m_initialized) {
		load(":/txt/names.txt");
		m_initialized = true;
	}
	QString fileKey = QFileInfo(from).fileName().toLower();
	if(m_names.contains(fileKey)) return m_names[fileKey].second;
	QString baseName = Names::baseName(from);
	QString key = baseName.toLower();
	if(m_names.contains(key)) return m_names[key].second;
	return baseName;
}

QString Names::baseName(QString from) {
	from = QFileInfo(from).fileName();
	if(from.endsWith(".slave", Qt::CaseInsensitive)) from.chop(6);
	if(from.contains("_")) from.truncate(from.indexOf("_"));
	return from.trimmed();
}

QString Names::infoUrl(QString from) {
	if(!m_initialized) {
		load(":/txt/names.txt");
		m_initialized = true;
	}

	QString properName;
	QString fileKey = QFileInfo(from).fileName().toLower();
	if(m_names.contains(fileKey)) {
		properName = m_names[fileKey].first;
	} else {
		QString baseName = Names::baseName(from);
		QString key = baseName.toLower();
		if(m_names.contains(key)) {
			properName = m_names[key].first;
		} else {
			properName = baseName;
		}
	}

	return QString(__WHDRun__InfoUrlTemplate).arg(properName);
}
