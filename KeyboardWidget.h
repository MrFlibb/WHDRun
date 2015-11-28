#ifndef __WHDRun__KeyboardWidget__H
#define __WHDRun__KeyboardWidget__H

#include <QString>
#include <QWidget>

class KeyboardWidget: public QWidget {
	Q_OBJECT
	
	public:
		enum Keymap { US=0, DE=1, FR=2, RAW=3 };
		KeyboardWidget(QWidget* parent = NULL);
		QSize sizeHint() const;
		
	public slots:
		void setKeymap(int layout);
		
	signals:
		void selected(QString);
		
	protected:
		void paintEvent(QPaintEvent* event);
		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void emitKey(QPoint pos);
	
	private:
		QImage m_usKeymap;
		QImage m_deKeymap;
		QImage m_frKeymap;
		QImage m_rawKeymap;
		QImage m_codes;
		
		QImage* m_currentKeymap;
	
};

#endif
