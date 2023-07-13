#ifndef WIDGET_H
#define WIDGET_H
#include "Multithreading.h"
#include "himitsu.h"
#include <QWidget>
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
  void dealSignal();//处理子线程响应函数，
  void fff();
  void dealClose();//关闭线程响应
  ~Widget();
  signals:
  void startThread();
  void updateSignal();
  private slots:
  void updateSlot();
  void on_listWidget_itemDoubleClicked();
  void on_listWidget_itemClicked();
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
signals:
  void fen();

protected:
  void enterEvent(QEnterEvent *);
  void leaveEvent(QEvent *);

private:
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
};
#endif // WIDGET_H
