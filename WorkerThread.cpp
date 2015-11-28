/*
	WHDRun - WHDLoad Runner for GameBase/MAGE Amiga
	(c)2007 Markus Thiele / MAGE Project

	WHDRun is free software under the GNU General Public License,
	Version 2, which should be included in the file whdrun-license.txt
*/

#include "Checksum.h"
#include "Names.h"
#include "UserStartup.h"
#include "WorkerThread.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

#include <ziparchive/ZipArchive.h>

WorkerThread::WorkerThread(): QThread(NULL),
	m_die(false),
	m_emulatorProcess(NULL) {
}

void WorkerThread::run() {
	m_die = false;
	if(!m_die) runTest();
	if(!m_die) runConfig();
	if(!m_die) runUnzip();
	if(!m_die) runEmulator();
	if(!m_die) runRezip();
}

WorkerThread::~WorkerThread() {
	if(m_emulatorProcess != NULL) m_emulatorProcess->kill();
	m_die = true;
	if(!wait(3000)) terminate();
}

void WorkerThread::runTest() {
	emit(beginRunTest());
	emit(stageEvent("Checking files and paths"));
	bool error = false;
	
	if(!QFileInfo(m_emuFileName).exists()) {
		emit(errorEvent("Specified winuae.exe not found (" + m_emuFileName + ")"));
		error = true;
	}
	
	if(!QFileInfo(m_gameFileName).exists()) {
		emit(errorEvent("Specified game pack not found (" + m_gameFileName + ")"));
		error = true;
	}
	
	if(!QFileInfo(m_configFileName).exists()) {
		emit(errorEvent("Specified config file not found (" + m_configFileName + ")"));
		error = true;
	}
	
	if(!QFileInfo(m_startupFileName).exists()) {
		emit(errorEvent("Specified user-startup not found (" + m_startupFileName + ")"));
		error = true;
	}
	
	bool tempError = false;
	QFileInfo tempPathInfo(m_tempPath);
	m_lockFile.setFileName(m_tempPath + "/" __WHDRun__LockFileName);
	if(tempPathInfo.exists()) {
		if(!tempPathInfo.isDir()) {
			emit(errorEvent("Temp path is an already existing file (" + m_tempPath +")"));
			tempError = true;
		} else {
			QDir tempDir(m_tempPath);
			if(tempDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot).count() > 0) {
				if(!m_lockFile.exists()) {
					errorEvent("Temp path already contains files apparently not created by WHDRun (" + m_tempPath + ")");
					tempError = true;
				} else if (!m_lockFile.remove()) {
					errorEvent("Temp path already in use by another instance of WHDRun (" + m_tempPath + ")");
					tempError = true;
				}
			}
		}
	}
	if(!tempError) {
		if(!removeFiles(m_tempPath)) {
			emit(errorEvent("Error while removing old files in temp path (" + m_tempPath + ")"));
			error = true;
		}
		QDir::current().mkpath(m_tempPath);
		tempPathInfo = QFileInfo(m_tempPath);
		if(!tempPathInfo.exists() || !tempPathInfo.isDir()) {
			emit(errorEvent("Temp path is invalid and/or could not be created (" + m_tempPath + ")"));
			error = true;
		}
		if(!m_lockFile.open(QIODevice::ReadWrite)) {
			emit(errorEvent("Could not create lock file in temp path (" + m_lockFile.fileName() + ")"));
			error = true;
		}
	} else error = true;

	QFileInfo dataDirInfo(m_dataPath);
	if(!dataDirInfo.exists() || !dataDirInfo.isDir()) {
		QDir::current().mkpath(m_dataPath);
		dataDirInfo = QFileInfo(m_dataPath);
		if(!dataDirInfo.exists() || !dataDirInfo.isDir()) {
			emit(errorEvent("Data path is invalid and/or could not be created (" + m_dataPath + ")")); error = true;
			error = true;
		}
	}
	
	if(error) m_die = true;
}

