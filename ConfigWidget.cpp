/*
	WHDRun - WHDLoad Runner for GameBase/MAGE Amiga
	(c)2007 Markus Thiele / MAGE Project

	WHDRun is free software under the GNU General Public License,
	Version 2, which should be included in the file whdrun-license.txt
*/

#include "ConfigWidget.h"
#include "KeyboardDialog.h"
#include "Names.h"
#include "Version.h"

#include <QBoxLayout>
#include <QCoreApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QIntValidator>
#include <QPushButton>
#include <QRegExpValidator>
#include <QSettings>

ConfigWidget::ConfigWidget(QString gameFileName, QString slaveName, QWidget* parent): QWidget(parent) {
	
	// m_baseKey = Names::baseName(gameFileName).toLower(); // base names aren't unique
	m_baseKey = slaveName.toLower();
	
	QString appPath = QCoreApplication::applicationDirPath();
	m_dataPath = appPath + "/whdrun-data"; // default
	QSettings globalSettings(appPath + "/WHDRun.ini", QSettings::IniFormat);
	if(globalSettings.value("WHDRun/dataPath.useCustom", false).toBool()) {
		m_dataPath = globalSettings.value("WHDRun/dataPath", m_dataPath).toString();
	}

	m_iniPath = m_dataPath + "/__WHDRun__Games.ini";

	QString gameInfoUrl = Names::infoUrl(gameFileName);
	QLabel* gameInfoLabel = new QLabel("<a href='" + gameInfoUrl + "'>" + gameInfoUrl + "</a>");
	gameInfoLabel->setOpenExternalLinks(true);
	
	QGroupBox* generalBox = new QGroupBox("General Information", this);
	QGridLayout* generalLayout = new QGridLayout(generalBox);
		generalLayout->addWidget(new QLabel("Game Name:"), 0, 0, Qt::AlignLeft);
		generalLayout->addWidget(new QLabel(Names::get(gameFileName)), 0, 1, Qt::AlignLeft);
		generalLayout->addWidget(new QLabel("Info URL:"), 1, 0, Qt::AlignLeft);
		generalLayout->addWidget(gameInfoLabel, 1, 1, Qt::AlignLeft);
		generalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding), 0, 2);

	QCheckBox* useCustomQuitKey = new QCheckBox("Custom quit key:");
	useCustomQuitKey->setProperty("key", "quitKey.useCustom");
	initSwitch(useCustomQuitKey);
	connect(useCustomQuitKey, SIGNAL(stateChanged(int)), SLOT(setSwitch(int)));
	
	QLabel* quitKey = new QLabel(this);
	quitKey->setProperty("key", "quitKey");
	quitKey->setFont(QFont("Courier New"));
	initKey(quitKey, "$4B");
		
	QPushButton* selectQuitKey = new QPushButton("select", this);
	KeyboardDialog* quitKeyboard = new KeyboardDialog(this);
	quitKeyboard->setProperty("key", "quitKey");
	connect(selectQuitKey, SIGNAL(clicked()), quitKeyboard, SLOT(exec()));
	connect(quitKeyboard, SIGNAL(selected(QString)), this, SLOT(setKey(QString)));
	connect(quitKeyboard, SIGNAL(selected(QString)), quitKey, SLOT(setText(QString)));
	
	QGroupBox* keyBox = new QGroupBox("Key Configuration", this);
	QGridLayout* keyLayout = new QGridLayout(keyBox);
		keyLayout->addWidget(useCustomQuitKey, 0, 0);
		keyLayout->addWidget(quitKey, 0, 2);
		keyLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding), 0, 3);
		keyLayout->addWidget(selectQuitKey, 0, 4);

	QCheckBox* useCustomCustom = new QCheckBox("CUSTOM:");
	useCustomCustom->setProperty("key", "CUSTOM.useCustom");
	initSwitch(useCustomCustom);
	connect(useCustomCustom, SIGNAL(stateChanged(int)), SLOT(setSwitch(int)));
	
	QLineEdit* custom = new QLineEdit(this);
	custom->setValidator(new QRegExpValidator(QRegExp("[^\"]*"), custom));
	custom->setProperty("key", "CUSTOM");
	initLine(custom, "");
	connect(custom, SIGNAL(textEdited(QString)), SLOT(setLine(QString)));
	
	QCheckBox* useCustomCustom1 = new QCheckBox("CUSTOM1:");
	useCustomCustom1->setProperty("key", "CUSTOM1.useCustom");
	initSwitch(useCustomCustom1);
	connect(useCustomCustom1, SIGNAL(stateChanged(int)), SLOT(setSwitch(int)));
	
	QLineEdit* custom1 = new QLineEdit(this);
	custom1->setValidator(new QIntValidator(0, 0xFFFF, custom1)); // TODO: allow 32 bit?
	custom1->setProperty("key", "CUSTOM1");
	initLine(custom1, "0");
	connect(custom1, SIGNAL(textEdited(QString)), SLOT(setLine(QString)));
	
	QCheckBox* useCustomCustom2 = new QCheckBox("CUSTOM2:");
	useCustomCustom2->setProperty("key", "CUSTOM2.useCustom");
	initSwitch(useCustomCustom2);
	connect(useCustomCustom2, SIGNAL(stateChanged(int)), SLOT(setSwitch(int)));
	
	QLineEdit* custom2 = new QLineEdit(this);
	custom2->setValidator(new QIntValidator(0, 0xFFFF, custom2)); // TODO: allow 32 bit?
	custom2->setProperty("key", "CUSTOM2");
	initLine(custom2, "0");
	connect(custom2, SIGNAL(textEdited(QString)), SLOT(setLine(QString)));
	
	QCheckBox* useCustomCustom3 = new QCheckBox("CUSTOM3:");
	useCustomCustom3->setProperty("key", "CUSTOM3.useCustom");
	initSwitch(useCustomCustom3);
	connect(useCustomCustom3, SIGNAL(stateChanged(int)), SLOT(setSwitch(int)));
	
	QLineEdit* custom3 = new QLineEdit(this);
	custom3->setValidator(new QIntValidator(0, 0xFFFF, custom3)); // TODO: allow 32 bit?
	custom3->setProperty("key", "CUSTOM3");
	initLine(custom3, "0");
	connect(custom3, SIGNAL(textEdited(QString)), SLOT(setLine(QString)));
	
	QCheckBox* useCustomCustom4 = new QCheckBox("CUSTOM4:");
	useCustomCustom4->setProperty("key", "CUSTOM4.useCustom");
	initSwitch(useCustomCustom4);
	connect(useCustomCustom4, SIGNAL(stateChanged(int)), SLOT(setSwitch(int)));
	
	QLineEdit* custom4 = new QLineEdit(this);
	custom4->setValidator(new QIntValidator(0, 0xFFFF, custom4)); // TODO: allow 32 bit?
	custom4->setProperty("key", "CUSTOM4");
	initLine(custom4, "0");
	connect(custom4, SIGNAL(textEdited(QString)), SLOT(setLine(QString)));
	
	QCheckBox* useCustomCustom5 = new QCheckBox("CUSTOM5:");
	useCustomCustom5->setProperty("key", "CUSTOM5.useCustom");
	initSwitch(useCustomCustom5);
	connect(useCustomCustom5, SIGNAL(stateChanged(int)), SLOT(setSwitch(int)));
	
	QLineEdit* custom5 = new QLineEdit(this);
	custom5->setValidator(new QIntValidator(0, 0xFFFF, custom5)); // TODO: allow 32 bit?
	custom5->setProperty("key", "CUSTOM5");
	initLine(custom5, "0");
	connect(custom5, SIGNAL(textEdited(QString)), SLOT(setLine(QString)));
	
	QGroupBox* customBox = new QGroupBox("Custom Value Configuration", this);
	QGridLayout* customLayout = new QGridLayout(customBox);
		customLayout->addWidget(useCustomCustom, 0, 0);
		customLayout->addWidget(custom, 0, 1);
		customLayout->addWidget(useCustomCustom1, 1, 0);
		customLayout->addWidget(custom1, 1, 1);
		customLayout->addWidget(useCustomCustom2, 2, 0);
		customLayout->addWidget(custom2, 2, 1);
		customLayout->addWidget(useCustomCustom3, 3, 0);
		customLayout->addWidget(custom3, 3, 1);
		customLayout->addWidget(useCustomCustom4, 4, 0);
		customLayout->addWidget(custom4, 4, 1);
		customLayout->addWidget(useCustomCustom5, 5, 0);
		customLayout->addWidget(custom5, 5, 1);
	
	
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->addWidget(generalBox);
		mainLayout->addWidget(keyBox);
		mainLayout->addWidget(customBox);
		mainLayout->addStretch();

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}

