#include "widget.h"
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
#include <QFile>
#include <QJsonDocument>
#include <QFont>
#include <QDateTime>
#include <QMessageBox>
#include <QSize>
#include<QHash>
QTime c_time ;
QHash<QString, QString> hash;
QHash<QString,QImage> ima;
QStringList livelist,key;
QList<QUrl> facelist;
QByteArrayList arr;
static int shenmeshenme=0;
static int fight=1,tot;
QByteArray bytes;
QSet <QString> g_set,o,f;
Widget::Widget(QWidget *parent)
    : QWidget(parent,Qt::Widget| Qt::WindowStaysOnTopHint)
      , ui(new Ui::Widget)
{
  ui->setupUi(this);
  QString path = QCoreApplication::applicationDirPath();
  path+="/list.txt";
  readFile(path);

  myT = new MyThread;
  thread = new QThread(this);
  myT->moveToThread(thread);
  manager = new QNetworkAccessManager(this);
  m = new QNetworkAccessManager(this);
  imi = new QNetworkAccessManager(this);
  connect(m, SIGNAL(finished(QNetworkReply*)),this, SLOT(sav(QNetworkReply*)));
  connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(finishedSlot(QNetworkReply*)));
  connect(imi, SIGNAL(finished(QNetworkReply *)),this, SLOT(getURLImage(QNetworkReply *)));
  connect(myT, &MyThread::mySignal, this, &Widget::dealSignal);
  connect(this, &Widget::startThread,myT, &MyThread::myTimeout);
  connect(this,&Widget::destroyed, this, &Widget::dealClose);
  getmedal();fff();
}
void Widget::readFile(QString path)
{
  QFile file(path);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QMessageBox::warning(nullptr,"提示","没有找到list.txt\n请检查文件后重新打开！");
  }
  QTextStream read(&file);
  while (!read.atEnd())
  {
    QString line = read.readLine();
    if (line.isEmpty())
      break;
    if (line.indexOf("//")==0)
      continue;
    else{
      if(line.lastIndexOf("//"))
      {int i=line.lastIndexOf("/");
        line=line.mid(0,i-1);}
      key<<line;}
  }
  file.close();
  if (key.isEmpty())
  {
    QString msg = "<a>程序目录下的list.txt为空<br>编辑第一行SESSDATA值，第二行bili_jct值//后可以备注\n<br><a href='https://api.vc.bilibili.com/dynamic_svr/v1/dynamic_svr/w_live_users?size=100'>打开接口</a>F12找到cookie中SESSDATA的值复制到list.txt" ;
    QMessageBox::information(nullptr,"提示",msg);
    exit(0);
  }
}

