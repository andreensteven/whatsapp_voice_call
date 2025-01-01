#pragma once
enum CommandType
{
  CommandType_Connect,
  CommandType_Login,
  CommandType_RecvMessage,
  CommandType_SendAudio,
  CommandType_QRCode,
};

enum LoginCode {
  LoginCode_Success = 0,
  // 握手失败
  LoginCode_Handshake_Failed = 1,
  LoginCode_Websocket_Disconnected = 2,
  // 扫码之后，重新扫码
  LoginCode_QRCode_Relogin = 3,
  // 二维码过期
  LoginCode_QRCode_Expired = 4,
  // 登录失败， 接收到了 stream:error,login faile
  LoginCode_Login_Failed = 5,
};