void WorkerThread::runConfig() {
	emit(stageEvent("Configuring WHDLoad slave"));
	bool ok;
	UserStartup userStartup(m_startupFileName, &ok);
	if(!ok) {
		emit(errorEvent("Could not read user-startup (" + m_startupFileName + ")"));
		m_die = true;
		return;
	}
	
	//QString baseKey = Names::baseName(m_gameFileName).toLower(); // base names aren't unique
	QString baseKey = userStartup.slaveName().toLower();
	
	QString appPath = QCoreApplication::applicationDirPath();
	QString dataPath = appPath + "/whdrun-data"; // default
	QSettings globalSettings(appPath + "/WHDRun.ini", QSettings::IniFormat);
	if(globalSettings.value("WHDRun/dataPath.useCustom", false).toBool()) {
		dataPath = globalSettings.value("WHDRun/dataPath", m_dataPath).toString();
	}
	
	QString iniPath = m_dataPath + "/__WHDRun__Games.ini";
	
	QSettings ini(iniPath, QSettings::IniFormat);

	if(ini.value(baseKey + "/quitKey.useCustom", 0).toInt() == 1) {
		QString quitKey = ini.value(baseKey + "/quitKey", "$4B").toString();
		quitKey = quitKey.right(quitKey.size() - 1);
		bool ok;
		quitKey = QString("%1").arg(quitKey.toInt(&ok, 16));
		userStartup.set("QUITKEY", ok ? quitKey : "75"); // 0x5D == 93; 0x4B == 75
	}
	
	if(ini.value(baseKey + "/CUSTOM.useCustom", 0).toInt() == 1) {
		userStartup.set("CUSTOM", "\"" + ini.value(baseKey + "/CUSTOM", "").toString() + "\"");
	}
	
	if(ini.value(baseKey + "/CUSTOM1.useCustom", 0).toInt() == 1) {
		userStartup.set("CUSTOM1", ini.value(baseKey + "/CUSTOM1", 0).toString());
	}
	
	if(ini.value(baseKey + "/CUSTOM2.useCustom", 0).toInt() == 1) {
		userStartup.set("CUSTOM2", ini.value(baseKey + "/CUSTOM2", 0).toString());
	}
	
	if(ini.value(baseKey + "/CUSTOM3.useCustom", 0).toInt() == 1) {
		userStartup.set("CUSTOM3", ini.value(baseKey + "/CUSTOM3", 0).toString());
	}
	
	if(ini.value(baseKey + "/CUSTOM4.useCustom", 0).toInt() == 1) {
		userStartup.set("CUSTOM4", ini.value(baseKey + "/CUSTOM4", 0).toString());
	}
	
	if(ini.value(baseKey + "/CUSTOM5.useCustom", 0).toInt() == 1) {
		userStartup.set("CUSTOM5", ini.value(baseKey + "/CUSTOM5", 0).toString());
	}
	
	if(!userStartup.save(m_startupFileName)) {
		emit(errorEvent("Could not write to user-startup (" + m_startupFileName + ")"));
		m_die = true;
		return;
	}
}

void WorkerThread::runUnzip() {
	emit(beginRunUnzip());
	emit(stageEvent("Extracting and merging game data"));
	
	emit(infoEvent("Unzipping original game files"));
	CZipArchive zip;
	try {
	    zip.Open(m_gameFileName.toStdString().c_str(), CZipArchive::zipOpenReadOnly);
		
		ZIP_SIZE_TYPE totalSize = 0;
		for(ZIP_INDEX_TYPE i = 0; i < zip.GetCount(); i++) {
			totalSize += zip.GetFileInfo(i)->m_uUncomprSize; // uncompressed size
			QString name = QString::fromUtf8(zip.GetFileInfo(i)->GetFileName());
			if(!validName(name)) {
				emit(errorEvent("Game pack contains illegal file names (e.g. " + name + ")"));
				emit(infoEvent("A near future release of WHDRun will fix this problem, sorry for the inconvenience"));
				m_die = true;
				return;
			}
		}
		emit(unzipTotalSize(totalSize));

		ZIP_SIZE_TYPE progress = 0;
		for(ZIP_INDEX_TYPE i = 0; i < zip.GetCount(); i++) {
			progress += zip.GetFileInfo(i)->m_uUncomprSize;
			zip.ExtractFile(i, m_tempPath.toUtf8());
			emit(unzipProgress(progress));
		}

	    zip.Close();
    } catch(CZipException ex) {
		zip.Close(CZipArchive::afAfterException);
		emit(errorEvent(QString("Error while unzipping: %1").arg((LPCTSTR)ex.GetErrorDescription())));
		m_die = true; // no need to rezip
		return;
	}

	emit(infoEvent("Collecting checksums of unmodified files"));
	emit(beginCollect());
	m_checksums.clear();
	collectFiles(m_tempPath, &m_checksums); // TODO: progress for this? (should always be quick)
	
	QString gameFileBase = QFileInfo(m_gameFileName).baseName();
	QString diffFileName = m_dataPath + "/" + gameFileBase + __WHDRun__DiffSuffix;
	if(QFileInfo(diffFileName).exists()) {
		emit(infoEvent("Merging original game files with previously modified files"));
		CZipArchive zip;
		try {
		    zip.Open(diffFileName.toUtf8(), CZipArchive::zipOpenReadOnly);

			//TODO: progress infoEvent for this? (should always be quick)
			for (ZIP_INDEX_TYPE i = 0; i < zip.GetCount(); i++) {
				zip.ExtractFile(i, m_tempPath.toUtf8());
			}

		    zip.Close();
	    } catch(CZipException ex) {
			zip.Close(CZipArchive::afAfterException);
			emit(errorEvent(QString("Error while unzipping changes: %1").arg((LPCTSTR)ex.GetErrorDescription())));
			m_die = true; // no need to rezip
			return;
		}
	}
	
	emit(endRunUnzip());
}

