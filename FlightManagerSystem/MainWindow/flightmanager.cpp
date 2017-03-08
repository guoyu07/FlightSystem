#include "flightmanager.h"
#include "ui_flightmanager.h"

#include <cstdlib>

#include <iostream>
using std::cout;
using std::endl;

FlightManager::FlightManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FlightManager)
{
    ui->setupUi(this);
    setObjectName("FilghtManager");
    setWindowTitle("航天信息管理系统");

    m_Connect = ConnectDataBase::GetInstance();

    Init();

    //建立信号槽连接
    connect(ui->searchaction, SIGNAL(triggered(bool)), this, SLOT(turn2search()) );
    connect(ui->insertaction, SIGNAL(triggered(bool)), this, SLOT(turn2insert()) );
    connect(ui->updateaction, SIGNAL(triggered(bool)), this, SLOT(turn2update()) );
    connect(ui->deleteaction, SIGNAL(triggered(bool)), this, SLOT(turn2delete()) );
}

FlightManager::~FlightManager()
{
    delete ui;
    cout << "delete FlightManger" << endl;
}

void FlightManager::Init()
{
    m_Sex.append("男");
    m_Sex.append("女");

    ui->list->insertItem(0, "欢迎");
    ui->list->insertItem(1, "查询");
    ui->list->insertItem(2, "添加");
    ui->list->insertItem(3, "更新");
    ui->list->insertItem(4, "删除");

    ui->list->setCurrentRow(0);
    ui->stackedWidget->setCurrentIndex(0);

    //查询页面中的航班查询标签页
    ui->searchairlineshow->setColumnCount(19);
    ui->searchairlineshow->verticalHeader()->setVisible(false);
    ui->searchairlineshow->setHorizontalHeaderLabels(QStringList() << "航班号" << "航空公司"
                                                     << "客机类型" << "出发国家" << "出发城市"
                                                     << "到达国家" << "到达城市" << "出发日期"
                                                     << "出发时间" << "到达时间" << "经济舱价格"
                                                     << "经济舱座位" << "剩余经济舱票数" << "商务舱票价"
                                                     << "商务舱座位" << "剩余商务舱票数" << "头等舱票价"
                                                     << "头等舱座位" << "剩余头等舱票数");
    GetAirLineInfo();
    ShowAirLineOnSearch();
    ui->searchtoolbox->setItemText(0, "查询航班信息");

    //查询页面中客户查询标签页
    ui->searchcustomershow->setColumnCount(8);
    ui->searchcustomershow->verticalHeader()->setVisible(false);
    ui->searchcustomershow->setHorizontalHeaderLabels(QStringList() << "客户编号" << "客户姓名"
                                                      << "客户类型编号" << "客户类型" << "折扣比例"
                                                      << "身份证号码" << "性别" << "联系方式");
    GetCustomerInfo();
    ShowCustomerOnSearch();

    ui->searchtoolbox->setItemText(1, "查询客户信息");
    ui->searchtoolbox->setCurrentIndex(0);

    //添加页面中的创建客户信息标签页
    JudgeReturnValue(ui->newnoshow, GetMaxNum("SELECT * FROM vi_customernum") );
    ui->newtypeselect->setEditable(false);
    if (!m_CustomerType.size())
        GetCustomerType();
    SetCustomer(ui->newtypeselect, m_CustomerType);
    SetCustomer(ui->newsexshow, m_Sex);
    ui->inserttab->setTabText(0, "创建客户信息");

    //添加页面中的订票信息标签页
    JudgeReturnValue(ui->bktktno, GetMaxNum("SELECT * FROM vi_ticket_max") );
    if (!m_CustomerInfo.size())
        GetCustomerInfo();
    SetCustomer(ui->bktktctmno, m_CustomerNo);
    SetCustomerInfoOnBook(ui->bktktctmno->currentIndex());
    if (!m_Country_Name.size())
        GetCity();
    SetCustomer(ui->bktktdepcot, m_Country_Name);
    SetCityOnBook(ui->bktktdepcy, ui->bktktdepcot->currentText());
    SetCustomer(ui->bktktarrcot, m_Country_Name);
    SetCityOnBook(ui->bktktarrcy, ui->bktktarrcot->currentText());
    if (!m_Ship.size())
        GetShip();
    SetCustomer(ui->bktktship, m_Ship);

    ui->inserttab->setTabText(1, "订票");
    ui->inserttab->setCurrentIndex(0);

    //更新页面中的客户信息更改
    ui->chgctmnamechk->setChecked(false);
    ui->chgctmnameline->setEnabled(false);
    ui->chgctmsexchk->setChecked(false);
    ui->chgctmsexcom->setEditable(false);
    ui->chgctmsexcom->setEnabled(false);
    ui->chgctmphechk->setChecked(false);
    ui->chgctmpheline->setEnabled(false);
    ui->chgctmtypchk->setChecked(false);
    ui->chgctmtypcom->setEditable(false);
    ui->chgctmtypcom->setEnabled(false);
    if (!m_CustomerInfo.size())
        GetCustomerInfo();
    SetCustomer(ui->chgctmselcom, m_CustomerNo);
    SetCustomer(ui->chgctmsexcom, m_Sex);
    SetCustomer(ui->chgctmtypcom, m_CustomerType);
    CustomerValueChange(0);

    //更新页面中的客机信息更改
    GetAirPlaneChange();
    for (int i = 0; i < m_PlaneInfo.size(); ++i)
        ui->chgplaneselcom->addItem(QString::number(m_PlaneInfo[i].plane_no) );
    PlaneValueChange(0);
    //更新页面中客机航线信息的更改

    GetAirLineInfo();
    for (int i = 0; i < m_LineInfo.size(); ++i)
    {
        ui->chgplanelinecom->addItem(QString::number(m_LineInfo[i].line_no) );
        ui->chglinenocom->addItem(QString::number(m_LineInfo[i].line_no) );
    }
    LineChangeOnPlane(0);

    //更新页面中客户优惠类型
    SetCustomer(ui->chgtypselcom, m_CustomerType );
    ui->chgtypselcom->setCurrentIndex(0);
    ui->chgtypinline->setPlaceholderText(QString::number(m_TypeDiscount[ui->chgtypselcom->currentText()]) );

    //更新页面中的订票类型
    GetTicket();
    for (int i = 0; i < m_TicketInfo.size(); ++i)
    {
        ui->chgtktnocom->addItem(QString::number(m_TicketInfo[i].book_num) );
        ui->delticketno->addItem(QString::number(m_TicketInfo[i].book_num) );
    }
    TicketValueChangeOnUpdate(0);
    if (!m_Ship.size())
        GetShip();
    SetCustomer(ui->chgtktshipcom, m_Ship);
    GetAirLineInfo();
    for (int i = 0; i < m_LineInfo.size(); ++i)
        ui->chgtktlinecom->addItem(QString::number(m_LineInfo[i].line_no) );
    LineChangeOnTicket(ui->chgtktlinecom->currentIndex() );

    //更新页面中航线类型的更改
    GetAirLineInfo();
    ui->chglinecmpcek->setChecked(false);
    ui->chglinedepcek->setChecked(false);
    ui->chglinearrcek->setChecked(false);
    ui->chglinedatecek->setChecked(false);
    ui->chglinetimecek->setChecked(false);
    ui->chglineecocek->setChecked(false);
    ui->chglinebuscek->setChecked(false);
    ui->chglinedelcek->setChecked(false);
    ui->chglinenocom->setEditable(false);
    ui->chglinecmpcom->setEnabled(false);
    ui->chglinecmpcom->setEditable(false);
    ui->chglinedepcom->setEnabled(false);
    ui->chglinedepcom->setEditable(false);
    ui->chglinearrcom->setEnabled(false);
    ui->chglinearrcom->setEditable(false);
    ui->chglinedatedit->setEnabled(false);
    ui->chglinetimedit->setEnabled(false);
    ui->chglineecoedit->setEnabled(false);
    ui->chglinebusedit->setEnabled(false);
    ui->chglinedeledit->setEnabled(false);

    if (!m_Airway.size())
        GetAirWay();
    if (!m_City_Name.size())
        GetCity();
    SetCustomer(ui->chglinecmpcom, m_Airway);
    SetCustomer(ui->chglinedepcom, m_City_Name);
    SetCustomer(ui->chglinearrcom, m_City_Name);
    ValueChangeOnAirLine(ui->chglinenocom->currentIndex() );

    //删除页面上的退票

    //删除页面上退票项
    TicketValueChangeOnDelete(ui->delticketno->currentIndex() );
}

