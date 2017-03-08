#ifndef FLIGHTMANAGER_H
#define FLIGHTMANAGER_H

#include <QMap>
#include <QDate>
#include <QTime>
#include <QVector>
#include <QMainWindow>
#include <QMessageBox>
#include "connectdatabase.h"

namespace Ui
{
class FlightManager;
}

class FlightManager : public QMainWindow
{
    Q_OBJECT

public:
    explicit FlightManager(QWidget *parent = 0);
    ~FlightManager();

public slots:
    void receive();

protected:
//    void resizeEvent(QResizeEvent*);
    void closeEvent(QCloseEvent*);

    void Init();//初始化
    int GetMaxNum(const QString& sql);//从数据库中获取数量
    void JudgeReturnValue(QLabel* label, const int& value);//判断从数据库返回客户人数量是否正确，不正确即表明数据库连接出错，则弹出错误
    bool GetCustomerType();//从数据库中获取客户类型的名称
    bool GetCustomerInfo();//从数据库中获取客户的编号
    void SetCustomer(QComboBox* combobox, const QVector<QString>& string);
    void CustomerValueChange(const int& order);
    bool GetAirPlaneChange();//从数据库中获取航线编号、城市等信息，用于更新页面上的客机信息更改
    void PlaneValueChange(const int& index);//设置更新页面中的客机更改信息
    bool GetAirLineInfo();//从数据库中获取航班信息，用于更新页面上的客机信息更改中的航班信息
    void LineChangeOnPlane(const int& index);//设置更新页面中客机中航线的更改信息
    bool GetTicket();//从数据库中获取订票信息
    void TicketValueChangeOnUpdate(const int& index);//设置更新页面中订票系统
    bool GetShip();//从数据库中获取舱位等级信息
    void LineChangeOnTicket(const int& index);//设置更新页面中订票系统中的航线更改信息
    bool GetAirWay();//从数据库中获取航空公司名
    bool GetCity();//从数据库中获取城市名，国家名以及省、州名
    void ValueChangeOnAirLine(const int& index);//设置改变更新页面中航线信息的项
    void TicketValueChangeOnDelete(const int& index);//设置删除页面中机票信息
    void SetCustomerInfoOnBook(const int& index);//在订票页面中显示选择的客户信息
    void SetCityOnBook(QComboBox* combobox, const QString& index);//在订票页面中显示城市
    void ShowAirLineOnSearch();//在查询机票页面中显示航班信息
    void ShowCustomerOnSearch();//在查询客户信息页面中显示客户信息

private slots:
    void turn2search();//查询菜单动作
    void turn2insert();//添加菜单动作
    void turn2update();//更新菜单动作
    void turn2delete();//删除菜单动作
    void turn2quit();//退出菜单动作

    void on_inserttab_tabBarClicked(int index);//点击插入面板上的标签触发的动作
    void on_newokbutton_clicked();//点击新建用户的确定按钮
    //插入面板上面标签，类似于做翻页动作
    void on_inserttab_currentChanged(int index);
    void on_chgctmnamechk_stateChanged(int arg1);
    void on_chgctmphechk_stateChanged(int arg1);
    void on_chgctmsexchk_stateChanged(int arg1);
    void on_chgctmtypchk_stateChanged(int arg1);
    void on_chgctmokbtn_clicked();
    void on_chgctmselcom_activated(int index);
    void on_chgctmselcom_activated(const QString &arg1);
    void on_chgplaneselcom_activated(int index);
    void on_chgplanelinecom_activated(int index);
    void on_chgplaneokbtn_clicked();
    void on_chgtypselcom_activated(const QString &arg1);
    void on_chgtypokbtn_clicked();
    void on_chgtktnocom_activated(int index);
    void on_chgtktlinecom_activated(int index);
    void on_chgtktokbtn_clicked();

    //更改页面中航线更改的槽
    void on_chglinecmpcek_stateChanged(int arg1);
    void on_chglinedepcek_stateChanged(int arg1);
    void on_chglinearrcek_stateChanged(int arg1);
    void on_chglinedatecek_stateChanged(int arg1);
    void on_chglinetimecek_stateChanged(int arg1);
    void on_chglineecocek_stateChanged(int arg1);
    void on_chglinebuscek_stateChanged(int arg1);
    void on_chglinedelcek_stateChanged(int arg1);
    void on_chglinenocom_activated(int index);
    void on_chglineokbtn_clicked();

    //删除页面上退票项的槽
    void on_delticketno_activated(int index);
    void on_delticketokbtn_clicked();

    void on_bktktctmno_activated(int index);

    void on_bktktdepcot_activated(const QString &arg1);

    void on_bktktarrcot_activated(const QString &arg1);

    void on_bktktarrcy_currentIndexChanged(const QString &arg1);

private:
    struct Customer
    {
        int no;
        QString name;
        int type_no;
        QString type_name;
        int discount;
        QString id;
        QString sex;
        QString phone;
    };

    struct AirPlane
    {
        int plane_no;
        QString plane_type;
        QString line_no;
        QString departure;
        QString arrive;
        QString date;
        QString time;
    };

    struct AirLine
    {
        int line_no;
        QString airway_short_name;
        QString airplane_type;
        QString departure_country;
        QString departure_city;
        QString arrive_country;
        QString arrive_city;
        QString departure_date;
        QString departure_time;
        QString arrive_time;
        int econemy_price;
        int econemy_num;
        int econemy_rest;
        int bussiness_price;
        int bussiness_num;
        int bussiness_rest;
        int deluxe_price;
        int deluxe_num;
        int deluxe_rest;
    };

    struct Ticket
    {
        int book_num;
        QString customer_name;
        int airline_no;
        QString departure;
        QString arrive;
        QString date;
        QString time;
        int ship_no;
        QString ship_name;
    };

    struct City
    {
        int country_no;
        QString country_name;
        QString province;
        QString city;
    };

    Ui::FlightManager *ui;
    ConnectDataBase* m_Connect;
    QMap<QString, int> m_TypeDiscount;//存放客户类型优惠

    QVector<QString> m_CustomerType;//存放客户类型
    QVector<QString> m_Sex;//存放性别
    QVector<QString> m_CustomerNo;//存放更改页面上客户信息的编号
    QVector<QString> m_Ship;//存放舱位等级
    QVector<QString> m_Airway;//存放航空公司
    QVector<QString> m_Country_Name;//存放国家名
    QVector<QString> m_City_Name;//存放城市名

    QVector<Customer> m_CustomerInfo;//存放更改页面上的客户信息
    QVector<AirPlane> m_PlaneInfo;//存放更改页面上客机信息中的客机信息
    QVector<AirLine> m_LineInfo;//存放更改页面客机信息中的航线信息
    QVector<Ticket> m_TicketInfo;//存放更改页面订票信息
    QVector<City> m_City;//存放城市、州、省、国家信息
};

#endif // FLIGHTMANAGER_H
