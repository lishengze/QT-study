#ifndef ANNOUNCEMENTFORM_H
#define ANNOUNCEMENTFORM_H

#include <QWidget>
#include "database.h"
namespace Ui {
class AnnouncementForm;
}

class AnnouncementForm : public QWidget
{
    Q_OBJECT

public:
    explicit AnnouncementForm(QWidget *parent = 0);
    AnnouncementForm(QString startDate, QString endDate, QWidget *parent = 0);

    void setTableView();
    void initTableView();

    ~AnnouncementForm();

private slots:
    void on_refreshTable_clicked();

private:
    Ui::AnnouncementForm *ui;
    QString m_startDate;
    QString m_endDate;
    Database* m_database;
};

#endif // ANNOUNCEMENTFORM_H
