#include "dialog.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "voice_command_def.h"
#include "./ui_dialog.h"
#include "proxydialog.h"
#include <QSettings>
#ifdef WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif // OS_WIN
#include <QMessageBox>
#include "qrcode_login.h"

#define MAX_DATA 20



Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    opus_player_ = RecordApi::Create();
    init_code_ = opus_player_->Init([this]() -> std::string {
      return HandleOutData();
      }, [this](const char* buffer, int size) {
        HandleRecordData(buffer, size);
      });

    connect(this, &Dialog::SendWebsocket, this, [this](const QByteArray& pb) {
      voice_client_socket_.sendBinaryMessage(pb);
    }, Qt::QueuedConnection);

    connect(&voice_client_socket_, &QWebSocket::connected, this, &Dialog::OnWebsocketConnected);
    connect(&voice_client_socket_, &QWebSocket::disconnected, this, &Dialog::OnWebsocketDisconnected);
    connect(&voice_client_socket_, &QWebSocket::textMessageReceived, this, &Dialog::OnWebsocketTextMessageReceived);
    connect(&voice_client_socket_, &QWebSocket::binaryMessageReceived, this, &Dialog::OnWebsocketBinaryMessageReceived);
    proxy_ = ProxyDialog::ReadLocalProxy();
    QSettings settings("whatsapp");
    ui->proxy_checkbox->setChecked(settings.value("enable_proxy").toBool());
    ui->bussiness_checkbox->setChecked(settings.value("bussiness").toBool());
    ui->account->setText(settings.value("account").toString());

    if (0 != init_code_) {
      ui->output->append("\n\nThe device(Microphone and speaker) initialization has failed. Please restart the software and try again.");
    }
}

Dialog::~Dialog()
{
  voice_client_socket_.close();
    delete ui;
}


std::string Dialog::HandleOutData()
{
  std::string data;
  {
    std::lock_guard lock_guard(mutex_);
    if (!frame_map_.empty())
    {
      data = std::move(frame_map_.begin()->second);
      frame_map_.erase(frame_map_.begin());
    }
  }
  return data;
}

void Dialog::HandleRecordData(const char* data, int len)
{
  if (send_audio_)
  {
    emit SendWebsocket(QByteArray(data, len));
  }
}

void Dialog::OnWebsocketConnected()
{
  ui->output->append("connected...");
  ui->output->append("login...");
  
  QJsonObject json;
  json["type"] = CommandType_Connect;
  json["role"] = "client";
  json["uuid"] = ui->account->text();
  if (ui->proxy_checkbox->isChecked())
  {
    json["proxy_server"] = proxy_.hostName();
    json["proxy_port"] = proxy_.port();
    json["proxy_username"] = proxy_.user();
    json["proxy_password"] = proxy_.password();
  }

  json["bussiness"] = ui->bussiness_checkbox->isChecked();
  json["reset"] = reset_connection_;
  voice_client_socket_.sendTextMessage(QJsonDocument(json).toJson());
}

void Dialog::OnWebsocketDisconnected()
{
  send_audio_ = false;
  ui->output->append("disconnected");
  ui->login_btn->setEnabled(true);
  ui->account->setEnabled(true);
}

void Dialog::OnWebsocketTextMessageReceived(const QString& message)
{
  QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
  QJsonObject command = doc.object();
  int type = command["type"].toInt();
  switch (type)
  {
  case CommandType_SendAudio: {
    send_audio_ = command["enable"].toBool();
    if (init_code_ != 0)
    {
      ui->output->append("The Microphone and speaker initialization have failed. Please restart the software and try again.");
    }
    break;
  }
  }
  ui->output->append(message);
}

void Dialog::PushFrame(const char* data, int len, uint16_t packet_serial, uint32_t timestamp)
{
  std::lock_guard lock_guard(mutex_);
  if (frame_map_.size() >= MAX_DATA)
  {
    frame_map_.erase(frame_map_.begin());
  }
  frame_map_[timestamp] = std::string(data, len);
}

void Dialog::ConnectToServer()
{
  QUrl url(QStringLiteral("ws://47.243.17.172:8182"));

  voice_client_socket_.open(url);
  ui->login_btn->setEnabled(false);
  ui->account->setEnabled(false);
  ui->output->append("connecting...");

  QSettings settings("whatsapp");
  settings.setValue("account", ui->account->text());
}


void Dialog::OnWebsocketBinaryMessageReceived(const QByteArray& message)
{
    uint16_t packet_serial;
  memcpy(&packet_serial, message.data(), sizeof(packet_serial));
  packet_serial = ntohs(packet_serial);

  uint32_t timestamp;
  memcpy(&timestamp, message.data() + sizeof(packet_serial), sizeof(timestamp));
  timestamp = ntohl(timestamp);

  uint16_t type;
  memcpy(&type, message.data() + sizeof(packet_serial) + sizeof(timestamp), sizeof(type));
  type = ntohs(type);
  if (type == 0)
  {
    PushFrame(message.data() + sizeof(packet_serial) + sizeof(timestamp) + sizeof(type), message.size() - sizeof(packet_serial) - sizeof(timestamp) - sizeof(type), packet_serial, timestamp);
  }
}

void Dialog::on_login_btn_clicked()
{
  if (ui->account->text().isEmpty())
  {
    return;
  }
  reset_connection_ = false;
  ConnectToServer();
}


void Dialog::on_proxy_btn_clicked()
{
  ProxyDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted)
  {
    proxy_ = dialog.proxy();
  }
}


void Dialog::on_disconnect_btn_clicked()
{
  voice_client_socket_.close();
}


void Dialog::on_proxy_checkbox_stateChanged(int arg1)
{
  QSettings settings("whatsapp");
  settings.setValue("enable_proxy", ui->proxy_checkbox->isChecked());
}


void Dialog::on_bussiness_checkbox_stateChanged(int arg1)
{
  QSettings settings("whatsapp");
  settings.setValue("bussiness", ui->bussiness_checkbox->isChecked());
}


void Dialog::on_scanqrcode_clicked()
{
  reset_connection_ = false;
  QNetworkProxy proxy;
  if (ui->proxy_checkbox->isChecked())
  {
    proxy = proxy_;
  }
  QString uuid = QUuid::createUuid().toString(QUuid::Id128).toUpper();
  QrcodeLogin dialog(proxy, uuid);
  if (dialog.exec() == QDialog::Accepted) {
    ui->account->setText(uuid);
    reset_connection_ = true;
    ConnectToServer();
  }
}

