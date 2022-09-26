#pragma once

#include <stdexcept>
#include <filesystem>

#include "demo.hpp"

#include "../utils/filebuffer.hpp"

class parser_error : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};

class parser
{
public:
  parser(const std::filesystem::path &demopath);

  void parse()
  {
    parse_frames();
  }

private:
  void parse_header();
  void parse_directories();
  void parse_frames();
  void parse_messages();

  file_buffer fdemo_; // represents the demo file itself
  demo demo_;

  bool prelim_info_gathered_ = false; // true if a valid local player has been obtained
};
