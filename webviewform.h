#ifndef WEBVIEWFORM_H
#define WEBVIEWFORM_H

#include <QWidget>

namespace Ui {
class WebViewForm;
}

class WebViewForm : public QWidget
{
    Q_OBJECT

public:
    explicit WebViewForm(QWidget *parent = 0);
    ~WebViewForm();

private:
    Ui::WebViewForm *ui;
};

#endif // WEBVIEWFORM_H
