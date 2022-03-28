#include "widget.h"
#include "ui_widget.h"
#include <QCoreApplication>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDebug>
#include <QVector>
#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QFont>
#include <QDateTime>
#include <QMessageBox>
QNetworkAccessManager m;
QStringList livelist;
static int ii;
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
  //当窗口销毁时，关闭线程
  connect(this,&Widget::destroyed, this, &Widget::dealClose);

 // emit startThread();

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
    exit(0);}

}
void Widget::fff()
{thread->start();
  emit startThread();
}
void Widget::elle(QNetworkRequest req,QVariant var)
{
  req.setHeader(QNetworkRequest::CookieHeader,var);
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
void Widget::finishedSlot()
{
  QByteArray bytes=nullptr;
  bytes.resize(10000);
  while(!m_reply->atEnd())
    bytes += m_reply->readAll();
  QString string=nullptr;
  string =QString::fromUtf8(bytes,bytes.size());
  int i =string.indexOf('{');
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
    ui->listWidget->clear();ui->label->setText("在线人数："+QString::number(value2.toDouble()));livelist.clear();
        for(ii=0;ii<ja.count();ii++){
      QJsonObject arrObj= ja[ii].toObject();
      QString s=arrObj["uname"].toString();
      ui->listWidget->setFont(list_font);
      ui->listWidget->addItem(s);set<<s;
      QString link=arrObj["link"].toString();
      livelist<<link;
      QString title=arrObj["title"].toString();
      ui->listWidget->item(ii)->setToolTip(title+"\n打开"+s+"的直播间");
      //QString sss=arrObj["face"].toString();
      //m_reply=m.get(QNetworkRequest(sss));
      //connect(m_reply,SIGNAL(readyRead()),this,SLOT(getURLImage()));
    }
        if(g_set!=set&&ui->checkBox->isChecked())
    {
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
  ui->listWidget->setIconSize(QSize(30,30));
connect(m_reply, &QNetworkReply::finished, [=]{
      //qDebug()<<"tu";
    QPixmap pixmap;
        if(pixmap.loadFromData(m_reply->readAll())){
        pixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        qDebug()<<pixmap;
        ui->listWidget->item(ii-1)->setIcon(QIcon(pixmap));
  });

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
  //delete m_reply;
    }

Widget::~Widget()
{
      delete m_reply;
  delete ui;
}

