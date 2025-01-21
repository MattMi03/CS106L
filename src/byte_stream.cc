#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

void Writer::push( string data )
{
  if ( is_closed() || data.empty() || available_capacity() == 0 )
    return;
  uint64_t write_in = min( data.size(), available_capacity() );
  buf_.push_back( move( data.substr( 0, write_in ) ) );
  bytes_pushed_ += write_in;
  bytes_in_buf_ += write_in;
}

void Writer::close()
{
  // Your code here.
  if ( is_closed() )
    return;
  buf_.push_back( string( 1, EOF ) );
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
  // if ( buf_.empty() ) {
  //   return {};
  // }
  return buf_.front();
}

void Reader::pop( uint64_t len )
{

  uint64_t left = len;
  while ( left > 0 ) {
    uint64_t buf_front_size = buf_.front().size();
    if ( left >= buf_front_size ) {
      left -= buf_front_size;
      buf_.pop_front();
    } else {
      string& front = buf_.front();
      front = front.substr( left );
      left = 0;
    }
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