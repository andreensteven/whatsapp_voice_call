#include "proxydialog.h"
#include <QIntValidator>
#include "ui_proxydialog.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QSettings>

ProxyDialog::ProxyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProxyDialog)
{
    ui->setupUi(this);
    QIntValidator* portValidator = new QIntValidator(0, 65535, this); // Set range from 0 to 65535
    ui->proxy_port->setValidator(portValidator);
    proxy_ = ReadLocalProxy();
    ui->proxy_server->setText(proxy_.hostName());
    ui->proxy_port->setText(QString::number(proxy_.port()));
    ui->proxy_username->setText(proxy_.user());
    ui->proxy_password->setText(proxy_.password());
}

ProxyDialog::~ProxyDialog()
{
    delete ui;
}

QNetworkProxy ProxyDialog::ReadLocalProxy()
{
  QNetworkProxy proxy;
  QSettings settings("whatsapp");

  QJsonDocument doc = QJsonDocument::fromJson(settings.value("proxy").toString().toUtf8());
  QJsonObject json = doc.object();
  proxy.setType(QNetworkProxy::Socks5Proxy);
  proxy.setHostName(json["server"].toString());
  proxy.setPort(json["port"].toInt());
  proxy.setUser(json["username"].toString());
  proxy.setPassword(json["password"].toString());
  return proxy;
}

void ProxyDialog::on_ok_clicked()
{
  if (ui->proxy_server->text().isEmpty() || ui->proxy_port->text().isEmpty())
  {
    return;
  }
  proxy_.setType(QNetworkProxy::Socks5Proxy);
  proxy_.setHostName(ui->proxy_server->text());
  proxy_.setPort(ui->proxy_port->text().toInt());
  proxy_.setUser(ui->proxy_username->text());
  proxy_.setPassword(ui->proxy_password->text());
  
  QJsonObject json;
  json["server"] = ui->proxy_server->text();
  json["port"] = ui->proxy_port->text().toInt();
  json["username"] = ui->proxy_username->text();
  json["password"] = ui->proxy_password->text();
  QJsonDocument doc(json);
  QSettings settings("whatsapp");
  settings.setValue("proxy", doc.toJson());
  accept();
}

