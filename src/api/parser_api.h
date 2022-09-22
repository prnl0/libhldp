#pragma once

#include <filesystem>
#include <memory>

class parser;

class parser_api
{
public:
  parser_api(const std::filesystem::path &demopath);
  virtual ~parser_api();

private:
  parser *parser_ = nullptr;
};