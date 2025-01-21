#include "reassembler.hh"
// #include <iostream>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Your code here.
  // end均是最后一个字符的index

  if ( is_last_substring ) {
    bytes_num_ = first_index + data.size();
  }

  if ( output_.writer().bytes_pushed() == bytes_num_ ) {
    output_.writer().close();
    is_end_ = true;
  }

  if ( is_end_ || output_.writer().available_capacity() == 0 ) {
    return;
  }

  first_unassembled_index_ = output_.writer().bytes_pushed();
  end_unassembled_index_ = first_unassembled_index_ + output_.writer().available_capacity() - 1;

  if ( first_index > end_unassembled_index_ || first_index + data.size() - 1 < first_unassembled_index_ ) {
    return;
  }

  if ( !data.empty() ) {
    uint64_t data_start = max( first_index, first_unassembled_index_ );
    uint64_t data_end = min( first_index + data.size() - 1, end_unassembled_index_ );
    uint64_t data_len = data_end - data_start + 1;
    for ( uint64_t i = 0; i < data_len; i++ ) {
      if ( !data_index_[data_start - first_unassembled_index_ + i] ) {
        data_wait_[data_start - first_unassembled_index_ + i] = data[data_start - first_index + i];
        data_index_[data_start - first_unassembled_index_ + i] = true;
        bytes_pending_++;
      }
    }
  }

  string out;
  out.reserve( bytes_pending_ );
  while ( data_index_.front() ) {
    out += data_wait_.front();
    data_wait_.pop_front();
    data_index_.pop_front();
    data_wait_.push_back( 0 );
    data_index_.push_back( false );
    bytes_pending_--;
  }

  output_.writer().push( out );

  if ( output_.writer().bytes_pushed() == bytes_num_ ) {
    output_.writer().close();
    is_end_ = true;
  }
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return bytes_pending_;
}
