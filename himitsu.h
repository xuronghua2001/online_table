#ifndef HIMITSU_H
#define HIMITSU_H
#include <QCoreApplication>
#include <QFile>
#include <QHash>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "openssl/evp.h"
#include "openssl/types.h"
#include "openssl/ossl_typ.h"
//#include <openssl/applink.c>
#include <iostream>
#include <openssl/core_names.h>
#include <windows.h>
#include <wincrypt.h>

int himitsu(QString url,QHash <QByteArray,QByteArray>*nv);

#endif // HIMITSU_H
