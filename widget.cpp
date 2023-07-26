#include "widget.h"
#include "himitsu.h"
#include "ui_widget.h"
#include <QApplication>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDebug>
#include <QString>
#include <QJsonDocument>
#include <QFont>
#include <QDateTime>
#include <QMessageBox>
#include <QSize>
QHash<QString, QString> hash;
QHash<QString,QImage> ima;
QHash<QByteArray,QByteArray>nv;
static int shenmeshenme=0;
static int fight=1,tot=0,val=0;
QByteArray bytes;
QStringList g_list;
QSet <QString> g_set;

Widget::Widget(QWidget *parent)
    : QWidget(parent,Qt::Widget| Qt::WindowStaysOnTopHint)
      , ui(new Ui::Widget)
{
  ui->setupUi(this);
  myT = new MyThread;
  thread = new QThread(this);
  myT->moveToThread(thread);
  manager = new QNetworkAccessManager(this);
  m = new QNetworkAccessManager(this);
  imi = new QNetworkAccessManager(this);
  connect(m, SIGNAL(finished(QNetworkReply*)),this, SLOT(sav(QNetworkReply*)));
  connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(finishedSlot(QNetworkReply*)));
//  connect(imi, SIGNAL(finished(QNetworkReply *)),this, SLOT(getURLImage(QNetworkReply *)));
  connect(myT, &MyThread::mySignal, this, &Widget::dealSignal);
//  connect(this, &Widget::updateSignal, this, &Widget::updateSlot);
  connect(this, &Widget::startThread,myT, &MyThread::myTimeout);
  connect(imi, SIGNAL(finished(QNetworkReply*)),this, SLOT(getURLImage(QNetworkReply*)));


  if(himitsu("bilibili.com",&nv))
  {
    QString msg = "请使用Edge浏览器<a href='https://www.bilibili.com/'>登录bilibili\n</a>" ;
    QMessageBox::information(nullptr,"提示",msg);
    exit(0);
  }

  QFont list_font;
  list_font.setPointSize(20);
  ui->label->setFont(list_font);
  ui->listWidget->setFont(list_font);
  fff();
//  qDebug()<<nv;
  QList<QNetworkCookie> allcookies;
  QHash<QByteArray, QByteArray>::iterator i = nv.begin();
  while (i != nv.end()) {
      allcookies.append(QNetworkCookie(i.key(),i.value()));
    ++i;
    }
  QNetworkCookieJar *jar = new QNetworkCookieJar();
  jar->setCookiesFromUrl(allcookies, QUrl("https://api.vc.bilibili.com/dynamic_svr/v1/dynamic_svr/w_live_users?size=100"));
  jar->setCookiesFromUrl(allcookies, QUrl("https://api.bilibili.com/x/polymer/web-dynamic/v1/portal"));
  manager->setCookieJar(jar);



}

void Widget::fff()
{
  thread->start();
  emit startThread();
}

void Widget::updateSlot()

{
  ui->listWidget->clear();
  ui->listWidget->addItems(g_list);
  for(int t=0;t<g_list.size();t++)
    ui->listWidget->item(t)->setToolTip(title_hash[ui->listWidget->item(t)->text()]+"\n打开"+ui->listWidget->item(t)->text()+"的直播间");
  kuai();

}

void Widget::dealSignal()
{
  QNetworkRequest req;
  req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);
  req.setRawHeader("User-Agent","Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:106.0) Gecko/20100101 Firefox/106.0");
  req.setUrl(QUrl("https://api.vc.bilibili.com/dynamic_svr/v1/dynamic_svr/w_live_users?size=100"));
  manager->get(req);
}
void Widget::updateDataReadProgress(qint64 r,qint64 t)
{
  QTime current_time =QTime::currentTime();
  QString now=current_time.toString();
  double dd=100*(double)r/t;
  QString bai=QString::number(dd,'f',5);
  QString tule=QString::number(ui->listWidget->count());
  ui->textEdit->setText(now+"  正在加载图片..."+tule+'/'+QString::number(shenmeshenme+1)+"..."+bai+"%");
}

