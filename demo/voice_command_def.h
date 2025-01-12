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
  LoginCode_Handshake_Failed = 1,
  LoginCode_Websocket_Disconnected = 2,
  LoginCode_QRCode_Relogin = 3,
  LoginCode_QRCode_Expired = 4,
  LoginCode_Login_Failed = 5,
};