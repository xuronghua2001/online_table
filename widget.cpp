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
#include <QImageReader>
#include <QSize>
QNetworkAccessManager m;
QStringList livelist,facelist;
QByteArrayList arr;
static int ii;
static int shenmeshenme=0;
static int xiaoshao=1;
QString key;
QSet <QString> g_set;
Widget::Widget(QWidget *parent)
    : QWidget(parent)
      , ui(new Ui::Widget)
{
  ui->setupUi(this);
  QString path = QCoreApplication::applicationDirPath();
  path+="/list.txt";
  readFile(path);
  myT = new MyThread;
  thread = new QThread(this);
  myT->moveToThread(thread);
  connect(myT, &MyThread::mySignal, this, &Widget::dealSignal);
  connect(this, &Widget::startThread,myT, &MyThread::myTimeout);
  connect(this,&Widget::destroyed, this, &Widget::dealClose);
  connect(this,&Widget::fen,this,&Widget::kuai);
  fff();
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
      key=line;}
  }
  file.close();
  if (key==nullptr)
  {
    QString msg = "<a>程序目录下的list.txt为空<br>编辑第一行SESSDATA值，//后可以备注\n<br><a href='https://api.vc.bilibili.com/dynamic_svr/v1/dynamic_svr/w_live_users?size=100'>打开接口</a>F12找到cookie中SESSDATA的值复制到list.txt" ;
    QMessageBox::information(nullptr,"提示",msg);
    exit(0);
  }
}
void Widget::fff()
{thread->start();
  emit startThread();
}
void Widget::dealSignal()
{
 QNetworkRequest req;
  QList<QNetworkCookie> allcookies;
  QVariant var;
  allcookies.append(QNetworkCookie("SESSDATA",key.toLatin1()));
  var.setValue(allcookies);
  req.setUrl(QUrl("https://api.vc.bilibili.com/dynamic_svr/v1/dynamic_svr/w_live_users?size=100"));
  req.setHeader(QNetworkRequest::CookieHeader,var);
  QNetworkCookieJar *jar = new QNetworkCookieJar();
  jar->setCookiesFromUrl(allcookies, QUrl("https://api.vc.bilibili.com/dynamic_svr/v1/dynamic_svr/w_live_users?size=100"));
  m.setCookieJar(jar);
  m_reply=m.get(req);
  connect(m_reply,SIGNAL(finished()),this,SLOT(finishedSlot()));
}
void Widget::kuai()
{
  {m_reply=m.get(QNetworkRequest(QUrl(facelist.at(shenmeshenme))));
    connect(m_reply,SIGNAL(readyRead()),this,SLOT(getURLImage()));}
}
void Widget::finishedSlot()
{
  QByteArray bytes=nullptr;
  bytes.resize(10000);
  //while(!m_reply->atEnd())
  while(m_reply->bytesAvailable()>10)
    bytes += m_reply->readAll();
  //qDebug()<<bytes;
  QString string=nullptr;
  string =QString::fromUtf8(bytes,bytes.size());
  int i =string.indexOf("{\"c");
  string = string.mid(i,-1);
  QJsonObject json;
  QJsonParseError e;
  QJsonDocument d = QJsonDocument::fromJson(string.toStdString().data(),&e);
  if(e.error==0)
  {
    QJsonObject obj=d.object();
    QJsonValue value = obj.value("data");  // 获取指定 key 对应的 value
    QJsonObject json2=value.toObject();
    QJsonValue value2 = json2.value("count");
    QFont list_font;
    list_font.setPointSize(20);
    ui->label->setFont(list_font);
    ui->lcdNumber->display(value2.toDouble());
    QJsonValue value3 = json2.value("items");
    QJsonArray ja=value3.toArray();
    QTime current_time =QTime::currentTime();
    QString t=current_time.toString();
    ui->textEdit->setText(t+"  请求正常");
    QSet <QString> set;
    QStringList list,titlelist,facelist_t;
    ui->label->setText("在线人数："+QString::number(value2.toDouble()));livelist.clear();
        for(ii=0;ii<ja.count();ii++){
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
        facelist=facelist_t;
    if(xiaoshao)
    {
       m_reply=m.get(QNetworkRequest(QUrl(facelist.at(shenmeshenme))));
       connect(m_reply,SIGNAL(readyRead()),this,SLOT(getURLImage()));
    }
        if(g_set!=set)
    {
      if(m_reply->isRunning())
      m_reply->abort();
      xiaoshao=1;shenmeshenme=0;
      ui->listWidget->clear();
      ui->listWidget->setFont(list_font);
      ui->listWidget->addItems(list);
      for(int t=0;t<titlelist.count();t++)
        ui->listWidget->item(t)->setToolTip(titlelist.at(t)+"\n打开"+ui->listWidget->item(t)->text()+"的直播间");
      if(ui->checkBox->isChecked()){
      QSet <QString> ons,offs;
      QStringList on,off;
      QString onnn,offf,zennbu;
      ons=set-g_set;offs=g_set-set;g_set=set;
      on=ons.values();off=offs.values();
      if(!ons.isEmpty())
      {onnn=on.join(",\n");onnn+="上线了！";}
      if(!offs.isEmpty())
      {if(!ons.isEmpty())
          offf='\n';
        offf+=off.join(",\n");offf+="下线了！";}
      zennbu=onnn+offf;
      QMessageBox::information(nullptr,"提示",zennbu);
      }
    }
  }
        else
  {
    QTime current_time =QTime::currentTime();
    QString t=current_time.toString();
    ui->textEdit->insertPlainText(t+"「あなたに逢えなくなって、錆びた時計と泣いたけど…」\n");
    ui->lcdNumber->display("error");
  }
}
void Widget::getURLImage()
{
  if(xiaoshao){
  m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
  ui->listWidget->setIconSize(QSize(50,50));
connect(m_reply, &QNetworkReply::finished, [=]{
      //qDebug()<<shenmeshenme;
    QPixmap pixmap;
      QImageReader ir;
        if(pixmap.loadFromData(m_reply->readAll())){
        pixmap = pixmap.scaled(500, 500, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmap.save(QString::number(shenmeshenme)+".jpg",nullptr,-1);
        QSize qs;qs.scale(500,500,Qt::KeepAspectRatio);
        ir.setFileName(QString::number(shenmeshenme)+".jpg");ir.setScaledSize(qs);
        ui->listWidget->item(shenmeshenme)->setIcon(QPixmap::fromImageReader(&ir));
        if(shenmeshenme<(ui->listWidget->count()-1))
        {shenmeshenme++;
          emit fen();}
        else
        {shenmeshenme=0;
          xiaoshao=0;
        }
        }
  });}
}
void Widget::on_listWidget_itemDoubleClicked()
{
  QDesktopServices::openUrl(QUrl(livelist.at(ui->listWidget->currentRow())));
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
  delete m_reply;
    }

Widget::~Widget()
{
  //delete m_reply;
  delete ui;
}