void Widget::kuai()
{
  for (int i = 0;i<ui->listWidget->count();i++)
  {
    shenmeshenme = i;
    if(ima.contains(ui->listWidget->item(i)->text()))
    {
      QImage q;
      QHash<QString, QImage>::iterator j;
      j = ima.find(ui->listWidget->item(i)->text());
      while (j != ima.end()&&j.key()==ui->listWidget->item(i)->text()) {
      q=j.value();
      ++j;}
      ui->listWidget->item(i)->setIcon(QPixmap::fromImage(q));
    }
    else
    {
      imi->get(QNetworkRequest(face_hash[ui->listWidget->item(i)->text()]));}
    }

}

void Widget::shuchu(QNetworkReply *r)
{
  QVariant statusCode = r->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  if(statusCode.isValid())
  {
    m->disconnect(m,SIGNAL(finished(QNetworkReply *)),this,SLOT(shuchu(QNetworkReply *)));
    qDebug() << " status code=" << statusCode.toInt();
  }
  QByteArray bytes = r->readAll();
  r->deleteLater();
  QJsonParseError e;
  QString mes;
  QJsonDocument doc = QJsonDocument::fromJson(bytes,&e);
  if(e.error==0)
  {QJsonObject obj = doc.object();
  QJsonValue message=obj.value("message");
  mes=message.toString();}
  else
    mes="无法连接到互联网";
  //qDebug()<<mes;
  QTime current_time =QTime::currentTime();
  QString t=current_time.toString();
  ui->textEdit->insertPlainText(t+"  "+mes+'\n');
}
void Widget::getmedal()
{
  tot = -1;
  if(fight>0)
  {
    QNetworkRequest reqg;
    QList<QNetworkCookie> cookies;
    cookies.append(QNetworkCookie("SESSDATA",nv.find("SESSDATA").value()));
    cookies.append(QNetworkCookie("bili_jct",nv.find("bili_jct").value()));
    //reqg.setUrl(QUrl("https://api.live.bilibili.com/fans_medal/v1/FansMedal/get_list_in_room"));
    reqg.setUrl(QUrl("https://api.live.bilibili.com/fans_medal/v2/HighQps/received_medals?page="+QString::number(fight)));
    QNetworkCookieJar *jar = new QNetworkCookieJar();
    //jar->setCookiesFromUrl(cookies, QUrl("https://api.live.bilibili.com/fans_medal/v1/FansMedal/get_list_in_room"));
    jar->setCookiesFromUrl(cookies, QUrl("https://api.live.bilibili.com/fans_medal/v2/HighQps/received_medals?page="+QString::number(fight)));
    m->setCookieJar(jar);
    m->get(reqg);
  }
}
void Widget::sav(QNetworkReply* reply)
{
  if (reply->error()!=QNetworkReply::NoError)
  {
    tot=0;
    qDebug()<<reply->error();
  }
  else
  {
  QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  if(statusCode.isValid())
  {
    qDebug() << "save status code=" << statusCode.toInt();
  }
    QString string=nullptr;
    QByteArray b=reply->readAll();
    string =QString::fromUtf8(b,b.size());
    QJsonParseError e;
    QJsonDocument d = QJsonDocument::fromJson(string.toStdString().data(),&e);
  if(e.error==0)
    {
    QJsonObject obj=d.object();
    QJsonValue value1=obj.value("data");
    QJsonObject obj2=value1.toObject();
    QJsonValue value2 = obj2.value("list");
    QJsonValue value3 = obj2.value("total_page");
    tot=value3.toInt();
    QJsonArray ja=value2.toArray();
  for(int ii=0;ii<ja.count();ii++)
    {
    QJsonObject arrObj= ja[ii].toObject();
    QString s=arrObj["target_name"].toString();
    QString medal_id=QString::number(arrObj["medal_id"].toInt());
    //qDebug()<<medal_id;
    hash.insert(s,medal_id);
    }
    if(fight<tot)
      {fight++;qDebug()<<fight;getmedal();}
    else if(fight>=tot && tot>0)
      {
//       qDebug()<<hash;
      m->disconnect(m, SIGNAL(finished(QNetworkReply*)),this, SLOT(sav(QNetworkReply*)));
      }
     }
  qDebug()<<hash.count();
  }
  reply->deleteLater();
}
void Widget::wear(QByteArray arr)
{
  QNetworkRequest req;
  QList<QNetworkCookie> allcookies;
  allcookies.append(QNetworkCookie("SESSDATA",nv.find("SESSDATA").value()));
  allcookies.append(QNetworkCookie("bili_jct",nv.find("bili_jct").value()));
  QNetworkCookieJar *jar = new QNetworkCookieJar();
  jar->setCookiesFromUrl(allcookies, QUrl("https://api.live.bilibili.com/xlive/web-room/v1/fansMedal/wear"));
  m->setCookieJar(jar);
  QByteArray dataArray="medal_id="+arr+"&csrf_token="+nv.find("bili_jct").value()+"&csrf="+nv.find("bili_jct").value();
  int contentLength=dataArray.size();
  req.setUrl(QUrl("https://api.live.bilibili.com/xlive/web-room/v1/fansMedal/wear"));
  req.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
  req.setHeader(QNetworkRequest::ContentLengthHeader,contentLength);
  m->post(req,dataArray);
  connect(m,SIGNAL(finished(QNetworkReply*)),this,SLOT(shuchu(QNetworkReply*)));
}

