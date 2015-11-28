#include "ConfigDialog.h"
#include "Names.h"
#include "StatusWindow.h"
#include "UnzipProgressDialog.h"
#include "Version.h"

#include <QApplication>
#include <QBoxLayout>
#include <QClipboard>
#include <QDateTime>
#include <QDesktopWidget>
#include <QHeaderView>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>

StatusWindow::StatusWindow(WorkerThread* workerThread, QString gameFileName, QWidget* parent):
	QMainWindow(parent),
	m_gameFileName(gameFileName),
	m_error(false),
	m_done(false) {

	connect(workerThread, SIGNAL(stageEvent(QString)), SLOT(stageEvent(QString)));
	connect(workerThread, SIGNAL(infoEvent(QString)), SLOT(infoEvent(QString)));
	connect(workerThread, SIGNAL(errorEvent(QString)), SLOT(errorEvent(QString)));
	connect(workerThread, SIGNAL(finished()), SLOT(finished()));
	connect(this, SIGNAL(killEmulator()), workerThread, SLOT(killEmulator()), Qt::DirectConnection);
	
	UnzipProgressDialog* unzipProgress = new UnzipProgressDialog(this);
	connect(workerThread, SIGNAL(unzipTotalSize(unsigned long)), unzipProgress, SLOT(start()));
	connect(workerThread, SIGNAL(unzipTotalSize(unsigned long)), unzipProgress, SLOT(setTotal(unsigned long)));
	connect(workerThread, SIGNAL(unzipProgress(unsigned long)), unzipProgress, SLOT(setProgress(unsigned long)));
	connect(workerThread, SIGNAL(endRunUnzip()), unzipProgress, SLOT(stop()));
	connect(workerThread, SIGNAL(beginCollect()), unzipProgress, SLOT(collecting()));
	
	QIcon aboutIcon(":/img/icon/about.png");
	QIcon hideIcon(":/img/icon/hide.png");
	QIcon showIcon(":/img/icon/show.png");
	QIcon quitIcon(":/img/icon/quit.png");
	QIcon copyIcon(":/img/icon/copy.png");
	QIcon killIcon(":/img/icon/kill.png");
	QIcon configIcon(":/img/icon/config.png");
	QIcon trayIcon(":/img/trayicon.png");
	
	QMenu* trayMenu = new QMenu(this);
	QAction* showAction = trayMenu->addAction(showIcon, "Show WHDRun Status", this, SLOT(show()));
	trayMenu->addAction(aboutIcon, "About WHDRun", this, SLOT(about()));
	trayMenu->addSeparator();
	trayMenu->addAction(hideIcon, "Hide WHDRun Status", this, SLOT(hide()));
	trayMenu->addAction(quitIcon, "Quit WHDRun", this, SLOT(quitProgram()));
	trayMenu->setDefaultAction(showAction);
	
	m_trayIcon = new QSystemTrayIcon(this);
	m_trayIcon->setIcon(trayIcon);
	m_trayIcon->setToolTip(__MAGE__WHDRun__NameAndVersion__);
	m_trayIcon->setContextMenu(trayMenu);
	m_trayIcon->show();
	
	QMenuBar* mainMenu = menuBar();
	QMenu* whdrunMenu = mainMenu->addMenu("WHDRun");
		m_hideAction = whdrunMenu->addAction(hideIcon, "Hide WHDRun Status", this, SLOT(hide()));
		whdrunMenu->addAction(quitIcon, "Quit WHDRun", this, SLOT(quitProgram()));
	QMenu* toolMenu = mainMenu->addMenu("Tools");
		toolMenu->addAction(configIcon, "Configure WHDLoad Slave", this, SLOT(config()));
		toolMenu->addSeparator();
		toolMenu->addAction(copyIcon, "Copy Log to Clipboard", this, SLOT(copyLog()));
		toolMenu->addSeparator();
		QAction* killAction = toolMenu->addAction(killIcon, "Kill WinUAE Process", this, SLOT(kill()));
		connect(workerThread, SIGNAL(emulatorRunning(bool)), killAction, SLOT(setEnabled(bool)));
	QMenu* helpMenu = mainMenu->addMenu("Info");
		helpMenu->addAction(aboutIcon, "About WHDRun", this, SLOT(about()));
		helpMenu->addAction(aboutIcon, "About Qt", qApp, SLOT(aboutQt()));
	
	connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

	QWidget* centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);

	QString gameInfoUrl = Names::infoUrl(gameFileName);
	QLabel* gameNameLabel = new QLabel("<h3>Running: <a href='" + gameInfoUrl + "'>" + Names::get(gameFileName) + "</a></h3>");
	gameNameLabel->setOpenExternalLinks(true);
	
	m_eventLog = new QTreeWidget(centralWidget);
	QStringList eventHeaderLabels; eventHeaderLabels << "Time" << "Status";
	m_eventLog->setHeaderLabels(eventHeaderLabels);
	m_eventLog->header()->setMovable(false);
	m_eventLog->setRootIsDecorated(false);
	m_eventLog->setSelectionMode(QAbstractItemView::NoSelection);
	m_eventLog->setFocusPolicy(Qt::NoFocus);
	
	QWidget* bottom = new QWidget(this);
	bottom->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	
	QLabel* logo = new QLabel(this);
	logo->setPixmap(QPixmap(":/img/logo.png"));

	m_hideButton = new QPushButton(hideIcon, "Hide Status");
	connect(m_hideButton, SIGNAL(clicked()), SLOT(hide()));
	QPushButton* quitButton = new QPushButton(quitIcon, "Quit WHDRun");
	connect(quitButton, SIGNAL(clicked()), SLOT(quitProgram()));
	QPushButton* aboutButton = new QPushButton(aboutIcon, "About WHDRun");
	connect(aboutButton, SIGNAL(clicked()), SLOT(about()));
	
	QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
		mainLayout->addWidget(gameNameLabel);
		mainLayout->addWidget(m_eventLog);
		mainLayout->addWidget(bottom);
		QHBoxLayout* bottomLayout = new QHBoxLayout(bottom);
			bottomLayout->addWidget(logo);
			bottomLayout->addStretch();
			QVBoxLayout* buttonLayout = new QVBoxLayout();
			bottomLayout->addLayout(buttonLayout);
				buttonLayout->addWidget(m_hideButton);
				buttonLayout->addWidget(quitButton);
				buttonLayout->addStretch();
				buttonLayout->addWidget(aboutButton);
		
	setWindowTitle(__MAGE__WHDRun__NameAndVersion__ " :: Status");
	setWindowFlags(Qt::Dialog);
	
	resize(620, 420);
	
	// center window on desktop
	QDesktopWidget* desktop = QApplication::desktop();
	move((desktop->availableGeometry().width() - size().width()) / 2,
		(desktop->availableGeometry().height() - size().height()) / 2);
}

