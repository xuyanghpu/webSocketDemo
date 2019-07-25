#include "webSocketDemo.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
 	webSocketDemo w;
	w.StartServer();
 	w.show();
	return a.exec();
}
