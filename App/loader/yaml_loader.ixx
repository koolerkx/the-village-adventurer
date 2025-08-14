module;
#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/yaml.h"

export module app.loader.yaml;
import std;
export import app.loader.types;

// TODO: schema validation
export class YAMLConfigLoader {
public:
  YAMLConfigLoader(const std::string& config_path);

  const Config& getConfig() const noexcept;

private:
  Config config_;

  void loadConfig(const std::string& config_path) ;
};
