/*
	WHDRun - WHDLoad Runner for GameBase/MAGE Amiga
	(c)2007 Markus Thiele / MAGE Project

	WHDRun is free software under the GNU General Public License,
	Version 2, which should be included in the file whdrun-license.txt
*/

#ifndef __MAGE__WHDRun__WorkerThread__H
#define __MAGE__WHDRun__WorkerThread__H

#include <QFile>
#include <QMap>
#include <QProcess>
#include <QString>
#include <QThread>

#include <stdint.h>

#define __WHDRun__DiffSuffix "__Changes.___"
#define __WHDRun__LockFileName "__WHDRun__LockFile.___"

class WorkerThread: public QThread {
	Q_OBJECT

	public:
		WorkerThread();

		void setGameFileName(QString gameFileName) { m_gameFileName = gameFileName; }
		void setTempPath(QString tempPath) { m_tempPath = tempPath; }
		void setEmuFileName(QString emuFileName) { m_emuFileName = emuFileName; }
		void setConfigFileName(QString configFileName) { m_configFileName = configFileName; }
		void setStartupFileName(QString startupFileName) { m_startupFileName = startupFileName; }
		void setDataPath(QString dataPath) { m_dataPath = dataPath; }
		
		void run();
		
		void die();
		~WorkerThread();
		
	public slots:
		void killEmulator() { if(m_emulatorProcess != NULL) m_emulatorProcess->kill(); }
	
	private:
		void runTest();
		void runConfig();
		void runUnzip();
		void runEmulator();
		void runRezip();
		
		void collectFiles(QString path, QMap<QString, uint32_t>* checksums);
		bool removeFiles(QString path);
		bool validName(QString name);

	signals:
		void beginRunTest();
		void endRunTest();
		void beginRunUnzip();
		void endRunUnzip();
		void beginRunEmulator();
		void endRunEmulator();
		void beginRunRezip();
		void endRunRezip();
		
		void unzipTotalSize(unsigned long totalSize);
		void unzipProgress(unsigned long progress);
		void beginCollect();
		
		void emulatorRunning(bool);
		
		void stageEvent(QString message);
		void errorEvent(QString message);
		void infoEvent(QString message);
	
	private:
		QString m_gameFileName;
		QString m_tempPath;
		QString m_emuFileName;
		QString m_configFileName;
		QString m_startupFileName;
		QString m_dataPath;
		
		QFile m_lockFile;
		QProcess* m_emulatorProcess;
		QMap<QString, uint32_t> m_checksums;
		bool m_die;	
};

#endif
