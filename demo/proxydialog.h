#ifndef PROXYDIALOG_H
#define PROXYDIALOG_H

#include <QDialog>
#include <QNetworkProxy>

namespace Ui {
class ProxyDialog;
}

class ProxyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProxyDialog(QWidget *parent = nullptr);
    ~ProxyDialog();

    static QNetworkProxy ReadLocalProxy();

    const QNetworkProxy& proxy() const { return proxy_; }

private slots:
    void on_ok_clicked();

private:
    Ui::ProxyDialog *ui;
    QNetworkProxy proxy_;
};

#endif // PROXYDIALOG_H
