#ifndef WIDGET_H
#define WIDGET_H
#include "Multithreading.h"
#include <QWidget>
#include <QListWidgetItem>
#include <QThread>
#include <QtNetwork>
QT_BEGIN_NAMESPACE
    namespace Ui { class Widget; }
QT_END_NAMESPACE

    class Widget : public QWidget
{
  Q_OBJECT

  public:
  Widget(QWidget *parent = nullptr);
  void dealSignal();//处理子线程响应函数，
  void fff();
  void elle(QNetworkRequest,QVariant var);
  void dealClose();
 //   void dealClose();//关闭线程响应
  ~Widget();
  signals:
  void startThread();
  private slots:
  void on_listWidget_itemDoubleClicked();
  void finishedSlot();
  void getURLImage();
  void readFile(QString path);
  //  void fen();
private:
  Ui::Widget *ui;
  QNetworkReply *m_reply;
  MyThread *myT;
  QThread *thread;
};
#endif // WIDGET_H
