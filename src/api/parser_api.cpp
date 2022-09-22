#include "parser_api.h"

#include <filesystem>
#include <memory>

#include "../parser/parser.h"

parser_api::parser_api(const std::filesystem::path &demopath)
  : parser_(new parser(demopath))
{
}

parser_api::~parser_api()
{
  delete parser_;
}