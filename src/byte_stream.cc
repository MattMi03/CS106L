#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

void Writer::push( string data )
{
  if ( is_closed() )
    return;
  uint64_t write_in = min( data.size(), available_capacity() );
  for ( uint64_t i = 0; i < write_in; i++ ) {
    buf_.push_back( data[i] );
  }
  bytes_pushed_ += write_in;
  bytes_in_buf_ += write_in;
}

void Writer::close()
{
  // Your code here.
  if ( is_closed() )
    return;
  buf_.push_back( EOF );
  is_closed_ = true;
}

bool Writer::is_closed() const
{
  return is_closed_; // Your code here.
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - bytes_in_buf_; // Your code here.
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_pushed_; // Your code here.
}

string_view Reader::peek() const
{
  return string_view( &buf_.front(), 1 ); // Your code here.
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  for ( uint64_t i = 0; i < len; i++ ) {
    buf_.pop_front();
  }
  bytes_poped_ += len;
  bytes_in_buf_ -= len;
}

bool Reader::is_finished() const
{
  return is_closed_ && bytes_in_buf_ == 0; // Your code here.
}

uint64_t Reader::bytes_buffered() const
{
  return bytes_in_buf_; // Your code here.
}

uint64_t Reader::bytes_popped() const
{
  return bytes_poped_; // Your code here.
}