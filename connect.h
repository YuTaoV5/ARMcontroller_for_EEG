#ifndef CONNECT_H
#define CONNECT_H
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include "mainwindow.h"

static bool createConnection()
{
     QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
     db.setDatabaseName("datebase.db");
     if (!db.open()) {
         QMessageBox::critical(0, qApp->tr("Cannot open database"),
         qApp->tr("Unable to establisha database connection." ), QMessageBox::Cancel);
         return false;
     }
     QSqlQuery query;
     QString create_sql = "create table HugeSmart_ARM (id int primary key, name varchar(30), age int)";
     query.prepare(create_sql);
     if(!query.exec())
     {
         qDebug() << "Error: Fail to create table." << query.lastError();
     }
     else
     {
         qDebug() << "Table created!";
     }
}
#endif // CONNECT_H
