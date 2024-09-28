#pragma once

# include "../lib/includes/webserv.h"
# include <string>

class Request {
private:
  std::string _code;
  std::string _method;
  std::string _content_type;
  std::string _message;
  std::string _response;


public: 
  Request();
  Request(int n, std::string s, method_type m);
  ~Request();

  Request(const Request &obj);
  Request &operator=(const Request &obj);

  std::string getCode() const;
  std::string getMethod() const;
  std::string getContentType() const;
  std::string getMessage() const;
  std::string getResponse() const;

  void fetchCode(int n);
  void fetchMethod(method_type m);
  void fetchContentType(std::string s);
  void fetchMessage();

  void setResponse(std::string);
  
};

std::ostream &operator<<(std::ostream &os, const Request &obj);
