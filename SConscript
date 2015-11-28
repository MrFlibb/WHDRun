qt4 = Environment(tools=["default", "qt4"])
qt4.EnableQt4Modules(["QtCore", "QtGui"])

qobjects  = []
qobjects += qt4.Object("Names.cpp")
qobjects += qt4.Object("UserStartup.cpp")
qobjects += qt4.Object("WorkerThread.cpp")
qobjects += qt4.Object("Checksum.cpp")
qobjects += qt4.Object("KeyboardWidget.cpp")
qobjects += qt4.Object("KeyboardDialog.cpp")
qobjects += qt4.Object("StatusWindow.cpp")
qobjects += qt4.Object("UnzipProgressDialog.cpp")
qobjects += qt4.Object("ConfigWidget.cpp")
qobjects += qt4.Object("ConfigDialog.cpp")

qobjects += qt4.Object("WHDRun.cpp")

resources = qt4.RES("WHDRun_RC.rc")
qres = qt4.Qrc("resources.qrc")

binary = qt4.Program(target = "WHDRun",
	source = [qobjects, resources, qres],
	LIBS = qt4['LIBS'] + ["ziparch"],
	CCFLAGS = "-Wall -Werror",
	LINKFLAGS = "-mwindows")

qt4.AddPostAction(binary, "strip $TARGET")
