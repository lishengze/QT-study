#include "announcementform.h"
#include "ui_announcementform.h"
#include <QStandardItemModel>
#include <QDebug>

AnnouncementForm::AnnouncementForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnnouncementForm)
{
    ui->setupUi(this);
}

AnnouncementForm::AnnouncementForm(QString startDate, QString endDate, QWidget *parent):
    m_startDate(startDate), m_endDate(endDate),
    QWidget(parent),
    ui(new Ui::AnnouncementForm)
{
    ui->setupUi(this);
    initTableView();

    QString host = "192.168.211.165";
    m_database = new Database("1", host);
    setTableView();
}

void AnnouncementForm::initTableView() {
    QStandardItemModel* initModel = new QStandardItemModel();
    initModel -> setHorizontalHeaderItem (0, new QStandardItem(QObject::tr("代码")));
    initModel -> setHorizontalHeaderItem (1, new QStandardItem(QObject::tr("时间")));
    initModel -> setHorizontalHeaderItem (2, new QStandardItem(QObject::tr("公告")));
    ui->AnnouncementTableView->setModel (initModel);
    ui->AnnouncementTableView->setColumnWidth (0, 100);
    ui->AnnouncementTableView->setColumnWidth (1, 100);
    ui->AnnouncementTableView->setColumnWidth (2, 800);
    ui->AnnouncementTableView->setShowGrid (false);
}

void AnnouncementForm::setTableView() {
    QList<QString> oriSecodeList = m_database->getTableList("Announcement");
    QMap<QString, QList<QStringList>> result = m_database->getAnnouncement(oriSecodeList, m_startDate, m_endDate);
//    qDebug() << "result: " << result;

    QStandardItemModel* tableItemMode = dynamic_cast<QStandardItemModel*>(ui->AnnouncementTableView->model ());

    if (NULL != tableItemMode) {
        int row = 0;
        QList<QString> result_secodeList = result.keys();
//        qDebug() << "result_secodeList: " << result_secodeList;
        for (int i = 0; i < result_secodeList.size(); ++i) {
            QString secode = result_secodeList[i];
            for (int j = 0; j <  result[secode].size(); ++j) {
                tableItemMode->setItem (row, 0,  new QStandardItem(result[secode][j][0]));
                tableItemMode->setItem (row, 1,  new QStandardItem(result[secode][j][1]));
                tableItemMode->setItem (row, 2,  new QStandardItem(result[secode][j][2]));
                ui->AnnouncementTableView->setRowHeight (row, 20);
                ++row;
            }
        }
    }
}

AnnouncementForm::~AnnouncementForm()
{
    delete ui;
}

void AnnouncementForm::on_refreshTable_clicked()
{
   QStandardItemModel* tableItemMode = dynamic_cast<QStandardItemModel*>(ui->AnnouncementTableView->model ());
   if (NULL != tableItemMode) {
       tableItemMode->clear();
       initTableView();
   }
   setTableView();
}
