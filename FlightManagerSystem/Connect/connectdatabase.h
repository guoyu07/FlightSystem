#ifndef CONNECTDATABASE_H
#define CONNECTDATABASE_H

#include <QtSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QtSql/QSqlDatabase>

//使用单例模式
class ConnectDataBase
{
public:
    static ConnectDataBase* GetInstance()
    {
        return m_Instance;
    }

    bool OpenDataBase();
    bool SelectResult(QSqlQuery* sqlquery, const QString& sql);
    QString InsertValue(const QString& table, const int& num, QVector<QString>& values);
    QString UpdateValue(QVector<QString>& sql);
    QString DeletValue(const QString& sql);
    int CallPro(const QString& sql);

private:
    //销毁对象
    class GC
    {
    public:
        ~GC()
        {
            if (!m_Instance)
            {
                delete m_Instance;
                m_Instance = NULL;
            }
        }
    };
    static GC gc;

    ConnectDataBase();

    static ConnectDataBase* m_Instance;
    QSqlDatabase m_DataBase;
};

#endif // CONNECTDATABASE_H