void WorkerThread::runEmulator() {
	emit(beginRunEmulator());
	emit(stageEvent("Running game in WinUAE"));
	QProcess emulatorProcess(NULL);
	m_emulatorProcess = &emulatorProcess;
	emulatorProcess.start(m_emuFileName, QStringList() << "-f" << m_configFileName, QIODevice::NotOpen);
	if(!emulatorProcess.waitForStarted()) {
		m_emulatorProcess = NULL;
		emit(errorEvent("Error while starting WinUAE"));
		m_die = true; // no need to rezip
		return;
	}
	emit(emulatorRunning(true));
	emit(infoEvent("Waiting for WinUAE to finish"));
	emulatorProcess.waitForFinished(-1); // wait until WinUAE is actually done
	emit(emulatorRunning(false));
	m_emulatorProcess = NULL;
	if(emulatorProcess.exitCode() == 0) {
		emit(infoEvent("WinUAE finished without errors"));
	} else {
		emit(errorEvent(QString("WinUAE finished with error code %1").arg(emulatorProcess.exitCode())));
	}
	emit(endRunEmulator());
}

void WorkerThread::runRezip() {
	emit(beginRunRezip());
	emit(stageEvent("Finding and saving modified game data"));
	
	emit(infoEvent("Identifying modified files (including previously modified files)"));
	QMap<QString, uint32_t> newChecksums;
	collectFiles(m_tempPath, &newChecksums);
	QStringList modified; // TODO: also determine removed files?
	QMapIterator<QString, uint32_t> i(newChecksums);
	while (i.hasNext()) {
		i.next();
		if(!m_checksums.contains(i.key()) || m_checksums[i.key()] != i.value()) { modified << i.key(); }
	}
	
	if(modified.empty()) {
		emit(infoEvent("No modified files found"));
		return;
	}
	
	emit(infoEvent("Rezipping modified files"));

	QString gameFileBase = QFileInfo(m_gameFileName).baseName();
	QString diffFileName = m_dataPath + "/" + gameFileBase + __WHDRun__DiffSuffix;
	CZipArchive zip;
	try {
		zip.Open(diffFileName.toUtf8(), CZipArchive::zipCreate);

		// TODO: review that path finding stuff some time.
		QDir tempDir(m_tempPath); tempDir.makeAbsolute();
		foreach(QString externalPath, modified) {
			QDir externalDir(externalPath); externalDir.makeAbsolute();
			QString internalPath = tempDir.relativeFilePath(externalDir.absolutePath());
			zip.AddNewFile((LPCTSTR)externalPath.toUtf8(),
				(LPCTSTR)internalPath.toUtf8(), CZipCompressor::levelBest);
		}

		zip.Close();
	} catch(CZipException ex) {
		zip.Close(CZipArchive::afAfterException);
		emit(errorEvent(QString("Error while rezipping: %1").arg((LPCTSTR)ex.GetErrorDescription())));
		return;
	}
	
	emit(endRunRezip());
}

bool WorkerThread::validName(QString name) {
	name = name.toLower();

	if(name.endsWith(".")) return false;

	// TODO: what about / and \\ ?
	if(name.contains("*")
		|| name.contains("?")
		|| name.contains(":")
		|| name.contains("\"")
		|| name.contains("<")
		|| name.contains(">")
		|| name.contains("|")) {
		return false;
	}
		
	if(name == "aux" || name == "con" || name == "nul" || name == "prn") {
		return false;
	}
	
	return true;
}

void WorkerThread::collectFiles(QString path, QMap<QString, uint32_t>* checksums) {
	QDir dir(path);
	QFileInfoList entries = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
	char* buffer = new char[65536];
	foreach(QFileInfo entry, entries) {
		if(entry.isDir()) {
			collectFiles(entry.filePath(), checksums);
		} else {
			Checksum crc;
			QFile file(entry.filePath());
			file.open(QIODevice::ReadOnly);
			qint64 bytesRead = 0;
			while((bytesRead = file.read(buffer, sizeof(buffer))) > 0) {
				crc.update(buffer, bytesRead);
			}
			checksums->insert(entry.filePath(), crc.value());
		}
	}
	delete [] buffer;
}


bool WorkerThread::removeFiles(QString path) {
	QDir dir(path);
	QFileInfoList entries = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
	bool result = true;
	foreach(QFileInfo entry, entries) {
		if(entry.isDir()) {
			if(!removeFiles(entry.filePath())) return false;
			if(!dir.rmdir(entry.fileName())) return false;
		} else {
			if(!dir.remove(entry.fileName())) return false;
		}
	}
	return true;
}
