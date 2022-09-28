#pragma once

#include <filesystem>
#include <fstream>
#include <memory>

#include "bitbuffer.hpp"

namespace utils
{
namespace file
{
  inline std::streamoff get_size(std::ifstream &fs)
  {
    const auto sz = fs.seekg(0, std::ios_base::end).tellg() - std::streampos(0);
    fs.seekg(0);
    return sz;
  }
} // namespace file
} // namespace utils

class file_buffer
{
public:
  /* ``bytes == -1`` signifies that the whole file is to be read. */
  file_buffer(
    const std::filesystem::path &path,
    const std::streamoff &bytes = -1
  ) : ifs_(path, std::ios::binary),
      path_(path)
  {
    ifs_.exceptions(std::ifstream::failbit);
    if (size_ = utils::file::get_size(ifs_); size_ > 0) {
      acquire_data(bytes);
    }
  }

  ~file_buffer()
  {
  }

  /* Read operations */
  template<typename T>
  T read() const
  {
    return datastream_->read<T>();
  }

  template<typename T>
  const file_buffer &read(T &out) const
  {
    datastream_->read(out);
    return *this;
  }

  const file_buffer &read(std::string &out, std::string::size_type sz) const
  {
    out = datastream_->read_string(sz);
    return *this;
  }

  std::string read_string(std::string::size_type sz) const
  {
    return datastream_->read_string(sz);
  }

  bit_buffer::data_t read_bytes(bit_buffer::size_t amt) const
  {
    return datastream_->read_bytes(amt);
  }

  /* Position operations */
  const file_buffer &seek_bytes(
    bit_buffer::size_t amt,
    bit_buffer::seek_dir dir = bit_buffer::seek_dir::beg
  ) const
  {
    datastream_->seek_bytes(amt, dir);
    return *this;
  }

  /* Resource management */
  /* Note: ``acquire_data`` and ``release_data`` possibly violate RAII,
   * although they are used in but two instances (in the ``parser`` class). */
  bool data_acquired() const noexcept
  {
    return datastream_.get() != nullptr;
  }

  void acquire_data(const std::streamoff &bytes = -1) const
  {
    datastream_ = std::make_unique<bit_buffer>(ifs_, bytes == -1 ? size_ : bytes);
  }

  void release_data() const noexcept
  {
    delete datastream_.release();
  }

  /* Auxiliaries */
  const std::filesystem::path &path() const noexcept
  {
    return path_;
  }

  std::u8string filename() const
  {
    return path_.filename().u8string();
  }

  std::streamoff size() const noexcept
  {
    return size_;
  }

private:
  mutable std::ifstream ifs_;
  const std::filesystem::path &path_;
  std::streamoff size_ = 0;
  mutable std::unique_ptr<bit_buffer> datastream_;
};