void Widget::unwear()
{
  QNetworkRequest req;
  QList<QNetworkCookie> allcookies;
  allcookies.append(QNetworkCookie("SESSDATA",nv.find("SESSDATA").value()));
  QNetworkCookieJar *jar = new QNetworkCookieJar();
  jar->setCookiesFromUrl(allcookies, QUrl("https://api.live.bilibili.com/xlive/web-room/v1/fansMedal/take_off"));
  m->setCookieJar(jar);
  QByteArray dataArray="csrf_token="+nv.find("bili_jct").value()+"&csrf="+nv.find("bili_jct").value();
  int contentLength=dataArray.size();
  req.setUrl(QUrl("https://api.live.bilibili.com/xlive/web-room/v1/fansMedal/take_off"));
  req.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
  req.setHeader(QNetworkRequest::ContentLengthHeader,contentLength);
  m->post(req,dataArray);
  connect(m,SIGNAL(finished(QNetworkReply*)),this,SLOT(shuchu(QNetworkReply*)));
}

void Widget::finishedSlot(QNetworkReply* r)
{
  QByteArray bytes;
  if(r->error()==0)
  bytes = r->readAll();
  else
  {
  QTime current_time =QTime::currentTime();
  QString t=current_time.toString();
  QVariant statusCode = r->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  if(statusCode.isValid())
  {
      qDebug() << "status code=" << statusCode.toInt();
      t+="   "+statusCode.toString();
  }

  ui->textEdit->insertPlainText(t+"   "+r->errorString()+"\n");
  }
  if (tot==0)
  getmedal();
  r->deleteLater();
  QString string=nullptr;
  string =QString::fromUtf8(bytes,bytes.size());
  int i =string.indexOf("{\"c");
  string = string.mid(i,-1);
//  qDebug()<<"ggg:"<<string;
  QJsonParseError e;
  QJsonDocument d = QJsonDocument::fromJson(string.toStdString().data(),&e);
  if(e.error==0)
  {
    myT->id++;
    QJsonObject obj=d.object();
    QJsonValue value = obj.value("data");
    QJsonObject json2=value.toObject();
    QJsonValue value2 = json2.value("count");
    if(value2.toInt()==0)
    {
      val++;
      if (val==1)
      {
      himitsu("bilibili.com",&nv);
      QList<QNetworkCookie> allcookies;
      QHash<QByteArray, QByteArray>::iterator i = nv.begin();
      while (i != nv.end()) {
          allcookies.append(QNetworkCookie(i.key(),i.value()));
          ++i;
      }
      QNetworkCookieJar *jar = new QNetworkCookieJar();
      jar->setCookiesFromUrl(allcookies, QUrl("https://api.vc.bilibili.com/dynamic_svr/v1/dynamic_svr/w_live_users?size=100"));
      jar->setCookiesFromUrl(allcookies, QUrl("https://api.bilibili.com/x/polymer/web-dynamic/v1/portal"));
      manager->setCookieJar(jar);}
    }
    else
      val=0;

    QJsonValue value3 = json2.value("items");
    QJsonArray ja=value3.toArray();
    QTime current_time =QTime::currentTime();
    QString t=current_time.toString();
    ui->textEdit->setText(t+"   第"+QString::number(myT->id)+"次请求正常");
    QSet <QString> set;
    QStringList list,titlelist;
    ui->label->setText("在线人数："+QString::number(value2.toDouble()));


    QSet<QString> futari1,futari0;
    if (!dataHash.isEmpty())
    {
      auto it = dataHash.begin();
      while (it != dataHash.end())
      {
      if (it.value().first.addSecs(300)<current_time)
      {
          it = dataHash.erase(it);
      }
      else
      {
          if (it.value().second==1)
              futari1<<it.key();
          else
              futari0<<it.key();
          ++it;
      }
      }
    }



    if(val==0||(val>1&&value2.toInt()==0))
    {
        for(int ii=0;ii<ja.count();ii++){
      QJsonObject arrObj= ja[ii].toObject();
      QString s=arrObj["uname"].toString();
      set<<s;

      if (!link_hash.contains(s))
      {
      QString link=arrObj["link"].toString();
      link_hash.insert(s,link);
      }
      QString title=arrObj["title"].toString();
      titlelist<<title;
      title_hash.insert(s,title);
      if (!face_hash.contains(s))
      {
        QString sss=arrObj["face"].toString();
        face_hash.insert(s,sss);
        url_dic.insert(sss,s);
      }
    }
    set.unite(futari1);
    set.subtract(futari0);
//    qDebug()<<dataHash;
//    qDebug()<<set;
//    qDebug()<<"g:"<<g_set;
    for (QSet<QString>::const_iterator iter = set.constBegin(); iter != set.constEnd(); ++iter) {
      const QString& element = *iter;
      list.append(element);
    }
    if(g_set!=set)
      {
      g_list = list;
//      emit updateSignal();
      ui->listWidget->clear();
      ui->listWidget->addItems(g_list);
      for(int t=0;t<g_list.size();t++)
        ui->listWidget->item(t)->setToolTip(title_hash[ui->listWidget->item(t)->text()]+"\n打开"+ui->listWidget->item(t)->text()+"的直播间");
      kuai();





      QSet <QString> ons,offs;
      if(ui->checkBox->isChecked())
      {
      QStringList on,off;
      QString onnn,offf,zennbu;
      ons=set-g_set;offs=g_set-set;
      g_set = set;
      on=ons.values();off=offs.values();
      if(!ons.isEmpty())
      {
              onnn=on.join(",\n");onnn+="上线了！";
              for (QSet<QString>::const_iterator iter = ons.constBegin(); iter != ons.constEnd(); ++iter) {
                  const QString& element = *iter;
                  dataHash.insert(element, QPair<QTime, int>(current_time, 1));
              }
      }

      if(!offs.isEmpty())
      {if(!ons.isEmpty())
          offf='\n';
        offf+=off.join(",\n");offf+="下线了！";
        for (QSet<QString>::const_iterator iter = offs.constBegin(); iter != offs.constEnd(); ++iter) {
          const QString& element = *iter;
          dataHash.insert(element, QPair<QTime, int>(current_time, 0));
        }
      }
      zennbu=onnn+offf;
      if(myT->id!= 1&&(!offs.isEmpty()||!ons.isEmpty()))
      {
        //QMessageBox::information(nullptr,"提示",zennbu);
        QMessageBox messageBox(QMessageBox::Information,"提示",zennbu,
                                   QMessageBox::Ok, NULL);
        messageBox.setWindowFlag(Qt::WindowStaysOnTopHint);
        messageBox.exec();
      }


    }

//      g_set = set;
    }
    }
  }

        else
  {
    QTime current_time =QTime::currentTime();
    QString t=current_time.toString();
    ui->textEdit->insertPlainText(t+"「あなたに逢えなくなって、錆びた時計と泣いたけど…」\n"+string);
  }
}

