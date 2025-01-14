#include "reassembler.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  if ( is_end_ )
    return;

  first_unassembled_index_ = output_.writer().bytes_pushed();
  end_unassembled_index_ = first_unassembled_index_ + output_.writer().available_capacity() - 1;

  if ( first_index + data.size() < first_unassembled_index_ || first_index > end_unassembled_index_ )
    return;

  // cout << "进入构造" << endl;
  uint64_t data_start_index = max( first_index, first_unassembled_index_ );
  uint64_t data_end_index = min( first_index + data.size() - 1, end_unassembled_index_ );
  std::string_view tem_input( data );
  tem_input = tem_input.substr( data_start_index - first_index, data_end_index - first_index + 1 );
  for ( uint64_t i = 0, index = data_start_index; i < tem_input.size(); i++, index++ ) {
    auto [iter, inserted] = data_wait_reassembler_.emplace( index, tem_input[i] );
    if ( inserted ) {
      bytes_pending_ += 1;
    }
  }

  std::string tem_output;
  tem_output.reserve( data_wait_reassembler_.size() );
  while ( data_wait_reassembler_.count( first_unassembled_index_ ) ) {
    tem_output += data_wait_reassembler_.at( first_unassembled_index_ );
    data_wait_reassembler_.erase( first_unassembled_index_ );
    bytes_pending_ -= 1;
    first_unassembled_index_ += 1;
  }

  output_.writer().push( tem_output );

  if ( is_last_substring ) {
    bytes_num_ = first_index + data.size();
  }

  if ( output_.writer().bytes_pushed() == bytes_num_ ) {
    is_end_ = true;
    output_.writer().close();
  }
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return bytes_pending_;
}
