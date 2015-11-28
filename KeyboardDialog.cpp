/*
	WHDRun - WHDLoad Runner for GameBase/MAGE Amiga
	(c)2007 Markus Thiele / MAGE Project

	WHDRun is free software under the GNU General Public License,
	Version 2, which should be included in the file whdrun-license.txt
*/

#include "KeyboardDialog.h"
#include "Version.h"

#include <QFrame>
#include <QTabBar>
#include <QIcon>
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>

KeyboardDialog::KeyboardDialog(QWidget* parent): QDialog(parent) {
	m_keyboard = new KeyboardWidget(this);
	m_keyboard->setKeymap(KeyboardWidget::US);
	m_keyboard->setContentsMargins(0, 0, 0, 0);
	connect(m_keyboard, SIGNAL(selected(QString)), this, SIGNAL(selected(QString)));

	QFrame* keyboardFrame = new QFrame(this);
	keyboardFrame->setFrameShape(QFrame::WinPanel);
	keyboardFrame->setFrameShadow(QFrame::Sunken);
	QVBoxLayout* keyboardFrameLayout = new QVBoxLayout(keyboardFrame);
		keyboardFrameLayout->addWidget(m_keyboard);
	
	QTabBar* tabBar = new QTabBar(this);
	tabBar->addTab(QIcon(":/img/keyboard/usflag.png"), "US Keymap");
	tabBar->addTab(QIcon(":/img/keyboard/deflag.png"), "DE Keymap");
	tabBar->addTab(QIcon(":/img/keyboard/frflag.png"), "FR Keymap");
	tabBar->addTab(QIcon(":/img/keyboard/rawflag.png"), "RAW Codes");
	connect(tabBar, SIGNAL(currentChanged(int)), m_keyboard, SLOT(setKeymap(int)));

	QLabel* selectedLabel = new QLabel("<b>Selected <a href=\"http://www.whdload.de/docs/en/rawkey.html\">raw key code</a>: </b>", this);
	selectedLabel->setOpenExternalLinks(true);

	QLabel* selected = new QLabel("$--", this);
	selected->setFont(QFont("Courier New", 16, QFont::Bold));
	connect(m_keyboard, SIGNAL(selected(QString)), selected, SLOT(setText(QString)));
	
	QPushButton* close = new QPushButton("Done", this);
	connect(close, SIGNAL(clicked()), this, SLOT(accept()));
	
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->setSizeConstraint(QLayout::SetFixedSize);
		mainLayout->setSpacing(0);
		mainLayout->addWidget(tabBar);
		mainLayout->addWidget(keyboardFrame);
		mainLayout->addSpacing(8);
		QHBoxLayout* selectionLayout = new QHBoxLayout();
			mainLayout->addLayout(selectionLayout);
			selectionLayout->addWidget(selectedLabel);
			selectionLayout->addSpacing(8);
			selectionLayout->addWidget(selected);
			selectionLayout->addStretch();
			selectionLayout->addWidget(close);
	
	setModal(true);
	setWindowTitle(__MAGE__WHDRun__NameAndVersion__ " :: WHDLoad Keyboard");
}