int FlightManager::GetMaxNum(const QString& sql)
{
    QSqlQuery* sqlquery = new QSqlQuery;
    try
    {
        if (m_Connect->SelectResult(sqlquery, sql))
            sqlquery->next();
        else
            throw false;
    }
    catch (bool&)
    {
        QMessageBox(QMessageBox::Warning, "查询失败", "数据库无法打卡, 请检查网络配置！", QMessageBox::Ok).exec();
        return -2;
    }

    return sqlquery->value(0).toInt();
}

void FlightManager::JudgeReturnValue(QLabel* label, const int& value)
{
    if (value >= 0)
        label->setText(QString::number(value + 1));
    else
        label->setText("数据库无法打开");
}

bool FlightManager::GetCustomerType()
{
    //查询得到所有用户类型名称
    QString sql = "SELECT * FROM vi_type_discount";
    QSqlQuery* sqlquery = new QSqlQuery;
    try
    {
        if (!m_Connect->SelectResult(sqlquery, sql))
            throw false;
    }
    catch (bool&)
    {
        QMessageBox(QMessageBox::Warning, "查询失败", "数据库无法打卡, 请检查网络配置！", QMessageBox::Ok).exec();
        return false;
    }

    while (sqlquery->next())
    {
        m_CustomerType.append(sqlquery->value("customertypename").toString() );
        m_TypeDiscount.insert(sqlquery->value("customertypename").toString(), sqlquery->value("discountpercent").toString().toInt() );
    }

    delete sqlquery;
    sqlquery = NULL;

    return true;
}

bool FlightManager::GetAirWay()
{
    QString sql = "SELECT airwayshortname FROM airway";
    QSqlQuery* sqlquery = new QSqlQuery;
    try
    {
        if (!m_Connect->SelectResult(sqlquery, sql))
            throw false;
    }
    catch (bool&)
    {
        QMessageBox(QMessageBox::Warning, "查询失败", "数据库无法打卡, 请检查网络配置！", QMessageBox::Ok).exec();
        return false;
    }

    while (sqlquery->next())
        m_Airway.append(sqlquery->value(0).toString());

    delete sqlquery;
    sqlquery = NULL;

    return true;
}

bool FlightManager::GetCity()
{
    QString sql = "SELECT * FROM vi_country_city";
    QSqlQuery* sqlquery = new QSqlQuery;
    try
    {
        if (!m_Connect->SelectResult(sqlquery, sql))
            throw false;
    }
    catch (bool&)
    {
        QMessageBox(QMessageBox::Warning, "查询失败", "数据库无法打卡, 请检查网络配置！", QMessageBox::Ok).exec();
        return false;
    }

    while (sqlquery->next())
    {
        City city;
        city.country_no = sqlquery->value("countryno").toString().toInt();
        city.country_name = sqlquery->value("countryname").toString();
        if (0 > m_Country_Name.indexOf(city.country_name))
            m_Country_Name.append(city.country_name);
        city.province = sqlquery->value("provincename").toString();
        city.city = sqlquery->value("cityname").toString();
        m_City_Name.append(city.city);
        m_City.append(city);
    }

    delete sqlquery;
    sqlquery = NULL;

    return true;
}

void FlightManager::SetCustomer(QComboBox* combobox, const QVector<QString>& string)
{
    for (QString str : string)
        combobox->addItem(str);
}

