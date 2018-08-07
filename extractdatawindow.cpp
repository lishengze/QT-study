#include "extractdatawindow.h"
#include "ui_extractdatawindow.h"
#include <QCheckBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#pragma execution_character_set("utf-8")

ExtractDataWindow::ExtractDataWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ExtractDataWindow)
{
    ui->setupUi(this);
}

ExtractDataWindow::ExtractDataWindow(QStringList oriKeyValueList, QWidget *parent):
    m_oriKeyValueList(oriKeyValueList),
    QMainWindow(parent),
    ui(new Ui::ExtractDataWindow)
{
    ui->setupUi(this);
    m_dirName = "";
//    qDebug() << "m_oriKeyValueList: " << m_oriKeyValueList;
    initChoosenCombox();
}

void ExtractDataWindow::initChoosenCombox() {
    m_keyValueListWidget = new QListWidget(this);
    for (int i = 0; i < m_oriKeyValueList.size(); ++i) {
        qDebug() << m_oriKeyValueList[i];
        QListWidgetItem *pItem = new QListWidgetItem(m_keyValueListWidget);
        pItem->setData(Qt::UserRole, m_oriKeyValueList[i]);

        m_keyValueListWidget->addItem(pItem);
        QCheckBox *pCheckBox = new QCheckBox(this);
        pCheckBox->setText(m_oriKeyValueList[i]);

        m_keyValueListWidget->addItem(pItem);
        m_keyValueListWidget->setItemWidget(pItem, pCheckBox);
    }
    ui->chooseValue_comboBox->setModel(m_keyValueListWidget->model());
    ui->chooseValue_comboBox->setView(m_keyValueListWidget);
}

QStringList ExtractDataWindow::getChoosenKeyValueList() {
    QStringList result;
    for (int i = 0; i < m_keyValueListWidget->count(); ++i)
    {
        QListWidgetItem *pItem = m_keyValueListWidget->item(i);
        QWidget *pWidget = m_keyValueListWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {
            QString strText = pCheckBox->text();
            result.append(strText);
        }
    }
    return result;
}

ExtractDataWindow::~ExtractDataWindow()
{
    qDebug() << "~ExtractDataWindow";
    delete ui;
}

void ExtractDataWindow::on_chooseDir_pushButton_clicked()
{
    QString file_name = QFileDialog::getExistingDirectory(NULL,"caption",".");
    if (file_name == "") return;
    m_dirName = QString("%1/").arg(file_name);
}

void ExtractDataWindow::on_startExtractData_pushButton_clicked()
{
    QStringList choosenKeyValueList = getChoosenKeyValueList();
    if (choosenKeyValueList.size() == 0) {
        QMessageBox::critical(NULL, "Error", QString(QString("还未选择指标")));
        return;
    }

    if (m_dirName == "") {
        QMessageBox::critical(NULL, "Error", QString(QString("还未选择文件夹")));
        return;
    }

    QString info = QString("选择的指标为: \n%1\n选择的文件夹为: %2").arg(choosenKeyValueList.join("\n")).arg(m_dirName);

    if (QMessageBox::Yes == QMessageBox::information(NULL, QString("确认信息"),
                                                     info, QMessageBox::Yes | QMessageBox::No)) {
        emit getChoosenInfo_signal(choosenKeyValueList, m_dirName);
        this->close();
    }
}
