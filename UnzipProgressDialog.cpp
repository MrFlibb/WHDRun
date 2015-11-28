#include "UnzipProgressDialog.h"
#include "Version.h"

#include <QBoxLayout>
#include <QTimer>

UnzipProgressDialog::UnzipProgressDialog(QWidget* parent): QDialog(parent),
	m_active(false), m_total(0), m_progress(0) {

	QLabel* icon = new QLabel(this);
	icon->setPixmap(QPixmap(":/img/icon.png"));
	icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	
	m_message = new QLabel("<h2>Extracting game files, please stand by...</h2>");
	m_bar = new QProgressBar();
	
	QHBoxLayout* mainLayout = new QHBoxLayout(this);
		mainLayout->addWidget(icon, 0, Qt::AlignVCenter);
		mainLayout->addSpacing(16);
		QVBoxLayout* displayLayout = new QVBoxLayout();
		mainLayout->addLayout(displayLayout);
			displayLayout->addStretch();
			displayLayout->addWidget(m_message);
			displayLayout->addWidget(m_bar);
			displayLayout->addStretch();
	
	setWindowTitle(__MAGE__WHDRun__NameAndVersion__ " :: Unzipping");
	setFixedSize(500, 100);
}

void UnzipProgressDialog::start() {
	m_active = true;
	QTimer::singleShot(1000, this, SLOT(maybeShow()));
}

void UnzipProgressDialog::stop() {
	m_active = false;
	hide();
	deleteLater();
}

void UnzipProgressDialog::setTotal(unsigned long total) {
	m_total = total;
	m_bar->setRange(0, total);
}

void UnzipProgressDialog::setProgress(unsigned long progress) {
	m_progress = progress;
	m_bar->setValue(progress);
}

void UnzipProgressDialog::collecting() {
	m_bar->setRange(0, 0);
	m_bar->setValue(0);
	m_message->setText("<h2>Collecting file checksums, please stand by...</h2>");
}

void UnzipProgressDialog::maybeShow() {
	if(m_active) {
		if(m_total != 0 && ((float)m_progress / m_total) < 0.5) show();
	}
}
