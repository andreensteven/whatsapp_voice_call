#include "qrcode_login.h"
#include "ui_qrcode_login.h"
#include "voice_command_def.h"
#include <QJsonDocument>
#include "record_export.h"
#include <QImage>

QrcodeLogin::QrcodeLogin(QNetworkProxy proxy, QString uuid, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::QrcodeLogin), proxy_(proxy), uuid_(uuid)
{
    ui->setupUi(this);

    connect(&voice_client_socket_, &QWebSocket::connected, this, &QrcodeLogin::OnWebsocketConnected);
    connect(&voice_client_socket_, &QWebSocket::disconnected, this, &QrcodeLogin::OnWebsocketDisconnected);
    connect(&voice_client_socket_, &QWebSocket::textMessageReceived, this, &QrcodeLogin::OnWebsocketTextMessageReceived);
    connect(&voice_client_socket_, &QWebSocket::binaryMessageReceived, this, &QrcodeLogin::OnWebsocketBinaryMessageReceived);
    on_refresh_btn_clicked();
}

QrcodeLogin::~QrcodeLogin()
{
  voice_client_socket_.close();
    delete ui;
}

void QrcodeLogin::OnWebsocketConnected()
{
  QJsonObject json;
  json["type"] = CommandType_Connect;
  json["role"] = "client";
  json["uuid"] = uuid_;

  json["proxy_server"] = proxy_.hostName();
  json["proxy_port"] = proxy_.port();
  json["proxy_username"] = proxy_.user();
  json["proxy_password"] = proxy_.password();

  voice_client_socket_.sendTextMessage(QJsonDocument(json).toJson());
}

void QrcodeLogin::OnWebsocketDisconnected()
{
  ui->refresh_btn->setEnabled(true);
  ui->qrcode->setPixmap(QPixmap());
}

void QrcodeLogin::OnWebsocketTextMessageReceived(const QString& message)
{
  qDebug() << "OnWebsocketTextMessageReceived:" << message;
  QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
  QJsonObject command = doc.object();
  int type = command["type"].toInt();
  switch (type)
  {
  case CommandType_QRCode: {
    ShowQRCode(command);
    break;
  }
  case CommandType_Login: {
    HandleLogin(command);
  }
   break;
  }
}

void QrcodeLogin::ShowQRCode(const QJsonObject& command) {
  QString qrcode = command["qrcode"].toString();
  std::string code = WriteBarcode(qrcode.toStdString(), 250, 250);
  ui->qrcode->setPixmap(QPixmap::fromImage(QImage((const quint8*)code.data(), 250, 250, 250, QImage::Format::Format_Grayscale8).copy()));
}

void QrcodeLogin::HideQRCode()
{
  ui->refresh_btn->setEnabled(true);
  ui->qrcode->setPixmap(QPixmap());
  voice_client_socket_.close();
}

void QrcodeLogin::HandleLogin(const QJsonObject& command)
{
  LoginCode code = (LoginCode)command["ret"].toInt();
  switch (code)
  {
  case LoginCode_Success:
  case LoginCode_Handshake_Failed:
  case LoginCode_Websocket_Disconnected:
  case LoginCode_QRCode_Expired:
  case LoginCode_Login_Failed:
    HideQRCode();
    break;
  case LoginCode_QRCode_Relogin:
    accept();
    break;
  default:
    break;
  }
}

void QrcodeLogin::OnWebsocketBinaryMessageReceived(const QByteArray& message)
{

}

void QrcodeLogin::on_refresh_btn_clicked()
{
  QUrl url(QStringLiteral("ws://47.243.17.172:8182"));
  voice_client_socket_.open(url);
  ui->refresh_btn->setEnabled(false);
}

