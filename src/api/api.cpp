#include "hldp/api.hpp"

#include <filesystem>
#include <memory>

#include "../parser/parser.hpp"

namespace hldp
{
  api::api(const std::filesystem::path &demopath)
    : parser_(new parser(demopath))
  {
  }
  
  api::~api()
  {
    delete parser_;
  }
}
