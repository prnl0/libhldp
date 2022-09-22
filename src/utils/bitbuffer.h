#pragma once

/* Note: assumes least significant bit to be on the right. */

#include <stdexcept>
#include <cstdint>
#include <vector>
#include <istream>
#include <string>

class bit_buffer_error : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};

class bit_buffer
{
  using ubyte_t = std::uint8_t;
  using value_t = std::uint64_t;
  using data_t = std::vector<ubyte_t>;
  using size_t = data_t::size_type;

  friend class file_buffer;

public:
  enum class seek_dir : std::uint8_t
  {
    /* Seek N bytes from the ... */
    cur = 0,  // ... current position towards the end
    beg,      // ... beginning towards the end
    end       // ... end towards the beginning
  };

  bit_buffer(
    std::istream &is,
    const std::streamoff &bytes
  ) : buffer_(bytes, '\0'),
      byte_(buffer_.data())
  {
    is.read(reinterpret_cast<char *>(buffer_.data()), bytes);
  }

  /* Read operations */
  value_t read_bits(ubyte_t amt) const;
  ubyte_t read_bit() const;

  data_t read_bytes(size_t amt) const;
  ubyte_t read_byte() const;

  template<typename T>
  T read() const
  {
    return static_cast<T>(read_bits(sizeof(T) * 8));
  }

  template<typename T>
  const bit_buffer &read(T &out) const
  {
    out = read<T>();
    return *this;
  }

  template<>
  float read<float>() const;

  template<>
  std::string read<std::string>() const;
  std::string read_string(std::string::size_type sz) const;

  /* Position operations */
  void skip_bits(size_t amt) const;
  void skip_bytes(size_t amt) const;
  void skip_byte() const;

  void seek_bytes(
    size_t amt,
    seek_dir dir = seek_dir::beg
  ) const;

  /* Auxiliaries */
  void align_byte() const;

  bool is_remaining_n(size_t bits) const noexcept
  {
    return (byte_ - buffer_.data()) * 8 + bit_pos_ + bits <= buffer_.size() * 8;
  }

private:
  data_t buffer_;
  mutable const ubyte_t *byte_ = nullptr;
  mutable ubyte_t bit_pos_ = 0; // relative to the current byte ([0; 7])
};
