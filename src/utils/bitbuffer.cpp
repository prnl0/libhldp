#include "bitbuffer.hpp"

#include <cstdint>
#include <string>

#include "fmt/format.h"

static constexpr std::uint64_t mask_table[] =
{
    0x0000000000000000, 0x0000000000000001, 0x0000000000000003,
    0x0000000000000007, 0x000000000000000F, 0x000000000000001F,
    0x000000000000003F, 0x000000000000007F, 0x00000000000000FF,
    0x00000000000001FF, 0x00000000000003FF, 0x00000000000007FF,
    0x0000000000000FFF, 0x0000000000001FFF, 0x0000000000003FFF,
    0x0000000000007FFF, 0x000000000000FFFF, 0x000000000001FFFF,
    0x000000000003FFFF, 0x000000000007FFFF, 0x00000000000FFFFF,
    0x00000000001FFFFF, 0x00000000003FFFFF, 0x00000000007FFFFF,
    0x0000000000FFFFFF, 0x0000000001FFFFFF, 0x0000000003FFFFFF,
    0x0000000007FFFFFF, 0x000000000FFFFFFF, 0x000000001FFFFFFF,
    0x000000003FFFFFFF, 0x000000007FFFFFFF, 0x00000000FFFFFFFF,
    0x00000001FFFFFFFF, 0x00000003FFFFFFFF, 0x00000007FFFFFFFF,
    0x0000000FFFFFFFFF, 0x0000001FFFFFFFFF, 0x0000003FFFFFFFFF,
    0x0000007FFFFFFFFF, 0x000000FFFFFFFFFF, 0x000001FFFFFFFFFF,
    0x000003FFFFFFFFFF, 0x000007FFFFFFFFFF, 0x00000FFFFFFFFFFF,
    0x00001FFFFFFFFFFF, 0x00003FFFFFFFFFFF, 0x00007FFFFFFFFFFF,
    0x0000FFFFFFFFFFFF, 0x0001FFFFFFFFFFFF, 0x0003FFFFFFFFFFFF,
    0x0007FFFFFFFFFFFF, 0x000FFFFFFFFFFFFF, 0x001FFFFFFFFFFFFF,
    0x003FFFFFFFFFFFFF, 0x007FFFFFFFFFFFFF, 0x00FFFFFFFFFFFFFF,
    0x01FFFFFFFFFFFFFF, 0x03FFFFFFFFFFFFFF, 0x07FFFFFFFFFFFFFF,
    0x0FFFFFFFFFFFFFFF, 0x1FFFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFFF,
    0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF
};

bit_buffer::value_t bit_buffer::read_bits(ubyte_t amt) const
{
  if (byte_ == nullptr) {
    throw bit_buffer_error("unable to read bits - buffer exhausted (all bits processed)");
  }
  if (!is_remaining_n(amt)) {
    throw bit_buffer_error(
      fmt::format("unable to read specified amount ({}) of bits - exceeded buffer size", amt)
    );
  }
  if (amt > 64) {
    throw bit_buffer_error(
      fmt::format("cannot read more than 64 bits at a time ({} requested)", amt)
    );
  }
  if (amt == 0) {
    return 0;
  }
  const auto ret = (*reinterpret_cast<const value_t *>(byte_) >> bit_pos_) & mask_table[amt];
  skip_bits(amt);
  return ret;
}

bit_buffer::ubyte_t bit_buffer::read_bit() const
{
  return static_cast<ubyte_t>(read_bits(1));
}

bit_buffer::data_t bit_buffer::read_bytes(size_t amt) const
{
  data_t out;
  out.reserve(amt);
  for (size_t i = 0; i != amt; ++i) {
    out.push_back(read_byte());
  }
  return out;
}

bit_buffer::ubyte_t bit_buffer::read_byte() const
{
  return static_cast<ubyte_t>(read_bits(8));
}

template<>
float bit_buffer::read<float>() const
{
  return *reinterpret_cast<float *>(read_bytes(sizeof(float)).data());
}

template<>
std::string bit_buffer::read<std::string>() const
{
  std::string str;
  for (ubyte_t b = 0; (b = read_byte()); ) {
    str += b;
  }
  return str;
}

std::string bit_buffer::read_string(std::string::size_type sz) const
{
  std::string str(sz, '\0');
  return str.assign(reinterpret_cast<const char *>(read_bytes(sz).data()), sz);
}

void bit_buffer::skip_bits(size_t amt) const
{
  if (!is_remaining_n(amt + 1)) {
    /* We skipped all remaining bits - set current byte to null to indicate
     * that we've reached the end. */
    byte_ = nullptr;
    bit_pos_ = 0;
  } else {
    byte_ += amt / 8;
    if ((bit_pos_ += static_cast<decltype(bit_pos_)>(amt) % 8) > 7) {
      bit_pos_ %= 8;
      ++byte_;
    }
  }
}

void bit_buffer::skip_bytes(size_t amt) const
{
  if (byte_ - buffer_.data() + amt > buffer_.size()) {
    byte_ = nullptr;
    bit_pos_ = 0;
  } else {
    byte_ += amt;
  }
}

void bit_buffer::skip_byte() const
{
  skip_bytes(1);
}

void bit_buffer::seek_bytes(
  size_t amt,
  seek_dir dir
) const
{
  switch (dir) {
    case seek_dir::cur:
      skip_bytes(amt);
      break;
    
    case seek_dir::beg:
      byte_ = buffer_.data();
      bit_pos_ = 0;
      skip_bytes(amt);
      break;
    
    case seek_dir::end:
      if (amt >= buffer_.size()) {
        byte_ = buffer_.data();
        bit_pos_ = 0;
      } else {
        byte_ = buffer_.data() + buffer_.size() - amt;
        bit_pos_ = 7;
      }
      break;

    default: throw bit_buffer_error("invalid seek direction");
  }
}

void bit_buffer::align_byte() const
{
  if (bit_pos_ > 0 && byte_ != buffer_.data()) {
    ++byte_;
    bit_pos_ = 0;
  }
}
