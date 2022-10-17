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
  void dealClose();//关闭线程响应
  ~Widget();
  signals:
  void startThread();
  private slots:
  void on_listWidget_itemDoubleClicked();
  void on_listWidget_itemClicked();
  void finishedSlot(QNetworkReply* r);
  void getURLImage(QNetworkReply *);
  void getmedal();
  void readFile(QString path);
  void kuai();
  void on_spinBox_valueChanged(int arg1);
  void updateDataReadProgress(qint64,qint64);
  void shuchu(QNetworkReply *r);
  void sav(QNetworkReply* reply);
  void wear(QByteArray arr);
  void unwear();
signals:
  void fen();

protected:
  void enterEvent(QEnterEvent *);
  void leaveEvent(QEvent *);

private:
  Ui::Widget *ui;
  QNetworkReply *m_reply;
  QNetworkAccessManager *m;
  QNetworkAccessManager *imi;
  MyThread *myT;
  QThread *thread;
  QNetworkAccessManager *manager;
  QPropertyAnimation *animation;
};
#endif // WIDGET_H
