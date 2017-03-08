#include "login.h"
#include "flightmanager.h"
#include <QApplication>

#include <iostream>

ConnectDataBase* ConnectDataBase::m_Instance = new ConnectDataBase;
ConnectDataBase::GC ConnectDataBase::gc;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Login* l = new Login;
//    l->show();
//    QApplication::processEvents();
    int result = 0;
    FlightManager* f = new FlightManager;
    if (!f)
    {
        QMessageBox(QMessageBox::Critical, "错误", "无法初始化主界面", QMessageBox::Ok).exec();
        return EXIT_FAILURE;
    }
    f->show();
    QApplication::processEvents();
    /*
     * 将登录类与主界面的类通过信号绑定，当login类检查到登陆成功时则会发信号到flightmanager
     * flightmanger接收到信号则会打开主界面，而登陆界面则会销毁
     */
//    a.connect(l, SIGNAL(send() ), f, SLOT(receive()) );

    try
    {
        std::cout << "before exec" << std::endl;
        result = a.exec();
        std::cout << "after exec" << std::endl;
    }
    catch (...)
    {
        QMessageBox::warning(0, "FlightManager crashed!", "Hum, it seems that FlightManager has crashed... Sorry about that :)");
    }

    delete f;
    f = NULL;

    return result;
}