bool FlightManager::GetCustomerInfo()
{
    QString sql = "SELECT * FROM vi_customer_info";
    QSqlQuery* sqlquery = new QSqlQuery;
    try
    {
        if (!m_Connect->SelectResult(sqlquery, sql))
            throw false;
    }
    catch (bool&)
    {
        QMessageBox(QMessageBox::Warning, "查询失败", "数据库无法打卡, 请检查网络配置！", QMessageBox::Ok).exec();
        return false;
    }

    if (m_CustomerInfo.size())
        m_CustomerInfo.clear();
    if (m_CustomerNo.size())
        m_CustomerNo.clear();

    while (sqlquery->next())
    {
        Customer ctm;
        ctm.no = sqlquery->value("customerno").toString().toInt();
        m_CustomerNo.append(QString::number(ctm.no));
        ctm.name = sqlquery->value("customername").toString();
        ctm.type_no = sqlquery->value("customertypeno").toString().toInt();
        ctm.type_name = sqlquery->value("customertypename").toString();
        ctm.discount = sqlquery->value("discountpercent").toString().toInt();
        ctm.id = sqlquery->value("identifynum").toString();
        ctm.sex = sqlquery->value("sex").toString();
        ctm.phone = sqlquery->value("phonenum").toString();
        m_CustomerInfo.append(ctm);
    }

    delete sqlquery;
    sqlquery = NULL;

    return true;
}

void FlightManager::CustomerValueChange(const int& order)
{
    ui->chgctmnameline->setText(m_CustomerInfo[order].name);
    ui->chgctmsexcom->setCurrentText(m_CustomerInfo[order].sex);
    ui->chgctmpheline->setText(m_CustomerInfo[order].phone);
    ui->chgctmtypcom->setCurrentText(m_CustomerInfo[order].type_name);
}

bool FlightManager::GetAirPlaneChange()
{
    QString date = QDate::currentDate().toString("yyyy-MM-dd");
    QString time = QTime::currentTime().toString("hh:mm:ss");
    QString sql = QString("SELECT * FROM vi_plane_line_info where departuredate > '")
            + date + QString("' OR (departuredate = '") + date
            + QString("' and departuretime > '") + time + QString("')");
    QSqlQuery* sqlquery = new QSqlQuery;
    try
    {
        if (!m_Connect->SelectResult(sqlquery, sql))
            throw false;
    }
    catch (bool&)
    {
        QMessageBox(QMessageBox::Warning, "查询失败", "数据库无法打开, 请检查网络配置！", QMessageBox::Ok).exec();
        return false;
    }

    if (m_PlaneInfo.size())
        m_PlaneInfo.clear();

    while (sqlquery->next())
    {
        AirPlane plane;
        plane.plane_no = sqlquery->value("airplaneno").toString().toInt();
        plane.plane_type = sqlquery->value("airplanetype").toString();
        plane.line_no = sqlquery->value("airlineno").toString().toInt();
        plane.departure = sqlquery->value("departurecity").toString();
        plane.arrive = sqlquery->value("arrivecity").toString();
        plane.date = sqlquery->value("departuredate").toString();
        plane.time = sqlquery->value("departuretime").toString();
        m_PlaneInfo.append(plane);
    }

    delete sqlquery;
    sqlquery = NULL;

    return true;
}

void FlightManager::PlaneValueChange(const int& index)
{
    ui->chgplanetype->setText(m_PlaneInfo[index].plane_type);
    ui->chgplanedep->setText(m_PlaneInfo[index].departure);
    ui->chgplanearr->setText(m_PlaneInfo[index].arrive);
    ui->chgplanenotime->setText(m_PlaneInfo[index].date + "\n" + m_PlaneInfo[index].time);
}

bool FlightManager::GetAirLineInfo()
{
    QString date = QDate::currentDate().toString("yyyy-MM-dd");
    QString time = QTime::currentTime().toString("hh:mm:ss");
    QString sql = QString("SELECT * FROM vi_airline_info WHERE departuredate > '")
            + date + QString("' OR (departuredate = '") + date
            + QString("' AND departuretime > '") + time + QString("')");
    QSqlQuery* sqlquery = new QSqlQuery;
    try
    {
        if (!m_Connect->SelectResult(sqlquery, sql))
            throw false;
    }
    catch (bool&)
    {
        QMessageBox(QMessageBox::Warning, "查询失败", "数据库无法打开, 请检查网络配置！", QMessageBox::Ok).exec();
        return false;
    }

    if (m_LineInfo.size())
        m_LineInfo.clear();

    while (sqlquery->next())
    {
        AirLine line;
        line.line_no = sqlquery->value("airlineno").toString().toInt();
        line.airway_short_name = sqlquery->value("airwayshortname").toString();
        line.airplane_type = sqlquery->value("airplanetype").toString();
        line.departure_country = sqlquery->value("departurecountry").toString();
        line.departure_city = sqlquery->value("departurecity").toString();
        line.arrive_country = sqlquery->value("arrivecountry").toString();
        line.arrive_city = sqlquery->value("arrivecity").toString();
        line.departure_date = sqlquery->value("departuredate").toString();
        line.departure_time = sqlquery->value("departuretime").toString();
        line.arrive_time = sqlquery->value("arrivetime").toString();
        line.econemy_price = sqlquery->value("economyclassprice").toString().toInt();
        line.econemy_num = sqlquery->value("economyclassnum").toString().toInt();
        line.econemy_rest = line.econemy_num - m_Connect->CallPro(QString("CALL pro_count_ticket(") + QString::number(line.line_no) + ", 3)");
        line.bussiness_price = sqlquery->value("businessclassprice").toString().toInt();
        line.bussiness_num = sqlquery->value("businessclassnum").toString().toInt();
        line.bussiness_rest = line.bussiness_num - m_Connect->CallPro(QString("CALL pro_count_ticket(") + QString::number(line.line_no) + ", 2)");
        line.deluxe_price = sqlquery->value("deluxeclassprice").toString().toInt();
        line.deluxe_num = sqlquery->value("deluxeclassnum").toString().toInt();
        line.deluxe_rest = line.deluxe_num - m_Connect->CallPro(QString("CALL pro_count_ticket(") + QString::number(line.line_no) + ", 2)");
        m_LineInfo.append(line);
    }

    delete sqlquery;
    sqlquery = NULL;

    return true;
}

void FlightManager::LineChangeOnPlane(const int& index)
{
    ui->chgplanelinecpy->setText(m_LineInfo[index].airway_short_name);
    ui->chgplanelinedep->setText(m_LineInfo[index].departure_city);
    ui->chgplanelinearr->setText(m_LineInfo[index].arrive_city);
    ui->chgplanelinetime->setText(m_LineInfo[index].departure_date + "\n" + m_LineInfo[index].departure_time);
}

