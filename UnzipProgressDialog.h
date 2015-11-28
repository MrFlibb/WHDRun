#ifndef __WHDRun__UnzipProgressDialog__H
#define __WHDRun__UnzipProgressDialog__H

#include <QDialog>
#include <QLabel>
#include <QProgressBar>

class UnzipProgressDialog: public QDialog {
	Q_OBJECT

	public:
		UnzipProgressDialog(QWidget* parent = NULL);
	
	public slots:
		void start();
		void stop();
		
		void setTotal(unsigned long total);
		void setProgress(unsigned long progress);
		void collecting();
		
		void maybeShow();
		
	private:
		bool m_active;
		unsigned long m_total;
		unsigned long m_progress;
		QProgressBar* m_bar;
		QLabel* m_message;
};

#endif