void Widget::getURLImage(QNetworkReply *r)
{
  QByteArray bytes;
  QUrl url;
  url = r->url();
  bytes=r->readAll();
  r->deleteLater();
  ui->listWidget->setIconSize(QSize(50,50));
  QImage pixmap;
  pixmap.loadFromData(bytes);bytes.clear();
  pixmap = pixmap.scaled(100,100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  QString name = url_dic.take(url);
  ima.insert(name,pixmap);
  int i = 0;
  bool b = false;
  for(i = 0;i<ui->listWidget->count();i++)
  {
    if(ui->listWidget->item(i)->text()==name)
    {b=true;break;}
  }
  if(!pixmap.isNull()&&b){
    ui->listWidget->item(i)->setIcon(QPixmap::fromImage(pixmap));
  }
}
void Widget::on_listWidget_itemClicked()
{
  for(int i=0;i<ui->listWidget->count();i++)
    if(ui->listWidget->item(i)->foreground()!=QColor(255,215,130)&&hash.contains(ui->listWidget->item(i)->text())){
    ui->listWidget->item(i)->setForeground(QColor(255,215,130));
    QListWidgetItem *item=ui->listWidget->takeItem(i);
    ui->listWidget->insertItem(0,item);
    }
}

void Widget::on_listWidget_itemDoubleClicked()
{
  QString s;
  QDesktopServices::openUrl(QUrl(link_hash[ui->listWidget->currentItem()->text()]));
  QHash<QString, QString>::iterator i;
  i = hash.find(ui->listWidget->item(ui->listWidget->currentRow())->text());
  while (i != hash.end()&&i.key()==ui->listWidget->item(ui->listWidget->currentRow())->text()) {
    s=i.value();
    ++i;}
  if (nv.count()>0)
  {
    if(s!=nullptr)
    {
      QByteArray arr=s.toLatin1();
      wear(arr);
    }
    else
      unwear();
  }
}
void Widget::dealClose()
{
  if(thread->isRunning() == false)//判断线程是否终止
  {
    return;
  }
  myT->setFlag(true);
  thread->quit();
  thread->wait();//将等待线程完成本次响应操作后终止
  delete myT;
}

void Widget::enterEvent(QEnterEvent *)
{
  if(geometry().y()<0){
    animation = new QPropertyAnimation(this,"pos");
    animation->setTargetObject(this);
    animation->setStartValue(QPoint(geometry().x(),geometry().y()));
    animation->setEndValue(QPoint(geometry().x(),0));
    animation->setDuration(100);
    animation->start();}
}


void Widget::leaveEvent(QEvent *)
{
  if(geometry().y()==30 && QCursor().pos().y()>30){
  animation = new QPropertyAnimation(this,"pos");
  animation->setTargetObject(this);
  animation->setStartValue(QPoint(geometry().x(),0));
  animation->setEndValue(QPoint(geometry().x(),-(geometry().y()+geometry().height())+4));
  animation->setDuration(100);
  animation->start();}
}


Widget::~Widget()
{
  dealClose();
  delete ui;
}
void Widget::on_spinBox_valueChanged(int arg1)
{
  myT->kkk=arg1;
}

