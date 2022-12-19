#include "himitsu.h"
#pragma comment (lib, "crypt32")
OSSL_LIB_CTX *libctx = NULL;
const char *propq = NULL;


QByteArray cry(QByteArray data,DATA_BLOB DataOut)
{
    QByteArray byteArray1 = data.mid(3,12);
    QByteArray byteArray2 = data.mid(15,-1);
    QByteArray byteArray3,byteArray4;
    for (int i = 0;i<byteArray2.size();i++)
        if (i<byteArray2.size()-16)
            byteArray3.append(byteArray2.at(i));
        else
            byteArray4.append(byteArray2.at(i));
    const unsigned char *gcm_iv = reinterpret_cast<const unsigned char *>(byteArray1.constData());
    const unsigned char *gcm_ct = reinterpret_cast<const unsigned char *>(byteArray3.constData());
    const unsigned char *gcm_tag = reinterpret_cast<const unsigned char *>(byteArray4.constData());
    const unsigned char *gcm_key = (DataOut.pbData);
    EVP_CIPHER_CTX *ctx;
    int outlen = 1024 + EVP_MAX_BLOCK_LENGTH;
    size_t gcm_ivlen = byteArray1.size();
    EVP_CIPHER *cipher = NULL;
    unsigned char outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
    OSSL_PARAM params[2] = {
        OSSL_PARAM_END, OSSL_PARAM_END
    };
    if ((ctx = EVP_CIPHER_CTX_new()) == NULL)
        qDebug()<<"error";
    if ((cipher = EVP_CIPHER_fetch(libctx, "AES-256-GCM", propq)) == NULL)
        qDebug()<<"error";
    params[0] = OSSL_PARAM_construct_size_t(OSSL_CIPHER_PARAM_AEAD_IVLEN,&gcm_ivlen);
    EVP_DecryptInit_ex2(ctx, cipher, gcm_key, gcm_iv, params);
    EVP_DecryptUpdate(ctx, outbuf, &outlen, gcm_ct, byteArray3.size());
//    qDebug()<<outbuf<<outlen;
    QByteArray qba(reinterpret_cast<char*>(outbuf),outlen);
//    qDebug()<<qba;
    params[0] = OSSL_PARAM_construct_octet_string(OSSL_CIPHER_PARAM_AEAD_TAG,(void*)gcm_tag, byteArray4.size());
    EVP_CIPHER_CTX_set_params(ctx, params);
    EVP_DecryptFinal_ex(ctx, outbuf, &outlen);
    EVP_CIPHER_free(cipher);
    EVP_CIPHER_CTX_free (ctx);
    return qba;
}

int himitsu(QString url,QHash <QByteArray,QByteArray>*nv)
{
//    QCoreApplication a(argc, argv);
    QStringList path = QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation);
    QString str = path[0];
    int i =str.indexOf("AppData");
    str = str.mid(0,i+8);
    QString key = str + "Local/Microsoft/Edge/User Data/Local State";
    QString cookies = str + "Local/Microsoft/Edge/User Data/Default/Network/Cookies";
    QFile file(key);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"error";
    }
    QTextStream read(&file);
    QString Local_State;
    QByteArray encrypted_key;
    Local_State = read.readAll();
    DATA_BLOB DataOut;
    QJsonParseError e;
    QJsonDocument doc = QJsonDocument::fromJson(Local_State.toStdString().c_str(), &e);
    if(e.error==0)
    {
        QJsonObject obj = doc.object();
        QJsonValue message=obj.value("os_crypt");
        QJsonObject os_crypt = message.toObject();
        QJsonValue cry=os_crypt.value("encrypted_key");
        QByteArray encrypted_key_with_header = QByteArray::fromBase64(cry.toString().toUtf8());
        encrypted_key = encrypted_key_with_header.mid(5);
        DATA_BLOB DataIn;
        LPWSTR pDescrOut =  NULL;
        ZeroMemory(&DataIn, sizeof(DataIn));
        ZeroMemory(&DataOut, sizeof(DataOut));
        // The flags
        DWORD flags = 0;
        DataIn.pbData =reinterpret_cast<unsigned char *>(encrypted_key.data());
        //qDebug()<<DataIn.pbData<<sizeof(DataIn.pbData)<<(unsigned char *)(encrypted_key.data());
        DataIn.cbData = encrypted_key.size();
        CryptUnprotectData(&DataIn,&pDescrOut, NULL, NULL, NULL, flags,&DataOut);
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(cookies);
    db.open();
    QString q = "SELECT name,encrypted_value FROM cookies where host_key like \'%." + url +"\'";
    QSqlQuery query(q,db);
    QByteArray data;
    while(query.next())
    {
        data.clear();
        data = query.value(1).toByteArray();
        *nv->insert(query.value(0).toByteArray(),cry(data,DataOut));
    }
    return 0;
}
