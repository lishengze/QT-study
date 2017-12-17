#include <QMessageBox>
#include <QDebug>
#include <iostream>
#include "test.h"

using namespace std;

#include "dialog.h"
#include "ui_dialog.h"
#include "WAPIWrapperCpp.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    m_testObj = new Test();
    m_testObj->moveToThread(&m_thread);
    connect(this, SIGNAL(startLogin()), m_testObj, SLOT(login()));

    m_thread.start();
}

Dialog::~Dialog()
{
    m_thread.quit();
    m_thread.wait();
    delete ui;
}

int Dialog::login() {
    qDebug() << "Logining...... ";
    int errcode = CWAPIWrapperCpp::start();
    qDebug() << "Login errcode: " << errcode;
    if (0 == errcode) {
        QMessageBox::information(this, "Login", "login successfully");
        cout << "login successfully" << endl;
    }else {
        WCHAR buffer[128];
        int bufferSize = 128;
        CWAPIWrapperCpp::getErrorMsg(errcode, eCHN, buffer, bufferSize);
        cout << buffer << endl;
        std::wcout.imbue(std::locale("chs"));
        std::wcout << buffer << std::endl;
        return 0;
    }
    return errcode;
}


int Dialog::testWset() {
    WindData wd;
    LPCWSTR reportname = TEXT("sectorconstituent");
    LPCWSTR options = TEXT("date=2017-11-07;sectorid=a001010100000000");
    int errcode = CWAPIWrapperCpp::wset(wd,reportname,options);
    qDebug() << "errcode: " << errcode;
    if (0 == errcode)
    {
        int codelen = wd.GetCodesLength();
        int fieldslen = wd.GetFieldsLength();
        int colnum = fieldslen + 1;
        std::cout << "No.       ";
        for (int i =1;i < colnum; ++i)
        {
            LPCWSTR outfields = wd.GetFieldsByIndex(i-1);
            qDebug() << outfields;
//            std::wcout << outfields.GetString();
//            std::wcout << "     ";
        }
        std::wcout <<std::endl;
        for (int i = 0; i < codelen; ++i)
        {
            LPCWSTR codes = wd.GetCodeByIndex(i);
            qDebug() << codes;
//            std::wcout << codes.GetString();
//            std::wcout << "  ";
            for (int j = 0; j < fieldslen; ++j)
            {
                VARIANT var;
                wd.GetDataItem(0,i,j,var);
//                LPCWSTR temp = toString(&var);
//                std::wcout.imbue(std::locale("chs"));
//                std::wcout << temp.GetString();
//                std::wcout << "  ";

            }
//            std::wcout << std::endl;
        }
        return 0;

    }
    else
    {
        WCHAR buffer[128];
        int bufferSize = 128;
        CWAPIWrapperCpp::getErrorMsg(errcode, eCHN, buffer, bufferSize);
        qDebug() << buffer;
        std::wcout.imbue(std::locale("chs"));
        std::wcout << buffer << std::endl;
        return 0;
    }
}

void Dialog::on_testWset_clicked()
{
}

void Dialog::on_loginButton_clicked()
{
    emit startLogin();
}
