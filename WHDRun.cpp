/*
	WHDRun - WHDLoad Runner for GameBase/MAGE Amiga
	(c)2007 Markus Thiele / MAGE Project

	WHDRun is free software under the GNU General Public License,
	Version 2, which should be included in the file whdrun-license.txt
*/

#include <QApplication>

#include <QClipboard>
#include <QIcon>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include <QStringList>

#ifndef NDEBUG
#	include <QErrorMessage>
#endif

#include "ConfigDialog.h"
#include "StatusWindow.h"
#include "UserStartup.h"
#include "Version.h"
#include "WorkerThread.h"

int main(int argc, char* argv[]) {
	QApplication WHDRun(argc, argv);
	WHDRun.setQuitOnLastWindowClosed(false);
	WHDRun.setWindowIcon(QIcon(":/img/trayicon.png"));
	
	QStringList arguments = WHDRun.arguments();
	
	#ifndef NDEBUG
		QErrorMessage* internalError = QErrorMessage::qtHandler();
		internalError->setWindowTitle("WHDRun " __MAGE__WHDRun__Version__ " :: Internal Error");
	#endif
	
	if(arguments.size() < 6) {
		QString title = __MAGE__WHDRun__NameAndVersion__ " :: Error";
	
		QString error = "<h2><b>Fatal error</b>: Too few arguments.</h2>";
	
		QString description = "<b>WHDRun must be called with exactly 5 arguments:</b><ul>"
			"<li><nobr>whdrun.exe &lt;<i>GamePack.zip</i>&gt; &lt;<i>TempDir</i>/&gt; &lt;<i>WinUAE.exe</i>&gt; &lt;<i>GameBase Amiga.uae</i>&gt; &lt;<i>user-startup</i>&gt;</nobr></li></ul>"
			"<b>If you get this error while starting a game from GameBase/MAGE Amiga, "
			"please report to Belgarath or eLowar (remember which game you were "
			"trying to start, please) at</b></font><ul>"
			"<li>http://eab.abime.net/ (<a href='http://eab.abime.net/forumdisplay.php?f=76'>project.MAGE sub-forum</a>) or</li>"
			"<li>in the IRC channel #mage on <a href='http://irc.abime.net/'>irc.abime.net</a> or</li>"
			"<li>by e-mail to <a href='mailto:mage@elowar.com'>mage@elowar.com</a></li></ul>";
			
		QString debugInfo = "<b>Debug information:</b>\n<ul>\n";
		debugInfo += QString("\t<li>%1 arguments given</li>\n").arg(arguments.size() - 1);
		for(unsigned int i = 1; i < arguments.size(); i++) {
			debugInfo += QString("\t<li>Argument %1: %2</li>\n").arg(i).arg(arguments[i]);
		}
		debugInfo += "</ul>\n";
		
		QString message = error + description + debugInfo;
		message += "<b>The program will end after this dialog is closed.</b>";
		
		QMessageBox errorBox(QMessageBox::Critical, title, message);
		QPushButton* ctcButton = errorBox.addButton(" C&opy to Clipboard && Close ", QMessageBox::AcceptRole);
		QPushButton* closeButton = errorBox.addButton(" &Close ",  QMessageBox::AcceptRole);
		errorBox.setEscapeButton(closeButton);
		errorBox.setDefaultButton(closeButton);
		
		errorBox.exec();
		
		if(errorBox.clickedButton() == ctcButton) {
			QClipboard* clipboard = QApplication::clipboard();
			QString clipboardText = __MAGE__WHDRun__NameAndVersion__ "\n";
			clipboardText += "Fatal error: Too few arguments.\nDebug information:\n";
			clipboardText += QString("-  %1 arguments given\n").arg(arguments.size() - 1);
			for(unsigned int i = 1; i < arguments.size(); i++) {
				clipboardText += QString("-  Argument %1: %2\n").arg(i).arg(arguments[i]);
			}
			
			clipboard->setText(clipboardText);
		}
	
		return -1;
	}
	
	QString gameFileName = arguments[1];

	{ // limited life time
		UserStartup userStartup(arguments[5]);
		QString slaveName = userStartup.slaveName();
		if(slaveName.trimmed().isEmpty()) {
			QString title = __MAGE__WHDRun__NameAndVersion__ " :: Error";
			QString error = "<h2><b>Fatal error</b>: Couldn't determine slave name.</h2>";
			QString message = "WHDRun could not determine the WHDLoad slave name from the specified user-startup file"
				"(" + arguments[5] + ").";
			QMessageBox::critical(NULL, title, error + message);
			return -1;
		}
		ConfigDialog configDialog(gameFileName, slaveName);
		configDialog.maybeExec();
	}
	
	WorkerThread workerThread;
	workerThread.setGameFileName(gameFileName);
	workerThread.setTempPath(arguments[2]);
	workerThread.setEmuFileName(arguments[3]);
	workerThread.setConfigFileName(arguments[4]);
	workerThread.setStartupFileName(arguments[5]);

	StatusWindow statusWindow(&workerThread, gameFileName);
	
	QString appPath = QCoreApplication::applicationDirPath();
	QString dataPath = appPath + "/whdrun-data"; // default
	QSettings settings(appPath + "/WHDRun.ini", QSettings::IniFormat);
	if(settings.value("WHDRun/dataPath.useCustom", false).toBool()) {
		dataPath = settings.value("WHDRun/dataPath", dataPath).toString();
	}
	workerThread.setDataPath(dataPath);
	
	workerThread.start();
	
	return WHDRun.exec();
}
