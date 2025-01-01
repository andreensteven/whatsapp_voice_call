#ifndef QRCODE_LOGIN_H
#define QRCODE_LOGIN_H

#include <QDialog>
#include <QWebSocket>
#include <QNetworkProxy>
#include <QJsonObject>


namespace Ui {
class QrcodeLogin;
}

class QrcodeLogin : public QDialog
{
    Q_OBJECT

public:
    QrcodeLogin(QNetworkProxy proxy, QString uuid, QWidget *parent = nullptr);
    ~QrcodeLogin();


    void OnWebsocketConnected();
    void OnWebsocketDisconnected();
    void OnWebsocketTextMessageReceived(const QString& message);
    void OnWebsocketBinaryMessageReceived(const QByteArray& message);
private slots:
    void on_refresh_btn_clicked();

private:
  void ShowQRCode(const QJsonObject& command);
  void HideQRCode();
  void HandleLogin(const QJsonObject& command);

private:
    Ui::QrcodeLogin *ui;
    QWebSocket voice_client_socket_;
    QNetworkProxy proxy_;
    QString uuid_;
};

#endif // QRCODE_LOGIN_H
