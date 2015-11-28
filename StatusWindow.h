/*
	WHDRun - WHDLoad Runner for GameBase/MAGE Amiga
	(c)2007 Markus Thiele / MAGE Project

	WHDRun is free software under the GNU General Public License,
	Version 2, which should be included in the file whdrun-license.txt
*/

#ifndef __MAGE__WHDRun__StatusWindow__H
#define __MAGE__WHDRun__StatusWindow__H

#include "WorkerThread.h"

#include <QAction>
#include <QMainWindow>
#include <QMenu>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QTreeWidget>

class StatusWindow: public QMainWindow {
	Q_OBJECT
	
	public:
		StatusWindow(WorkerThread* workerThread, QString gameFileName, QWidget* parent = NULL);
		
	public slots:
		void trayActivated(QSystemTrayIcon::ActivationReason reason);
		void quitProgram();
		void about();
		void copyLog();
		void kill();
		void config();
		
		void stageEvent(QString description);
		void infoEvent(QString description);
		void errorEvent(QString description);
		void finished();
		
	signals:
		void killEmulator();
		
	protected:
		void closeEvent(QCloseEvent* event);

	private:
		QSystemTrayIcon* m_trayIcon;
		QTreeWidget* m_eventLog;
		
		QPushButton* m_hideButton;
		QAction* m_hideAction;
		
		QString m_gameFileName;
		
		bool m_error;
		bool m_done;
		
		StatusWindow();
};

#endif // __MAGE__WHDRun__StatusWindow__H