bool FlightManager::GetTicket()
{
    QString date = QDate::currentDate().toString("yyyy-MM-dd");
    QString time = QTime::currentTime().toString("hh:mm:ss");
    QString sql = QString("SELECT * FROM vi_ticket_change WHERE departuredate > '")
            + date + QString("' OR (departuredate = '") + date
            + QString("' AND departuretime > '") + time + QString("')");
    QSqlQuery* sqlquery = new QSqlQuery;
    try
    {
        if (!m_Connect->SelectResult(sqlquery, sql))
            throw false;
    }
    catch (bool&)
    {
        QMessageBox(QMessageBox::Warning, "查询失败", "数据库无法打开, 请检查网络配置！", QMessageBox::Ok).exec();
        return false;
    }

    if (m_TicketInfo.size())
        m_TicketInfo.clear();

    while (sqlquery->next())
    {
        Ticket ticket;
        ticket.book_num = sqlquery->value("booknum").toString().toInt();
        ticket.customer_name = sqlquery->value("customername").toString();
        ticket.airline_no = sqlquery->value("airlineno").toString().toInt();
        ticket.departure = sqlquery->value("departurecity").toString();
        ticket.arrive = sqlquery->value("arrivecity").toString();
        ticket.date = sqlquery->value("departuredate").toString();
        ticket.time = sqlquery->value("departuretime").toString();
        ticket.ship_no = sqlquery->value("shipno").toString().toInt();
        ticket.ship_name = sqlquery->value("shipname").toString();
        m_TicketInfo.append(ticket);
    }

    delete sqlquery;
    sqlquery = NULL;

    return true;
}

void FlightManager::TicketValueChangeOnUpdate(const int &index)
{
    ui->chgtktlineabl->setText(QString::number(m_TicketInfo[index].airline_no) );
    ui->chgtktdepabl->setText(m_TicketInfo[index].departure);
    ui->chgtktarrabl->setText(m_TicketInfo[index].arrive);
    ui->chgtkttimeabl->setText(m_TicketInfo[index].date + "\n" + m_TicketInfo[index].time);
    ui->chgtktshipabl->setText(m_TicketInfo[index].ship_name);
}

bool FlightManager::GetShip()
{
    QString sql = "SELECT * FROM vi_ship_name";
    QSqlQuery* sqlquery = new QSqlQuery;
    try
    {
        if (!m_Connect->SelectResult(sqlquery, sql))
            throw false;
    }
    catch (bool&)
    {
        QMessageBox(QMessageBox::Warning, "查询失败", "数据库无法打卡, 请检查网络配置！", QMessageBox::Ok).exec();
        return false;
    }

    while (sqlquery->next())
        m_Ship.append(sqlquery->value(0).toString());

    delete sqlquery;
    sqlquery = NULL;

    return true;
}

void FlightManager::LineChangeOnTicket(const int &index)
{
    ui->chgtktdepature->setText(m_LineInfo[index].departure_city);
    ui->chgtktarrive->setText(m_LineInfo[index].arrive_city);
    ui->chgtkttime->setText(m_LineInfo[index].departure_date + "\n" + m_LineInfo[index].departure_time);
}

void FlightManager::ValueChangeOnAirLine(const int& index)
{
    ui->chglinecmpcom->setCurrentText(m_LineInfo[index].airway_short_name);
    ui->chglinedepcom->setCurrentText(m_LineInfo[index].departure_city);
    ui->chglinearrcom->setCurrentText(m_LineInfo[index].arrive_city);

    QStringList datelist = m_LineInfo[index].departure_date.split("-");
    int indate[3];
    int i = 0;

    for (QString d: datelist)
    {
        indate[i] = d.toInt();
        ++i;
    }

    QDate date(indate[0], indate[1], indate[2]);
    ui->chglinedatedit->setDate(date);
    QTime time = QTime::fromString(m_LineInfo[index].departure_time);

    ui->chglinetimedit->setTime(time);
    ui->chglineecoedit->setText(QString::number(m_LineInfo[index].econemy_price));
    ui->chglinebusedit->setText(QString::number(m_LineInfo[index].bussiness_price));
    ui->chglinedeledit->setText(QString::number(m_LineInfo[index].deluxe_price));
}

void FlightManager::TicketValueChangeOnDelete(const int& index)
{
    ui->delticketname->setText(m_TicketInfo[index].customer_name);
    ui->delticketlineno->setText(QString::number(m_TicketInfo[index].airline_no));
    ui->delticketdep->setText(m_TicketInfo[index].departure);
    ui->delticketarr->setText(m_TicketInfo[index].arrive);
    ui->delticketdeptime->setText(m_TicketInfo[index].date + "\n" + m_TicketInfo[index].time);
    ui->delticketship->setText(m_TicketInfo[index].ship_name);
}

void FlightManager::SetCustomerInfoOnBook(const int& index)
{
    ui->bktktctmname->setText(m_CustomerInfo[index].name);
    ui->bktktctmtyp->setText(m_CustomerInfo[index].type_name);
    ui->bktktdiscot->setText(QString::number(m_CustomerInfo[index].discount) );
}

void FlightManager::SetCityOnBook(QComboBox* combobox, const QString &index)
{
    combobox->clear();
    for (int i = 0; i < m_City.size(); ++i)
        if (index == m_City[i].country_name)
            if ("" == m_City[i].province)
                combobox->addItem(m_City[i].city);
            else
                combobox->addItem(m_City[i].city + ", " + m_City[i].province);
}