void Widget::fff()
{
  thread->start();
  emit startThread();
}
void Widget::dealSignal()
{
  if(shenmeshenme==0)
  {QNetworkRequest req;
  QList<QNetworkCookie> allcookies;
  QVariant var;
  allcookies.append(QNetworkCookie("SESSDATA",key.at(0).toLatin1()));
  var.setValue(allcookies);
  req.setUrl(QUrl("https://api.vc.bilibili.com/dynamic_svr/v1/dynamic_svr/w_live_users?size=1000"));
  req.setHeader(QNetworkRequest::CookieHeader,var);
  if(ui->listWidget->item(0)==nullptr)
  {QNetworkCookieJar *jar = new QNetworkCookieJar();
    jar->setCookiesFromUrl(allcookies, QUrl("https://api.vc.bilibili.com/dynamic_svr/v1/dynamic_svr/w_live_users?size=1000"));
    manager->setCookieJar(jar);}
  m_reply=manager->get(req);
   //connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(finishedSlot(QNetworkReply*)));
  //connect(m_reply,SIGNAL(finished()),this,SLOT(finishedSlot()));
  }
  //manager->deleteLater();
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
  //mutex.lock();
  if(!facelist.isEmpty()){
  QUrl url=facelist.front();
  //QUrl url=facelist.at(shenmeshenme);
  QNetworkReply *r;
  facelist.pop_front();
  if(ima.contains(ui->listWidget->item(shenmeshenme)->text()))
  {
    QImage q;
    QHash<QString, QImage>::iterator i;
    i = ima.find(ui->listWidget->item(shenmeshenme)->text());
    while (i != ima.end()&&i.key()==ui->listWidget->item(shenmeshenme)->text()) {
      q=i.value();
      ++i;}
    ui->listWidget->item(shenmeshenme)->setIcon(QPixmap::fromImage(q));
    if(shenmeshenme<(livelist.count()-1))
    {
      shenmeshenme++;
      kuai();
    }
    else
      shenmeshenme=0;
  }
  else
  {r=imi->get(QNetworkRequest(url));
    connect(r,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(updateDataReadProgress(qint64,qint64)));}
  //connect(m_reply,SIGNAL(finished()),this,SLOT(getURLImage()));
  //connect(m, SIGNAL(finished(QNetworkReply *)),this, SLOT(getURLImage(QNetworkReply *))); // mutex.unlock();
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
  QString string;
  string =QString::fromUtf8(bytes,bytes.size());
  //qDebug()<<string;
  QTime current_time =QTime::currentTime();
  QString t=current_time.toString();
  ui->textEdit->insertPlainText(t+"  佩戴成功\n"+string+'\n');

}
void Widget::getmedal()
{
  if(fight)
  {qDebug()<<fight;
    QNetworkReply *reply;
    QNetworkRequest reqg;
    QList<QNetworkCookie> cookies;
    cookies.append(QNetworkCookie("SESSDATA",key.at(0).toLatin1()));
    cookies.append(QNetworkCookie("bili_jct",key.at(1).toLatin1()));
    //reqg.setUrl(QUrl("https://api.live.bilibili.com/fans_medal/v1/FansMedal/get_list_in_room"));
    reqg.setUrl(QUrl("https://api.live.bilibili.com/fans_medal/v2/HighQps/received_medals?page="+QString::number(fight)));
    QNetworkCookieJar *jar = new QNetworkCookieJar();
    //jar->setCookiesFromUrl(cookies, QUrl("https://api.live.bilibili.com/fans_medal/v1/FansMedal/get_list_in_room"));
    jar->setCookiesFromUrl(cookies, QUrl("https://api.live.bilibili.com/fans_medal/v2/HighQps/received_medals?page="+QString::number(fight)));
    m->setCookieJar(jar);
    reply=m->get(reqg);
  }

 // fight=0;
}
void Widget::sav(QNetworkReply* reply)
{
  QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  if(statusCode.isValid())
  {
    //m->disconnect(m, SIGNAL(finished(QNetworkReply*)),this, SLOT(sav(QNetworkReply*)));
    qDebug() << "save status code=" << statusCode;
  }
  QString string=nullptr;
  QByteArray b=reply->readAll();
  string =QString::fromUtf8(b,b.size());//bytes.clear();
  //qDebug()<<string;
  QJsonParseError e;
  QJsonDocument d = QJsonDocument::fromJson(string.toStdString().data(),&e);
  if(e.error==0){
  QJsonObject obj=d.object();
  QJsonValue value1=obj.value("data");
  QJsonObject obj2=value1.toObject();
  //QJsonValue value2 = obj.value("data");
  QJsonValue value2 = obj2.value("list");
  QJsonValue value3 = obj2.value("total_page");
  tot=value3.toInt();
  QJsonArray ja=value2.toArray();
  for(int ii=0;ii<ja.count();ii++){
    QJsonObject arrObj= ja[ii].toObject();
    QString s=arrObj["target_name"].toString();
    QString medal_id=QString::number(arrObj["medal_id"].toInt());
    //qDebug()<<medal_id;
    hash.insert(s,medal_id);
  }
  if(fight<tot)
  {fight++;qDebug()<<fight;getmedal();}
  else
  {fight=0;qDebug()<<hash;
  m->disconnect(m, SIGNAL(finished(QNetworkReply*)),this, SLOT(sav(QNetworkReply*)));
  }
  }
 // else
      //fight=1;
  qDebug()<<hash.count();
}
void Widget::wear(QByteArray arr)
{
  QNetworkReply *r;
  QNetworkRequest req;
  QList<QNetworkCookie> allcookies;
  QVariant var;
  allcookies.append(QNetworkCookie("SESSDATA",key.at(0).toLatin1()));
  var.setValue(allcookies);
  req.setHeader(QNetworkRequest::CookieHeader,var);
  QNetworkCookieJar *jar = new QNetworkCookieJar();
  jar->setCookiesFromUrl(allcookies, QUrl("https://api.live.bilibili.com/xlive/web-room/v1/fansMedal/wear"));
  m->setCookieJar(jar);
  QByteArray dataArray="medal_id="+arr+"&csrf_token="+key.at(1).toLatin1()+"&csrf="+key.at(1).toLatin1()+"&visit_id="+key.at(2).toLatin1();
  int contentLength=dataArray.size();
  req.setUrl(QUrl("https://api.live.bilibili.com/xlive/web-room/v1/fansMedal/wear"));
  req.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
  req.setHeader(QNetworkRequest::ContentLengthHeader,contentLength);
  r=m->post(req,dataArray);
  connect(m,SIGNAL(finished(QNetworkReply *)),this,SLOT(shuchu(QNetworkReply *)));
}
void Widget::finishedSlot(QNetworkReply* r)
{
  QByteArray bytes=nullptr;
  if(r->error()==0)
  bytes = r->readAll();
  r->deleteLater();
  //getmedal();
  QString string=nullptr;
  string =QString::fromUtf8(bytes,bytes.size());bytes.clear();
  int i =string.indexOf("{\"c");
  string = string.mid(i,-1);
  QJsonParseError e;
  QJsonDocument d = QJsonDocument::fromJson(string.toStdString().data(),&e);
  if(e.error==0)
  {
    myT->id++;
    QJsonObject obj=d.object();
    QJsonValue value = obj.value("data");  // 获取指定 key 对应的 value
    QJsonObject json2=value.toObject();
    QJsonValue value2 = json2.value("count");
    QFont list_font;
    list_font.setPointSize(20);
    ui->label->setFont(list_font);
    QJsonValue value3 = json2.value("items");
    QJsonArray ja=value3.toArray();
    QTime current_time =QTime::currentTime();
    QString t=current_time.toString();
    ui->textEdit->setText(t+"   第"+QString::number(myT->id)+"次请求正常");
    QSet <QString> set;
    QStringList list,titlelist;
    QList<QUrl> facelist_t;
    ui->label->setText("在线人数："+QString::number(value2.toDouble()));livelist.clear();
        for(int ii=0;ii<ja.count();ii++){
      QJsonObject arrObj= ja[ii].toObject();
      QString s=arrObj["uname"].toString();
      list<<s;set<<s;
      QString link=arrObj["link"].toString();
      livelist<<link;
      QString title=arrObj["title"].toString();
      titlelist<<title;
      QString sss=arrObj["face"].toString();
      facelist_t<<sss;
    }
        //if(!facelist_t.isEmpty())
      facelist=facelist_t;
    if(g_set!=set)
      {QTime current_time =QTime::currentTime();
        /*bool b=true;
      if(c_time.isNull()){
      b=true;
      }
      else
        b=(c_time.secsTo(current_time))>10;
      bool b1=f==set-g_set;bool b2=o==g_set-set;qDebug()<<b1<<b2;
      if(((!(f==set-g_set||o==g_set-set))&&b)||c_time.isNull()||(f.isEmpty()&&g_set.isEmpty())) {*/
        //mutex.lock();
      ui->listWidget->setFont(list_font);
      ui->listWidget->clear();
      ui->listWidget->addItems(list);
      kuai();
      for(int t=0;t<titlelist.count();t++)
        ui->listWidget->item(t)->setToolTip(titlelist.at(t)+"\n打开"+ui->listWidget->item(t)->text()+"的直播间");
      QSet <QString> ons,offs;
      if(ui->checkBox->isChecked())
      {
      //QSet <QString> ons,offs;
      QStringList on,off;
      QString onnn,offf,zennbu;
      ons=set-g_set;offs=g_set-set;g_set=set;
      //o=ons;f=offs;
      on=ons.values();off=offs.values();
      if(!ons.isEmpty())
      {onnn=on.join(",\n");onnn+="上线了！";}
      if(!offs.isEmpty())
      {if(!ons.isEmpty())
          offf='\n';
        offf+=off.join(",\n");offf+="下线了！";}
      zennbu=onnn+offf;
      qDebug()<<(c_time.secsTo(current_time)>10)<<((f.isEmpty()&&!o.isEmpty())&&(o!=offs)&&(f!=ons));
      if(c_time.secsTo(current_time)>10||((f.isEmpty()&&!o.isEmpty())&&(o!=offs)&&(f!=ons)))
      {o=ons;f=offs;c_time=QTime::currentTime();
        //QMessageBox::information(nullptr,"提示",zennbu);
        QMessageBox messageBox(QMessageBox::Information,"提示",zennbu,
                                   QMessageBox::Ok, NULL);
        messageBox.setWindowFlag(Qt::WindowStaysOnTopHint);
        messageBox.exec();
      }
      else
      {o=ons;f=offs;c_time=QTime::currentTime();}
      }
       o=ons;f=offs;
      c_time=QTime::currentTime();
      g_set=set;
    }
  }
        else
  {
    QTime current_time =QTime::currentTime();
    QString t=current_time.toString();
    ui->textEdit->insertPlainText(t+"「あなたに逢えなくなって、錆びた時計と泣いたけど…」\n");
  }
}
void Widget::getURLImage(QNetworkReply *r)
{
  QByteArray bytes;
  bytes=r->readAll();
  r->deleteLater();
  ui->listWidget->setIconSize(QSize(50,50));
  QImage pixmap;
  pixmap.loadFromData(bytes);
  ima.insert(ui->listWidget->item(shenmeshenme)->text(),pixmap);
  bytes.clear();
      if(!pixmap.isNull()/*&&(pixmap.pixel(pixmap.width()-1,pixmap.height()-1 )!=4286611584)&&(pixmap.pixel(pixmap.width()/2,pixmap.height()-1 )  !=  4286611584)&&(pixmap.pixel(0,pixmap.height()-1 )  !=  4286611584)*/)
  {
        pixmap = pixmap.scaled(100,100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        if(ui->listWidget->item(shenmeshenme))
        ui->listWidget->item(shenmeshenme)->setIcon(QPixmap::fromImage(pixmap));
        if(shenmeshenme<(livelist.count()-1))
        {//mutex.lock();
          shenmeshenme++;
          kuai();
         //mutex.unlock();
        }
        else
        shenmeshenme=0;
    }
      else
      {
        if(shenmeshenme<(livelist.count()-1))
      {shenmeshenme++;kuai();}
       else
          shenmeshenme=0;
    }
}
void Widget::on_listWidget_itemClicked()
{
  for(int i=0;i<ui->listWidget->count();i++)
    if(hash.contains(ui->listWidget->item(i)->text()))
      ui->listWidget->item(i)->setForeground(QColor(255,215,130));
}
void Widget::on_listWidget_itemDoubleClicked()
{
  QString s;
  QDesktopServices::openUrl(QUrl(livelist.at(ui->listWidget->currentRow())));
  QHash<QString, QString>::iterator i;
  i = hash.find(ui->listWidget->item(ui->listWidget->currentRow())->text());
  while (i != hash.end()&&i.key()==ui->listWidget->item(ui->listWidget->currentRow())->text()) {
    s=i.value();
    ++i;}
  //qDebug()<<s;
  if(s!=nullptr)
  {QByteArray arr=s.toLatin1();
    wear(arr);}
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
Widget::~Widget()
{
  delete ui;
}
void Widget::on_spinBox_valueChanged(int arg1)
{
  myT->kkk=arg1;
}

