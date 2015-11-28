/*
	WHDRun - WHDLoad Runner for GameBase/MAGE Amiga
	(c)2007 Markus Thiele / MAGE Project

	WHDRun is free software under the GNU General Public License,
	Version 2, which should be included in the file whdrun-license.txt
*/

#include "KeyboardWidget.h"

#include <QTextStream>

#include <QPainter>
#include <QMouseEvent>

KeyboardWidget::KeyboardWidget(QWidget* parent):
	QWidget(parent) {
	
	setAttribute(Qt::WA_StaticContents);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
	
	m_usKeymap = QImage(":/img/keyboard/keyus.png");
	m_deKeymap = QImage(":/img/keyboard/keyde.png");
	m_frKeymap = QImage(":/img/keyboard/keyfr.png");
	m_rawKeymap = QImage(":/img/keyboard/keyraw.png");
	m_codes = QImage(":/img/keyboard/codes.png");
	
	m_currentKeymap = &m_usKeymap;
}

QSize KeyboardWidget::sizeHint() const {
	return QSize(m_codes.size());
}

void KeyboardWidget::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	painter.drawImage(0, 0, *m_currentKeymap);
}

void KeyboardWidget::mousePressEvent(QMouseEvent* event) {
	if(event->button() == Qt::LeftButton) emitKey(event->pos());
}

void KeyboardWidget::mouseMoveEvent(QMouseEvent* event) {
	if(event->buttons() & Qt::LeftButton) emitKey(event->pos());
}

void KeyboardWidget::emitKey(QPoint pos) {
	if(!m_codes.rect().contains(pos)) return;
	unsigned char key = qRed(m_codes.pixel(pos));
	if(key != 0xFF) {
		emit(selected(QString("$%1").arg(key, 2, 16, QChar('0')).toUpper()));
	}
}

void KeyboardWidget::setKeymap(int layout) {
	switch(layout) {
		case US: m_currentKeymap = &m_usKeymap; break;
		case DE: m_currentKeymap = &m_deKeymap; break;
		case FR: m_currentKeymap = &m_frKeymap; break;
		default: m_currentKeymap = &m_rawKeymap; break;
	}
	repaint();
}
