#include "widget.h"
//#include "QBreakpadHandler.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
//  QBreakpadInstance.setDumpPath("crashes");
  Widget w;
  w.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
  w.show();
  return a.exec();
}