void FlightManager::ShowAirLineOnSearch()
{
    ui->searchairlineshow->setRowCount(m_LineInfo.size());
    for (int i = 0; i < m_LineInfo.size(); ++i)
    {
        ui->searchairlineshow->setItem(i, 0, new QTableWidgetItem(QString::number(m_LineInfo[i].line_no)) );
        ui->searchairlineshow->setItem(i, 1, new QTableWidgetItem(m_LineInfo[i].airway_short_name) );
        ui->searchairlineshow->setItem(i, 2, new QTableWidgetItem(m_LineInfo[i].airplane_type) );
        ui->searchairlineshow->setItem(i, 3, new QTableWidgetItem(m_LineInfo[i].departure_country) );
        ui->searchairlineshow->setItem(i, 4, new QTableWidgetItem(m_LineInfo[i].departure_city) );
        ui->searchairlineshow->setItem(i, 5, new QTableWidgetItem(m_LineInfo[i].arrive_country) );
        ui->searchairlineshow->setItem(i, 6, new QTableWidgetItem(m_LineInfo[i].arrive_city) );
        ui->searchairlineshow->setItem(i, 7, new QTableWidgetItem(m_LineInfo[i].departure_date) );
        ui->searchairlineshow->setItem(i, 8, new QTableWidgetItem(m_LineInfo[i].departure_time) );
        ui->searchairlineshow->setItem(i, 9, new QTableWidgetItem(m_LineInfo[i].arrive_time) );
        ui->searchairlineshow->setItem(i, 10, new QTableWidgetItem(QString::number(m_LineInfo[i].econemy_price)) );
        ui->searchairlineshow->setItem(i, 11, new QTableWidgetItem(QString::number(m_LineInfo[i].econemy_num)) );
        ui->searchairlineshow->setItem(i, 12, new QTableWidgetItem(QString::number(m_LineInfo[i].econemy_rest)) );
        ui->searchairlineshow->setItem(i, 13, new QTableWidgetItem(QString::number(m_LineInfo[i].bussiness_price)) );
        ui->searchairlineshow->setItem(i, 14, new QTableWidgetItem(QString::number(m_LineInfo[i].bussiness_num)) );
        ui->searchairlineshow->setItem(i, 15, new QTableWidgetItem(QString::number(m_LineInfo[i].bussiness_rest)) );
        ui->searchairlineshow->setItem(i, 16, new QTableWidgetItem(QString::number(m_LineInfo[i].deluxe_price)) );
        ui->searchairlineshow->setItem(i, 17, new QTableWidgetItem(QString::number(m_LineInfo[i].deluxe_num)) );
        ui->searchairlineshow->setItem(i, 18, new QTableWidgetItem(QString::number(m_LineInfo[i].deluxe_rest)) );
    }
}

void FlightManager::ShowCustomerOnSearch()
{
    ui->searchcustomershow->setRowCount(m_CustomerInfo.size());
    for (int i = 0; i < m_CustomerInfo.size(); ++i)
    {
        ui->searchcustomershow->setItem(i, 0, new QTableWidgetItem(QString::number(m_CustomerInfo[i].no)) );
        ui->searchcustomershow->setItem(i, 1, new QTableWidgetItem(m_CustomerInfo[i].name) );
        ui->searchcustomershow->setItem(i, 2, new QTableWidgetItem(QString::number(m_CustomerInfo[i].type_no)) );
        ui->searchcustomershow->setItem(i, 3, new QTableWidgetItem(m_CustomerInfo[i].type_name) );
        ui->searchcustomershow->setItem(i, 4, new QTableWidgetItem(QString::number(m_CustomerInfo[i].discount)) );
        ui->searchcustomershow->setItem(i, 5, new QTableWidgetItem(m_CustomerInfo[i].id) );
        ui->searchcustomershow->setItem(i, 6, new QTableWidgetItem(m_CustomerInfo[i].sex) );
        ui->searchcustomershow->setItem(i, 7, new QTableWidgetItem(m_CustomerInfo[i].phone) );
    }
}

//void FlightManager::resizeEvent(QResizeEvent*)
//{
//    // 计算宽度和高度的缩放比例
//    qreal ratioW = width() / 852.0;
//    qreal ratioH = height() / 595.0;

//    ui->gridLayoutWidget->setGeometry(0, 0, 239 * ratioW, 570 * ratioH);//重画左边列表
//    ui->gridLayoutWidget_2->setGeometry(239 * ratioW, 0, 609 * ratioW, 570 * ratioH);//重画右边stackwidget
//    ui->welcomelabel->setGeometry(90 * ratioW, 80 * ratioH, 231 * ratioW, 51 * ratioH);//重画欢迎标签
//    ui->gridLayoutWidget_3->setGeometry(1 * ratioW, 1 * ratioH, 609 * ratioW, 570 * ratioH);//重画插入面板

//    //重画“创建客户信息板面”
//    ui->newnolabel->setGeometry(210 * ratioW, 110 * ratioH, 61 * ratioW, 41 * ratioH);
//    ui->newnoshow->setGeometry(330 * ratioW, 113 * ratioH, 121 * ratioW, 31 * ratioH);
//    ui->newnamelabel->setGeometry(210 * ratioW, 160 * ratioH,71 * ratioW, 41 * ratioH);
//    ui->newnamedit->setGeometry(330 * ratioW, 160 * ratioH, 121 * ratioW, 31 * ratioH);
//    ui->newtypelabel->setGeometry(210 * ratioW, 210 * ratioH, 71 * ratioW, 41 * ratioH);
//    ui->newtypeselect->setGeometry(330 * ratioW, 220 * ratioH, 121 * ratioW, 24 * ratioH);
//    ui->newidlabel->setGeometry(210 * ratioW, 270 * ratioH, 71 * ratioW, 31 * ratioH);
//    ui->newidedit->setGeometry(330 * ratioW, 270 * ratioH, 121 * ratioW, 24 * ratioH);
//    ui->newsexlabel->setGeometry(210 * ratioW, 320 * ratioH, 61 * ratioW, 21 * ratioH);
//    ui->newsexshow->setGeometry(330 * ratioW, 320 * ratioH, 121 * ratioW, 24 * ratioH);
//    ui->newphonelabel->setGeometry(210 * ratioW, 360 * ratioH, 61 * ratioW, 31 * ratioH);
//    ui->newphonedit->setGeometry(330 * ratioW, 370 * ratioH, 121 * ratioW, 24 * ratioH);
//    ui->newokbutton->setGeometry(480 * ratioW, 450 * ratioH, 101 * ratioW, 41 * ratioH);

//    //重画“更新面板”
//    ui->layoutWidget->setGeometry(0, 0, 669 * ratioW, 570 * ratioH);
//    //重画“更新客户信息面板”
//    ui->layoutWidget1->setGeometry(4 * ratioW, 60 * ratioH, 327 * ratioW, 140 * ratioH);//重画需更改栏目的布局
//    ui->widget->setGeometry(1 * ratioW, 32 * ratioH, 327 * ratioW, 27 * ratioH);//重画“标签 选择 确定”按钮的布局

