# Whatsapp Call:  **+66 627011785**
Whatsapp: **+66 627011785**

telegram: **@gxccc618**

# 1 connect to server
websocket: "ws://47.243.17.172:8182"

# 2 login with qrcode
## 2.1 request
```json
//random 32 bytes string with socks5 proxy
 {
    "type":0,
    "role": "client",
    "uuid": "99493F69F7E641E48CE8484BA6518775",
    "proxy_server":"xxxx",
    "proxy_port": 1234,
    "proxy_username":"xxxx",
    "proxy_password":"xxxx"
 }
```
## response
```json
{
    "type" : 4,
    "qrcode": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
}
```

## 2.2 scan qrcode with you whatsapp
if scan qrcode you will receive websocket response
```json
{
    "type" : 1,
    "ret" : 3
}
```

## 2.3 disconnect and reconnect with pre uuid
```json
 {
    "type":0,
    "role": "client",
    "uuid": "99493F69F7E641E48CE8484BA6518775",
    "proxy_server":"xxxx",
    "proxy_port": 1234,
    "proxy_username":"xxxx",
    "proxy_password":"xxxx",
    "reset" : 1
 }

```
## 2.4 That's all. If you want to login next time, send 
```json
 {
    "type":0,
    "role": "client",
    "uuid": "99493F69F7E641E48CE8484BA6518775",
    "proxy_server":"xxxx",
    "proxy_port": 1234,
    "proxy_username":"xxxx",
    "proxy_password":"xxxx",
 }
 ```

 # 3 login with 6 parts account
 if account is whatsapp bussiness, set "bussiness" : true, else false
 ```json
 {
    "type":0,
    "role": "client",
    "uuid": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    "proxy_server":"xxxx",
    "proxy_port": 1234,
    "proxy_username":"xxxx",
    "proxy_password":"xxxx",
    "bussiness" : true
 }
 ```

 ## 4 start voice call
 send text message to current account with another whatsapp after login, then start voice call, you can send and receive voice data from websocket use binary message channel.
 