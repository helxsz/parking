#ifndef WEBDIALOG_H
#define WEBDIALOG_H

#include <QtWebKitWidgets>
#include <QDialog>

namespace Ui {
class WebDialog;
}

class WebDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WebDialog(QWidget *parent = 0);
    ~WebDialog();

private:
    Ui::WebDialog *ui;

};

#endif // WEBDIALOG_H