//    //重画“更新客机航线信息面板”

//    //重画“更新客户类型优惠信息面板”

//}

void FlightManager::closeEvent(QCloseEvent *)
{
    cout << "Close FlightManger" << endl;
}

//公共槽，用于接收登陆界面的信号，接收到信号后就显示主界面
void FlightManager::receive()
{
    cout << "FlightManager Show" << endl;
    show();
}

//菜单栏动作
void FlightManager::turn2search()
{
    ui->list->setCurrentRow(1);
    ui->stackedWidget->setCurrentIndex(1);
}

void FlightManager::turn2insert()
{
    ui->list->setCurrentRow(2);
    ui->stackedWidget->setCurrentIndex(2);
}

void FlightManager::turn2update()
{
    ui->list->setCurrentRow(3);
    ui->stackedWidget->setCurrentIndex(3);
}

void FlightManager::turn2delete()
{
    ui->list->setCurrentRow(4);
    ui->stackedWidget->setCurrentIndex(4);
}

void FlightManager::turn2quit()
{
    exit(0);
}

void FlightManager::on_inserttab_tabBarClicked(int index)
{
    if (0 == index)
    {
        JudgeReturnValue(ui->newnoshow, GetMaxNum("SELECT * FROM vi_customernum"));
        ui->newtypeselect->clear();
        if (!m_CustomerType.size())
            GetCustomerType();
        for (QString str : m_CustomerType)
            ui->newtypeselect->addItem(str);
    }
}

void FlightManager::on_newokbutton_clicked()
{
    QVector<QString> insertstr;

    QString no = ui->newnoshow->text();
    insertstr.append(no);
    QString name = ui->newnamedit->text();
    insertstr.append(name);
    QString type = QString::number(ui->newtypeselect->currentIndex() + 1);
    insertstr.append(type);
    QString id = ui->newidedit->text();
    insertstr.append(id);
    QString sex = ui->newsexshow->currentText();
    insertstr.append(sex);
    QString phone = ui->newphonedit->text();
    insertstr.append(phone);

    QString sql = "INSERT INTO customer VALUES (?, ?, ?, ?, ?, ?)";
    QString rtn = m_Connect->InsertValue(sql, 6, insertstr);
    try
    {
        if ("Success" != rtn)
            throw false;
        QMessageBox(QMessageBox::Information, "添加成功", "恭喜，成功新添一名客户", QMessageBox::Ok).exec();

        ui->newnoshow->setText(QString::number(no.toInt() + 1));
        ui->newnamedit->clear();
        ui->newtypeselect->setCurrentIndex(0);
        ui->newidedit->clear();
        ui->newphonedit->clear();
    }
    catch (bool&)
    {
        QMessageBox(QMessageBox::Warning, "插入失败", rtn, QMessageBox::Ok).exec();
    }
}

void FlightManager::on_inserttab_currentChanged(int index)
{
    if (0 == index)
    {
        ui->newnamedit->clear();
        ui->newtypeselect->setCurrentIndex(0);
        ui->newidedit->clear();
        ui->newphonedit->clear();
    }
}

void FlightManager::on_chgctmnamechk_stateChanged(int arg1)
{
    if (arg1)
        ui->chgctmnameline->setEnabled(true);
    else
        ui->chgctmnameline->setEnabled(false);
}

void FlightManager::on_chgctmphechk_stateChanged(int arg1)
{
    if (arg1)
        ui->chgctmpheline->setEnabled(true);
    else
        ui->chgctmpheline->setEnabled(false);
}

void FlightManager::on_chgctmsexchk_stateChanged(int arg1)
{
    if (arg1)
        ui->chgctmsexcom->setEnabled(true);
    else
        ui->chgctmsexcom->setEnabled(false);
}

void FlightManager::on_chgctmtypchk_stateChanged(int arg1)
{
    if (arg1)
        ui->chgctmtypcom->setEnabled(true);
    else
        ui->chgctmtypcom->setEnabled(false);
}

void FlightManager::on_chgctmokbtn_clicked()
{
    bool chg_name = ui->chgctmnamechk->isChecked();
    bool chg_phone = ui->chgctmphechk->isChecked();
    bool chg_sex = ui->chgctmsexchk->isChecked();
    bool chg_tye = ui->chgctmtypchk->isChecked();

    if (!chg_name && !chg_phone && !chg_sex && !chg_tye)
    {
        QMessageBox(QMessageBox::Warning, "更新错误", "请至少选择一项更新项", QMessageBox::Ok).exec();
        return;
    }

    QString name = ui->chgctmnameline->text();
    QString phone = ui->chgctmpheline->text();
    QString sex = ui->chgctmsexcom->currentText();
    int type = ui->chgctmtypcom->currentIndex() + 1;

    QString ctmno = ui->chgctmselcom->currentText();
    QString udtname = QString("UPDATE customer SET customername = '") + name + QString("' WHERE customerno = ") + ctmno;
    QString udtphe = QString("UPDATE customer SET phonenum = '") + phone + QString("' WHERE customerno = ") + ctmno;
    QString udtsex = QString("UPDATE customer SET sex = '") + sex + QString("' WHERE customerno = ") + ctmno;
    QString udttye = QString("UPDATE customer SET customertypeno = '") + QString::number(type) + QString("' WHERE customerno = ") + ctmno;

    if (!chg_name)
        udtname = "";
    if (!chg_phone)
        udtphe = "";
    if (!chg_sex)
        udtsex = "";
    if (!chg_tye)
        udttye = "";

    QVector<QString> udtsql;
    udtsql.append(udtname);
    udtsql.append(udtphe);
    udtsql.append(udtsex);
    udtsql.append(udttye);
    QString rtn = m_Connect->UpdateValue(udtsql);
    if (rtn == "Success")
        QMessageBox(QMessageBox::Information, "成功", "更新成功", QMessageBox::Ok).exec();
    else
        QMessageBox(QMessageBox::Critical, "更新错误", rtn, QMessageBox::Ok).exec();
}

void FlightManager::on_chgctmselcom_activated(int index)
{
    CustomerValueChange(index);
}

void FlightManager::on_chgctmselcom_activated(const QString &arg1)
{
    GetCustomerInfo();
}