void ConfigWidget::setSwitch(int state) {
	if(sender() == NULL) return;
	QSettings ini(m_iniPath, QSettings::IniFormat);
	ini.setValue(m_baseKey + "/" + sender()->property("key").toString(), state == Qt::Unchecked ? 0 : 1);
}

void ConfigWidget::initSwitch(QCheckBox* checkBox) {
	QSettings ini(m_iniPath, QSettings::IniFormat);
	int state = ini.value(m_baseKey + "/" + checkBox->property("key").toString(), 0).toInt();
	checkBox->setCheckState(state == 0 ? Qt::Unchecked : Qt::Checked);
}

void ConfigWidget::setKey(QString key) {
	if(sender() == NULL) return;
	QSettings ini(m_iniPath, QSettings::IniFormat);
	ini.setValue(m_baseKey + "/" + sender()->property("key").toString(), key);
}

void ConfigWidget::initKey(QLabel* label, QString defaultValue) {
	QSettings ini(m_iniPath, QSettings::IniFormat);
	QString value = ini.value(m_baseKey + "/" + label->property("key").toString(), defaultValue).toString();
	label->setText(value);
}

void ConfigWidget::setLine(QString value) {
	if(sender() == NULL) return;
	if(value.isEmpty()) value = sender()->property("default").toString();
	QSettings ini(m_iniPath, QSettings::IniFormat);
	ini.setValue(m_baseKey + "/" + sender()->property("key").toString(), value);
}

void ConfigWidget::initLine(QLineEdit* lineEdit, QString defaultValue) {
	QSettings ini(m_iniPath, QSettings::IniFormat);
	QString value = ini.value(m_baseKey + "/" + lineEdit->property("key").toString(), defaultValue).toString();
	lineEdit->setProperty("default", defaultValue);
	lineEdit->setText(value);
}
