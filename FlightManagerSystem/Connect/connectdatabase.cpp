#include "connectdatabase.h"

#include <iostream>

ConnectDataBase::ConnectDataBase()
{
    m_DataBase = QSqlDatabase::addDatabase("QMYSQL");
    m_DataBase.setHostName("localhost");
    m_DataBase.setDatabaseName("flight_management_system");
    m_DataBase.setUserName("root");
    m_DataBase.setPassword("12345678");

    try
    {
        if (!OpenDataBase())
            throw false;
    }
    catch (bool&)
    {
        std::cout << "数据库连接错误，请查看网络连接是否正确" << std::endl;
        std::cout << m_DataBase.lastError().text().toStdString() << std::endl;
        exit(0);
    }
}

bool ConnectDataBase::OpenDataBase()
{
    return m_DataBase.open();
}

bool ConnectDataBase::SelectResult(QSqlQuery* sqlquery, const QString& sql)
{
    try
    {
        if (!sqlquery->exec(sql))
        {
            throw false;
            return false;
        }
        return true;
    }
    catch (bool&)
    {
        std::cout << "数据库打开错误，将检查网络连接！" << std::endl;
        std::cout << sqlquery->lastError().text().toStdString() << std::endl;
    }
}

QString ConnectDataBase::InsertValue(const QString& sql, const int& num, QVector<QString>& values)
{
    QSqlQuery qsqlquery;
    qsqlquery.prepare(sql);
    while (values.size())
    {
        for (int i = 0; i < num; ++i)
        {
            if (values.front().toInt())
                qsqlquery.addBindValue(values.front().toInt());
            else
                qsqlquery.addBindValue(values.front());
            values.pop_front();
        }
        if (!qsqlquery.exec())
        {
            std::cout << qsqlquery.lastError().text().toStdString() << std::endl;
            return qsqlquery.lastError().text();
        }
    }
    return "Success";
}

QString ConnectDataBase::UpdateValue(QVector<QString> &sql)
{
    QSqlQuery qsqlquery;
    while (sql.size())
    {
        if (sql.front() == "")
        {
            sql.pop_front();
            continue;
        }
        qsqlquery.prepare(sql.front());
        try
        {
            if (!qsqlquery.exec())
                throw false;
        }
        catch (bool&)
        {
            std::cout << qsqlquery.lastError().text().toStdString() << std::endl;
            return qsqlquery.lastError().text();
        }
        sql.pop_front();
    }

    return "Success";
}

QString ConnectDataBase::DeletValue(const QString &sql)
{
    QSqlQuery qsqlquery;

    try
    {
        if (!qsqlquery.exec(sql) )
            throw false;
    }
    catch (bool&)
    {
        std::cout << qsqlquery.lastError().text().toStdString() << std::endl;
        return qsqlquery.lastError().text();
    }

    return "Success";
}

int ConnectDataBase::CallPro(const QString& sql)
{
    QSqlQuery qsqlquery(sql);

    if (qsqlquery.exec())
    {
        qsqlquery.next();
        return qsqlquery.value(0).toString().toInt();
    }
    else
    {
        std::cout << qsqlquery.lastError().text().toStdString() << std::endl;
        return -1;
    }
}