void FlightManager::on_chgplaneselcom_activated(int index)
{
    PlaneValueChange(index);
}

void FlightManager::on_chgplanelinecom_activated(int index)
{
    LineChangeOnPlane(index);
}

void FlightManager::on_chgplaneokbtn_clicked()
{
    QString plane_no = ui->chgplaneselcom->currentText();
    QString chg_line = ui->chgplanelinecom->currentText();
    QString sql = "UPDATE airplane SET airlineno = " + chg_line + " WHERE airplaneno = " + plane_no;

    QVector<QString> plane_sql;
    plane_sql.append(sql);
    QString rtn = m_Connect->UpdateValue(plane_sql);
    if (rtn == "Success")
        QMessageBox(QMessageBox::Information, "成功", "更新成功", QMessageBox::Ok).exec();
    else
        QMessageBox(QMessageBox::Critical, "更新错误", rtn, QMessageBox::Ok).exec();
}

void FlightManager::on_chgtypselcom_activated(const QString &arg1)
{
    ui->chgtypinline->setPlaceholderText(QString::number(m_TypeDiscount[arg1]) );
}

void FlightManager::on_chgtypokbtn_clicked()
{
    int discount = ui->chgtypinline->text().toInt();
    if (discount < 0 || discount > 90)
    {
        QMessageBox(QMessageBox::Critical, "违法输入", "只可以输入0-90之间的数字", QMessageBox::Ok).exec();
        return;
    }
    else
    {
        QString sql = "UPDATE customertype SET discountpercent = " + QString::number(discount)
                + " WHERE customertypename = '" + ui->chgtypselcom->currentText() + "'";
        QVector<QString> discount_sql;
        discount_sql.append(sql);
        QString rtn = m_Connect->UpdateValue(discount_sql);
        if (rtn == "Success")
            QMessageBox(QMessageBox::Information, "成功", "更新成功", QMessageBox::Ok).exec();
        else
            QMessageBox(QMessageBox::Critical, "更新错误", rtn, QMessageBox::Ok).exec();
    }
}

void FlightManager::on_chgtktnocom_activated(int index)
{
    TicketValueChangeOnUpdate(index);
}

void FlightManager::on_chgtktlinecom_activated(int index)
{
    LineChangeOnTicket(index);
}

void FlightManager::on_chgtktokbtn_clicked()
{
    int book_num = ui->chgtktnocom->currentText().toInt();
    int line = ui->chgtktlinecom->currentText().toInt();
    int ship = ui->chgtktshipcom->currentIndex() + 1;

    bool chg_line = line != m_TicketInfo[book_num].airline_no ? true : false;
    bool chg_ship = ship != m_TicketInfo[book_num].ship_no ? true :false;

    if (!chg_line && !chg_ship)
    {
        QMessageBox(QMessageBox::Warning, "更新错误", "请选择不同值进行更新", QMessageBox::Ok).exec();
        return;
    }
    else
    {
        QString search_line_ship = "SELECT * FROM vi_line_4_ticketchange WHERE airlineno =" + QString::number(line);
        QSqlQuery* sqlquery = new QSqlQuery;
        try
        {
            if (!m_Connect->SelectResult(sqlquery, search_line_ship))
                throw false;
        }
        catch (bool&)
        {
            QMessageBox(QMessageBox::Warning, "查询失败", "数据库无法打卡, 请检查网络配置！", QMessageBox::Ok).exec();
            return;
        }

        sqlquery->next();
        int departure_country = sqlquery->value("departurecountry").toString().toInt();
        QString departure_city = sqlquery->value("departurecity").toString();
        int arrive_country = sqlquery->value("arrivecountry").toString().toInt();
        QString arrive_city = sqlquery->value("arrivecity").toString();
        int price = 0;
        switch (ship)
        {
        case 1: price = sqlquery->value("deluxeclassprice").toString().toInt(); break;
        case 2: price = sqlquery->value("businessclassprice").toString().toInt(); break;
        case 3: price = sqlquery->value("economyclassprice").toString().toInt(); break;
        }

        delete sqlquery;
        sqlquery = NULL;

        QVector<QString> vec_sql;

        if (!chg_line && chg_ship)
        {
            QString ship_sql = "UPDATE bookticket SET shipno = " + QString::number(ship)
                    + ", ticketprice = " + QString::number(price) + " WHERE booknum = "
                    + QString::number(book_num);
            vec_sql.append(ship_sql);
        }
        else
        {
            QString line_sql = "UPDATE bookticket SET airlineno = " + QString::number(line)
                    + ", departurecountry = " + QString::number(departure_country)
                    + ", departurecity = '" + departure_city
                    + "', arrivecountry = " + QString::number(arrive_country)
                    + ", arrivecity = '" + arrive_city
                    + "', shipno = " + QString::number(ship)
                    + ", ticketprice = " + QString::number(price)
                    + " WHERE booknum = " + QString::number(book_num);
            vec_sql.append(line_sql);
        }

        QString rtn = m_Connect->UpdateValue(vec_sql);
        if (rtn == "Success")
            QMessageBox(QMessageBox::Information, "成功", "更新成功", QMessageBox::Ok).exec();
        else
            QMessageBox(QMessageBox::Critical, "更新错误", rtn, QMessageBox::Ok).exec();
    }
}

void FlightManager::on_chglinecmpcek_stateChanged(int arg1)
{
    if (arg1)
        ui->chglinecmpcom->setEnabled(true);
    else
        ui->chglinecmpcom->setEnabled(false);
}

void FlightManager::on_chglinedepcek_stateChanged(int arg1)
{
    if (arg1)
        ui->chglinedepcom->setEnabled(true);
    else
        ui->chglinedepcom->setEnabled(false);
}

void FlightManager::on_chglinearrcek_stateChanged(int arg1)
{
    if(arg1)
        ui->chglinearrcom->setEnabled(true);
    else
        ui->chglinearrcom->setEnabled(false);
}

void FlightManager::on_chglinedatecek_stateChanged(int arg1)
{
    if (arg1)
        ui->chglinedatedit->setEnabled(true);
    else
        ui->chglinedatedit->setEnabled(false);
}

void FlightManager::on_chglinetimecek_stateChanged(int arg1)
{
    if (arg1)
        ui->chglinetimedit->setEnabled(true);
    else
        ui->chglinetimedit->setEnabled(false);
}

