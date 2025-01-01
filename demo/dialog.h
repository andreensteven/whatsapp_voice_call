#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <mutex>
#include <map>
#include <QByteArray>
#include <QWebSocket>
#include <QNetworkProxy>
#include <QTimer>
#include <memory>
#include "record_export.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

#define MAX_COUNT_DOWN 5 * 60

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();


    std::string HandleOutData();
    void HandleRecordData(const char* data, int len);

private:
  void OnWebsocketConnected();
  void OnWebsocketDisconnected();
  void OnWebsocketTextMessageReceived(const QString& message);
  void OnWebsocketBinaryMessageReceived(const QByteArray& message);
  void PushFrame(const char* data, int len, uint16_t packet_serial, uint32_t timestamp);

  void ConnectToServer();

signals:
  void SendWebsocket(const QByteArray& buffer);

private slots:
    void on_login_btn_clicked();

    void on_proxy_btn_clicked();

    void on_disconnect_btn_clicked();

    void on_proxy_checkbox_stateChanged(int arg1);

    void on_bussiness_checkbox_stateChanged(int arg1);

    void on_scanqrcode_clicked();

private:
    Ui::Dialog *ui;
    std::unique_ptr<RecordApi> opus_player_;
    std::mutex mutex_;
    std::map<uint32_t, std::string> frame_map_;
    QWebSocket voice_client_socket_;
    QNetworkProxy proxy_;
    bool send_audio_ = false;
    int init_code_ = 0;
    bool reset_connection_ = false;
};
#endif // DIALOG_H
