#ifndef WIDGET_H
#define WIDGET_H
#include "Multithreading.h"
#include <QWidget>
#include <QMouseEvent>
#include <QListWidgetItem>
#include <QThread>
#include <QtNetwork>
#include <QHash>
QT_BEGIN_NAMESPACE
    namespace Ui { class Widget; }
QT_END_NAMESPACE

    class Widget : public QWidget
{
  Q_OBJECT

  public:
  Widget(QWidget *parent = nullptr);

  ~Widget();
  signals:
  void startThread();

  private slots:
  void updateSlot();
  void on_listWidget_itemDoubleClicked();
  void item_medal_sort();
  void finishedSlot(QNetworkReply* r);
  void getURLImage(QNetworkReply *);
  void getmedal();
  void kuai();
  void on_spinBox_valueChanged(int arg1);
  void updateDataReadProgress(qint64,qint64);
  void shuchu(QNetworkReply *r);
  void sav(QNetworkReply* reply);
  void wear(QByteArray arr);
  void unwear();

protected:
  void enterEvent(QEnterEvent *);
  void leaveEvent(QEvent *);



private:
  void dealSignal();//处理子线程响应函数，
  void fff();
  void dealClose();//关闭线程响应
  Ui::Widget *ui;
  QNetworkAccessManager *m;
  QNetworkAccessManager *imi;
  MyThread *myT;
  QThread *thread;
  QNetworkAccessManager *manager;
  QPropertyAnimation *animation;
  QHash<QUrl, QString> url_dic;
  QHash<QString, QString> title_hash;
  QHash<QString, QString> link_hash;
  QHash<QString, QUrl> face_hash;
  QHash<QString, QPair<QTime, int>> dataHash;
  QHash<QString, QString> medal_hash;
};
#endif // WIDGET_H