void FlightManager::on_chglineecocek_stateChanged(int arg1)
{
    if (arg1)
        ui->chglineecoedit->setEnabled(true);
    else
        ui->chglineecoedit->setEnabled(false);
}

void FlightManager::on_chglinebuscek_stateChanged(int arg1)
{
    if (arg1)
        ui->chglinebusedit->setEnabled(true);
    else
        ui->chglinebusedit->setEnabled(false);
}

void FlightManager::on_chglinedelcek_stateChanged(int arg1)
{
    if (arg1)
        ui->chglinedeledit->setEnabled(true);
    else
        ui->chglinedeledit->setEnabled(false);
}

void FlightManager::on_chglinenocom_activated(int index)
{
    ValueChangeOnAirLine(index);
}

void FlightManager::on_chglineokbtn_clicked()
{
    bool chg_way = ui->chglinecmpcek->isChecked();
    bool chg_dep = ui->chglinedepcek->isChecked();
    bool chg_arr = ui->chglinearrcek->isChecked();
    bool chg_date = ui->chglinedatecek->isChecked();
    bool chg_time = ui->chglinetimecek->isChecked();
    bool chg_econ = ui->chglineecocek->isChecked();
    bool chg_bus = ui->chglinebuscek->isChecked();
    bool chg_del = ui->chglinedelcek->isChecked();

    if (!chg_way && !chg_dep && !chg_arr && !chg_date && !chg_time && !chg_econ && !chg_bus && !chg_del)
    {
        QMessageBox(QMessageBox::Warning, "更新错误", "请至少选择一项更新项", QMessageBox::Ok).exec();
        return;
    }

    QString way = ui->chglinecmpcom->currentText();
    QString dep = ui->chglinedepcom->currentText();
    QString arr = ui->chglinearrcom->currentText();
    QString time = ui->chglinetimedit->text();
    QString econ = ui->chglineecoedit->text();
    QString bus = ui->chglinebusedit->text();
    QString del = ui->chglinedeledit->text();

    QString d = ui->chglinedatedit->text();
    QStringList datelist = d.split("/");
    QString indate[3];
    int i = 0;

    for (QString dt : datelist)
    {
        indate[i] = dt;
        ++i;
    }

    QString date = QString("20") + indate[2] + "-" + indate[0] + "-" + indate[1];

    QString line_no = ui->chglinenocom->currentText();
    QString udtway = QString("UPDATE airline SET airwayshortname = '") + way + "' WHERE airlineno = " + line_no;
    QString udtdep = QString("UPDATE airline SET departurecity = '") + dep + "' WHERE airlineno = " + line_no;
    QString udtarr = QString("UPDATE airline SET arrivecity = '") + arr + "' WHERE airlineno = " + line_no;
    QString udtdate = QString("UPDATE airline SET departuredate = '") + date + "' WHERE airlineno = " + line_no;
    QString udttime = QString("UPDATE airline SET departuretime = '") + time + "' WHERE airlineno = " + line_no;
    QString udtecon = QString("UPDATE airline SET economyclassprice = ") + econ + " WHERE airlineno = " + line_no;
    QString udtbus = QString("UPDATE airline SET businessclassprice = ") + bus + " WHERE airlineno = " + line_no;
    QString udtdel = QString("UPDATE airline SET deluxeclassprice = ") + del + " WHERE airlineno = " + line_no;

    if (!chg_way)
        udtway = "";
    if (!chg_dep)
        udtdep = "";
    if (!chg_arr)
        udtarr = "";
    if (!chg_date)
        udtdate = "";
    if (!chg_time)
        udttime = "";
    if (!chg_econ)
        udtecon = "";
    if (!chg_bus)
        udtbus = "";
    if (!chg_del)
        udtdel = "";

    QVector<QString> udtsql;
    udtsql.append(udtway);
    udtsql.append(udtdep);
    udtsql.append(udtarr);
    udtsql.append(udtdate);
    udtsql.append(udttime);
    udtsql.append(udtecon);
    udtsql.append(udtbus);
    udtsql.append(udtdel);

    QString rtn = m_Connect->UpdateValue(udtsql);
    if (rtn == "Success")
        QMessageBox(QMessageBox::Information, "成功", "更新成功", QMessageBox::Ok).exec();
    else
        QMessageBox(QMessageBox::Critical, "更新错误", rtn, QMessageBox::Ok).exec();
}

void FlightManager::on_delticketno_activated(int index)
{
    TicketValueChangeOnDelete(index);
}

void FlightManager::on_delticketokbtn_clicked()
{
    QString ticket_no = ui->delticketno->currentText();
    QString sql = "DELETE FROM bookticket WHERE booknum = " + ticket_no;

    QString rtn = m_Connect->DeletValue(sql);
    if (rtn == "Success")
        QMessageBox(QMessageBox::Information, "成功", "删除成功", QMessageBox::Ok).exec();
    else
        QMessageBox(QMessageBox::Critical, "删除错误", rtn, QMessageBox::Ok).exec();
}

void FlightManager::on_bktktctmno_activated(int index)
{
    SetCustomerInfoOnBook(index);
}

void FlightManager::on_bktktdepcot_activated(const QString &arg1)
{
    SetCityOnBook(ui->bktktdepcy, arg1);
}

void FlightManager::on_bktktarrcot_activated(const QString &arg1)
{
    SetCityOnBook(ui->bktktarrcy, arg1);
}

void FlightManager::on_bktktarrcy_currentIndexChanged(const QString &arg1)
{
    QString dep = ui->bktktdepcy->currentText();
    QStringList d = dep.split(",", QString::SkipEmptyParts);
    QString dt[2];
    int i = 0;

    for (QString str : d)
    {
        dt[i] = str;
        ++i;
    }

    QString departure = dt[0];

    QStringList a = arg1.split(",", QString::SkipEmptyParts);
    i = 0;

    for (QString str : a)
    {
        dt[i] = str;
        ++i;
    }

    QString arrive = dt[0];

    GetAirLineInfo();
    ui->bktktline->clear();

    for (AirLine airline : m_LineInfo)
        if (departure == airline.departure_city && arrive == airline.arrive_city)
            ui->bktktline->addItem(QString::number(airline.line_no));
}
