#include "Request.h"
#include <string>

std::string methodTypeToString(method_type t);
std::string identifyContentType(std::string s);

Request::Request() {
  this->_code = nullptr;
  this->_method = nullptr;
  this->_content_type = nullptr;
  this->_message = nullptr;
  this->_response = nullptr;
}

Request::~Request() {}

Request::Request(int n, std::string s, method_type m) {
  std::string met = methodTypeToString(m);
  std::string num = std::to_string(n);
  std::string content = identifyContentType(s);
  this->_code = num,
  this->_method = met;
  this->_content_type = content;
  this->_response = "";
  this->_message = "";

}

Request &Request::operator=(const Request &obj) {
  if (this != &obj) {
    *this = obj;
  }
  return *this;
}

Request::Request(const Request &obj) {
  if (this != &obj)
    *this = obj;
}

std::ostream &operator<<(std::ostream &os, const Request &obj) {
  os << "Code: " << obj.getCode() 
  << ", Method = " << obj.getMethod() 
  << ", Content_type = " << obj.getContentType()
  << ", Message = " << obj.getMessage()
  << ", Response = " << obj.getResponse() << std::endl;

  return os;
}

void Request::fetchCode(int n) {
  std::string s = std::to_string(n);
  this->_code = s;
}

void Request::fetchMethod(method_type m) {
  std::string s = methodTypeToString(m);
  
  this->_method = s;
}

void Request::fetchContentType(std::string s) {
  this->_content_type = s;
}

void Request::setResponse(std::string s) {
  this->_response = s;
}

void Request::fetchMessage() {}


std::string Request::getCode() const {
  return (this->_code);
}
std::string Request::getContentType() const {
  return (this->_content_type);
}
std::string Request::getMessage() const {
  return (this->_message);
}

std::string Request::getMethod() const {
  return (this->_method);
}

std::string Request::getResponse() const {
  return (this->_response);
}
