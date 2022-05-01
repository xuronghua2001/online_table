#ifndef MULTITHREADING_H
#define MULTITHREADING_H
#include <QObject>
class MyThread : public QObject
{
  Q_OBJECT
      public:
               int id=0;
        int kkk=5;
  explicit MyThread(QObject *parent = nullptr);

  void setFlag(bool flag = true);
signals:

  void mySignal();

public slots:  void myTimeout();
private:
  bool isStop;
};
#endif // MULTITHREADING_H
