#pragma once

#include <filesystem>
#include <memory>

class parser;

namespace hldp
{
  class api
  {
  public:
    api(const std::filesystem::path &demopath);
    virtual ~api();
  
  private:
    parser *parser_ = nullptr;
  };
} // namespace hldp
