#include "UserStartup.h"
#include "Version.h"

#include <QFile>
#include <QTextStream>

bool UserStartup::open(QString fileName) {
	clear();
	
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly)) return false;
	
	QTextStream in(&file);
	while(!in.atEnd()) {
		QString line = in.readLine();
		if(line.startsWith("cd ", Qt::CaseInsensitive)) {
			int length = line.size() - 3;
			if(line.endsWith(";")) --length;
			m_slaveDir = line.mid(3, length).trimmed();
		} else if(line.startsWith("whdload ", Qt::CaseInsensitive)) {
			QStringList tokens = tokenize(line, 8);
			if(tokens.size() < 1) return (clear(), false);
			m_slaveName = tokens[0];
			int i = 1;
			while(i < tokens.size()) {
				if(tokens[i] == ";") break; // rest is commented out
				if(tokens.value(i + 1) == "=") {
					if(i + 2 < tokens.size()) {
						m_params[tokens[i].toUpper()] = tokens[i + 2];
						i += 3;
					} else break; // something is wrong at the end
				} else {
					m_params[tokens[i].toUpper()] = QString(); // switch
					++i;
				}
			}
		}
	}
	
	file.close();
	return true;
}

QStringList UserStartup::tokenize(QString line, int startIndex) {
	QStringList tokens;
	
	QString token;
	bool inString = false;
	//QChar lastChar = QChar();
	for(unsigned int i = startIndex; i < line.size(); i++) {
		if((line[i] == '=' || line[i] == ';') && !inString) {
			token = token.trimmed();
			if(!token.isEmpty()) tokens << token;
			tokens << QString(line[i]);
			token.clear();
		} else if((line[i].isSpace() && !inString) || i == line.size() - 1) {
			token += line[i];
			token = token.trimmed();
			if(!token.isEmpty()) tokens << token;
			token.clear();
		} else {
			if(line[i] == '"') {
				if(!inString) {
					inString = true;
				} else if(inString /* && lastChar != '\\' */) {
					inString = false;
				}
			}
			token += line[i];
		}
		//lastChar = line[i];
	}
	return tokens;
}

UserStartup::UserStartup(QString fileName, bool* ok) {
	bool result = open(fileName);
	if(ok != NULL) *ok = result;
}

bool UserStartup::save(QString fileName) {
	if(fileName.isNull()) fileName = m_fileName;
	
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly)) return false;
	
	QString cdCommand = "cd " + m_slaveDir;
	QString whdloadCommand = "whdload " + m_slaveName;
	QMapIterator<QString, QString> param(m_params);
	while(param.hasNext()) {
		param.next();
		whdloadCommand += " " + param.key();
		if(!param.value().isNull()) whdloadCommand += "=" + param.value();
	}
	
	QTextStream out(&file);
	out << "echo \"\";\x0D\x0A";
	out << "echo \"" __MAGE__WHDRun__LongNameAndVersion__ "\";\x0D\x0A";
	out << "echo \"Running: " + m_slaveName + "\";\x0D\x0A";
	out << "echo \"\";\x0D\x0A";
	out << "echo \"Remember to use the quit key instead of closing WinUAE!\";\x0D\x0A";
	out << "echo \"\";\x0D\x0A";
	out << cdCommand << ";\x0D\x0A";
	out << whdloadCommand << ";\x0D\x0A";
	
	file.close();
	return true;
}

void UserStartup::clear() {
	m_fileName = QString();
	m_slaveDir = QString();
	m_slaveName = QString();
	m_params.clear();
}
