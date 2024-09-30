#pragma once

# include <string>

struct ConfigComponent {
  int retValue;
  std::string path;
  bool empty;
};

ConfigComponent build_config_component(std::string &str);
