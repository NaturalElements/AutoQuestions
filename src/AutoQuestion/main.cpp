#include "AutoQuestionWindow.h"
#include <QtWidgets/qapplication.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CAutoQuestionWindow w;
	w.show();
	return a.exec();
}
