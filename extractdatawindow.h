#ifndef EXTRACTDATAWINDOW_H
#define EXTRACTDATAWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QListWidget>

namespace Ui {
class ExtractDataWindow;
}

class ExtractDataWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ExtractDataWindow(QWidget *parent = 0);
    ExtractDataWindow(QStringList oriKeyValueList, QWidget *parent = 0);
    ~ExtractDataWindow();

    void initChoosenCombox();
    QStringList getChoosenKeyValueList();
signals:
    void getChoosenInfo_signal(QStringList, QString);

public slots:

private slots:
    void on_chooseDir_pushButton_clicked();

    void on_startExtractData_pushButton_clicked();

private:
    Ui::ExtractDataWindow *ui;

    QListWidget*            m_keyValueListWidget;
    QList<QString>          m_oriKeyValueList;
    QString                 m_dirName;
};

#endif // EXTRACTDATAWINDOW_H
