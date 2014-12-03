#include "webdialog.h"
#include "ui_webdialog.h"


WebDialog::WebDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WebDialog)
{

    ui->setupUi(this);

    ui->webView->load(QUrl("http://localhost:8080/camera2.html"));
    ui->webView->show();
}

WebDialog::~WebDialog()
{
    delete ui;
}
