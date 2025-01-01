#pragma once
#include <functional>
#include <string>
#include <memory>

#define RECORDEXPORT __declspec(dllimport)

RECORDEXPORT std::string WriteBarcode(const std::string& text, int width, int height);

class RECORDEXPORT RecordApi {
public:
  virtual ~RecordApi();
  virtual int Init(std::function<std::string()> out_delegate, std::function<void(const char* buffer, int size)> in_delegate) = 0;
  static std::unique_ptr<RecordApi> Create();
};