void StatusWindow::trayActivated(QSystemTrayIcon::ActivationReason reason) { if(reason == QSystemTrayIcon::Trigger) showNormal(); }

void StatusWindow::quitProgram() {
	QMessageBox::StandardButton really = QMessageBox::Yes;
	
	if(!m_done) {
		QString title = __MAGE__WHDRun__NameAndVersion__ " :: Warning";
		QString warning = "<h2>Warning: Action may cause data loss.</h2>";
		QString description = "Quitting WHDRun prematurely may result in data loss.<br>"
			"Your game (if still running) will quit and modified data will <b>not</b> be saved.<br><br>"
			"<b>Do you really want to quit?</b>";
		QString message = warning + description;	
		really = QMessageBox::warning(this, title, message, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
	}
		
	if(really == QMessageBox::Yes) {
		QCoreApplication::quit();
	}
}

void StatusWindow::about() {
		QString title = __MAGE__WHDRun__NameAndVersion__ " :: About";

		QString version = "<h2><nobr>GameBase/MAGE Amiga :: WHDRun " __MAGE__WHDRun__Version__ "</nobr></h2>";

		QString description = "<b>WHDLoad Runner for GameBase/MAGE Amiga</b><br>"
			"<nobr>(c)2007 Markus Thiele / MAGE Project</nobr><br><br>"
			"<b>WHDRun uses the following libraries:</b><ul>"
			"\t<li>Qt 4.2.1 (c)1994-2006 by <a href='http://www.trolltech.com/'>Trolltech ASA</a></li> (<a href='http://www.gnu.org/copyleft/gpl.html'>GPL</a>)"
			"\t<li>ZipArchive 3.0.1 (c)2000-2007 by <a href='http://www.artpol-software.com/'>Artpol Software</a></li> (<a href='http://www.gnu.org/copyleft/gpl.html'>GPL</a>)"
			"</ul><br>"
			"Many thanks to the creators of these great libraries.<br><br>"
			"<nobr><b>WHDRun is free software under the <a href='http://www.gnu.org/copyleft/gpl.html'>GNU General Public License</a></nobr><br>"
			"(see included <a href='whdrun-license.txt'>whdrun-license.txt</a> for details)</b>";

		QString message = version + description;
		
		QMessageBox about(QMessageBox::Information, title, message, QMessageBox::Close);
		about.setIconPixmap(QPixmap(":/img/icon.png"));
		about.exec();
}

void StatusWindow::stageEvent(QString description) {
	QStringList values;
	values << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << description;
	QTreeWidgetItem* latest = new QTreeWidgetItem(values, 0);
	latest->setIcon(1, QIcon(":/img/events/stageEvent.png"));
	m_eventLog->addTopLevelItem(latest);
	m_eventLog->resizeColumnToContents(0);
	m_eventLog->scrollToBottom();
}

void StatusWindow::infoEvent(QString description) {
	QStringList values;
	values << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << description;
	QTreeWidgetItem* latest = new QTreeWidgetItem(values, 0);
	latest->setIcon(1, QIcon(":/img/events/infoEvent.png"));
	m_eventLog->addTopLevelItem(latest);
	m_eventLog->resizeColumnToContents(0);
	m_eventLog->scrollToBottom();
}

void StatusWindow::errorEvent(QString description) {
	QStringList values;
	values << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << description;
	QTreeWidgetItem* latest = new QTreeWidgetItem(values, 0);
	latest->setIcon(1, QIcon(":/img/events/errorEvent.png"));
	m_eventLog->addTopLevelItem(latest);
	m_eventLog->resizeColumnToContents(0);
	m_eventLog->scrollToBottom();
	show();
	m_error = true;
}

void StatusWindow::kill() {
	QString title = __MAGE__WHDRun__NameAndVersion__ " :: Warning";

	QString warning = "<h2>Warning: Action may cause data loss.</h2>";
	
	QString description = "Killing WinUAE prematurely may result in data loss.<br>"
		"Data already written to disk will be saved, but data in memory will be lost.<br><br>"
		"<b>It is highly recommended that you use WHDLoad's quit key to exit the game if you wish to preserve your data.</b><br><br>"
		"<b>Do you really want to kill WinUAE?</b>";
		
	QString message = warning + description;
	
	QMessageBox::StandardButton really = QMessageBox::warning(this, title, message, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

	if(really == QMessageBox::Yes) {
		QStringList values;
		values << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "User requested killing of WinUAE process";
		QTreeWidgetItem* latest = new QTreeWidgetItem(values, 0);
		latest->setIcon(1, QIcon(":/img/events/killEvent.png"));
		m_eventLog->addTopLevelItem(latest);
		m_eventLog->resizeColumnToContents(0);
		m_eventLog->scrollToBottom();
		emit(killEmulator());
	}
}

void StatusWindow::config() {
	ConfigDialog configDialog(m_gameFileName, false, this);
	configDialog.exec();
}

void StatusWindow::finished() {
	QStringList values;
	values << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	values << QString("WHDRun is done%1").arg(m_error ? ", but there were errors" : "");
	QTreeWidgetItem* latest = new QTreeWidgetItem(values, 0);
	latest->setIcon(1, QIcon(m_error ? ":/img/events/errorFinish.png" : ":/img/events/normalFinish.png"));
	m_eventLog->addTopLevelItem(latest);
	m_eventLog->resizeColumnToContents(0);
	m_eventLog->scrollToBottom();
	if(m_error) show();
	if(!isVisible()) QCoreApplication::quit();
	m_hideAction->setEnabled(false);
	m_hideButton->setEnabled(false);
	m_trayIcon->hide();
	m_done = true;
}

void StatusWindow::closeEvent(QCloseEvent* event) { if(m_done) QCoreApplication::quit(); }

void StatusWindow::copyLog() {
	QClipboard* clipboard = QApplication::clipboard();
	QString clipboardText = __MAGE__WHDRun__NameAndVersion__ "\n";
	clipboardText += "Status log as of " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "\n";
	for(unsigned int i = 0; i < m_eventLog->topLevelItemCount(); i++) {
		QTreeWidgetItem* item = m_eventLog->topLevelItem(i);
		clipboardText += item->text(0) + "  " + item->text(1) + "\n";
	}
	clipboard->setText(clipboardText);
}
