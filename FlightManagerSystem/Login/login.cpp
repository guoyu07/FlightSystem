#include "login.h"
#include "ui_login.h"

#include <iostream>
using std::cout;
using std::endl;

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    setWindowTitle("航空信息管理系统登陆");
    ui->passwordtext->setEchoMode(QLineEdit::Password);

    m_Connect = ConnectDataBase::GetInstance();

    //当自定义信号发出时，关闭此对话框并销毁该对话框的内存
    connect(this, SIGNAL(send()), this, SLOT(close()) );
    connect(this, SIGNAL(send()), this, SLOT(deleteLater()) );
}

Login::~Login()
{
    delete ui;
    cout << "delete Login" << endl;
}

int Login::CheckWriting()
{
    if (ui->accounttext->text() == "")
    {
        QMessageBox(QMessageBox::Warning, "账号错误", "账号为空，请输入账号", QMessageBox::Ok).exec();
        return AccountBlank;
    }
    if (ui->passwordtext->text() == "")
    {
        QMessageBox(QMessageBox::Warning, "密码错误", "密码为空，请输入密码", QMessageBox::Ok).exec();
        return PasswordBlank;
    }
    if (ui->accounttext->text() == ui->passwordtext->text())
    {
        QMessageBox(QMessageBox::Warning, "重复错误", "账号密码不得相同", QMessageBox::Ok).exec();
        return RepeatError;
    }
    return Success;
}

int Login::CheckAccount()
{
    QString account = ui->accounttext->text();
    QString word = ui->passwordtext->text();
    char* str = new char[word.size() + 1];
    strcpy(str, word.toStdString().c_str());
    unsigned int password = BKDRHash(str);
    QString sql = "select password from account where account = '" + account + "'";
    QSqlQuery* sqlquery = new QSqlQuery;
    try
    {
        //返回的数据集停在第一条记录前，必须执行next或first后才能指向第一条记录
        if (m_Connect->SelectResult(sqlquery, sql) )
            sqlquery->next();
        else
            throw false;
    }
    catch (bool&)
    {
        return WebError;
    }

    bool flag;
    if (!sqlquery->size() )
        return AccountWrong;
    else
        flag = password == sqlquery->value("password").toUInt() ? true : false;

    delete sqlquery;
    sqlquery = NULL;
    delete[] str;
    str = NULL;

    if (flag)
        return Success;
    else
        return PasswordWrong;
}

unsigned int Login::BKDRHash(char* str)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    while (*str)
        hash = hash * seed + (*str++);

    return (hash & 0x7FFFFFFF);
}

void Login::on_loginbutton_clicked()
{
    if (Success != CheckWriting() )
        return;

    int flag = CheckAccount();
    if (Success == flag)
        dosend();
    else if (WebError == flag)
        QMessageBox(QMessageBox::Warning, "查询失败", "数据库无法打卡, 请检查网络配置！", QMessageBox::Ok).exec();
    else if (AccountWrong == flag)
        QMessageBox(QMessageBox::Warning, "输入错误", "没有此账号，请确认后重新输入！", QMessageBox::Ok).exec();
    else if (PasswordWrong == flag)
        QMessageBox(QMessageBox::Warning, "输入错误", "密码错误，请检查密码是否正确后重新输入!", QMessageBox::Ok).exec();
}

void Login::on_quitbutton_clicked()
{
    exit(0);
}

//发送自定义信号
void Login::dosend()
{
    emit send();
}
