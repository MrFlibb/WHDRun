/*
	WHDRun - WHDLoad Runner for GameBase/MAGE Amiga
	(c)2007 Markus Thiele / MAGE Project

	WHDRun is free software under the GNU General Public License,
	Version 2, which should be included in the file whdrun-license.txt
*/

#include "ConfigDialog.h"
#include "ConfigWidget.h"
#include "Names.h"
#include "Version.h"

#include <QBoxLayout>
#include <QCoreApplication>
#include <QLabel>
#include <QPushButton>
#include <QSettings>	

ConfigDialog::ConfigDialog(QString gameFileName, QString slaveName, bool atStartup, QWidget* parent): QDialog(parent) {

	// m_baseKey = Names::baseName(gameFileName).toLower(); // base names aren't unique
	m_baseKey = slaveName.toLower();
	
	QString appPath = QCoreApplication::applicationDirPath();
	m_dataPath = appPath + "/whdrun-data"; // default
	QSettings globalSettings(appPath + "/WHDRun.ini", QSettings::IniFormat);
	if(globalSettings.value("WHDRun/dataPath.useCustom", false).toBool()) {
		m_dataPath = globalSettings.value("WHDRun/dataPath", m_dataPath).toString();
	}

	m_iniPath = m_dataPath + "/__WHDRun__Games.ini";

	
	QLabel* logo = new QLabel(this);
	if(atStartup) logo->setPixmap(QPixmap(":/img/vlogo.png"));
	logo->setAlignment(Qt::AlignHCenter);
	
	ConfigWidget* configWidget = new ConfigWidget(gameFileName, slaveName, this);
	
	QWidget* bottom = new QWidget(this);
	bottom->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	
	QCheckBox* gameNeverAgain = new QCheckBox("Don't show at startup for this game", this);
	initGameNeverAgain(gameNeverAgain);
	connect(gameNeverAgain, SIGNAL(stateChanged(int)), SLOT(setGameNeverAgain(int)));
	
	QCheckBox* globalNeverAgain = new QCheckBox("Don't ever show at startup", this);
	initGlobalNeverAgain(globalNeverAgain);
	connect(globalNeverAgain, SIGNAL(stateChanged(int)), SLOT(setGlobalNeverAgain(int)));
	
	QPushButton* startButton = new QPushButton(atStartup ? "Start Game" : "Close", this);
	startButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
	connect(startButton, SIGNAL(clicked()), SLOT(accept()));
	
	QHBoxLayout* mainLayout = new QHBoxLayout(this);
		//mainLayout->setSizeConstraint(QLayout::SetFixedSize);
		if(atStartup) mainLayout->addWidget(logo, 0, 0);
		QVBoxLayout* rightLayout = new QVBoxLayout();
		mainLayout->addLayout(rightLayout);
			rightLayout->addWidget(new QLabel("<h2><center>WHDLoad Slave Configuration</center></h2>", this));
			if(!atStartup) {
				rightLayout->addWidget(new QLabel("<center><b>Note: </b>Changes won't take effect until next start.</center>"));
			}
			rightLayout->addWidget(configWidget);
			rightLayout->addStretch();
			rightLayout->addWidget(bottom);
			QHBoxLayout* bottomLayout = new QHBoxLayout(bottom);
				QVBoxLayout* futureLayout = new QVBoxLayout();
				bottomLayout->addLayout(futureLayout);
					futureLayout->addWidget(gameNeverAgain);
					futureLayout->addWidget(globalNeverAgain);
				bottomLayout->addStretch();
				bottomLayout->addWidget(startButton);
		
	setWindowTitle(__MAGE__WHDRun__NameAndVersion__ " :: WHDLoad Slave Configuration");
	
	startButton->setFocus(Qt::OtherFocusReason);
}

void ConfigDialog::setGameNeverAgain(int state) {
	QSettings ini(m_iniPath, QSettings::IniFormat);
	ini.setValue(m_baseKey + "/neverAgain", state == Qt::Unchecked ? 0 : 1);
}

void ConfigDialog::initGameNeverAgain(QCheckBox* checkBox) {
	QSettings ini(m_iniPath, QSettings::IniFormat);
	int state = ini.value(m_baseKey + "/neverAgain", 0).toInt();
	checkBox->setCheckState(state == 0 ? Qt::Unchecked : Qt::Checked);
}

void ConfigDialog::setGlobalNeverAgain(int state) {
	QSettings ini(m_iniPath, QSettings::IniFormat);
	ini.setValue("global/neverAgain", state == Qt::Unchecked ? 0 : 1);
}

void ConfigDialog::initGlobalNeverAgain(QCheckBox* checkBox) {
	QSettings ini(m_iniPath, QSettings::IniFormat);
	int state = ini.value("global/neverAgain", 0).toInt();
	checkBox->setCheckState(state == 0 ? Qt::Unchecked : Qt::Checked);
}

void ConfigDialog::maybeExec() {
	QSettings ini(m_iniPath, QSettings::IniFormat);
	int globalNeverAgain = ini.value("global/neverAgain", 0).toInt();
	int gameNeverAgain = ini.value(m_baseKey + "/neverAgain", 0).toInt();
	if(globalNeverAgain != 1 && gameNeverAgain != 1) {
		exec();
	}
}
