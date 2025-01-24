#include "reassembler.hh"
#include <algorithm>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  Writer& bytes_writer = output_.writer();
  if ( const uint64_t unacceptable_index = expecting_index_ + bytes_writer.available_capacity();
       bytes_writer.is_closed() || bytes_writer.available_capacity() == 0 || first_index >= unacceptable_index )
    return; // 流关闭、容量为零和超出处理部分的分组都不被接受
  else if ( first_index + data.size() >= unacceptable_index ) {
    is_last_substring = false;                       // 数据会被截断，不视作最后一个数据分组
    data.resize( unacceptable_index - first_index ); // 超出范围的数据要丢弃
  }

  if ( first_index > expecting_index_ )
    cache_bytes( first_index, move( data ), is_last_substring );
  else
    push_bytes( first_index, move( data ), is_last_substring );
  flush_buffer();
}

uint64_t Reassembler::bytes_pending() const
{
  return num_bytes_pending_;
}

void Reassembler::push_bytes( uint64_t first_index, string data, bool is_last_substring )
{
  if ( first_index < expecting_index_ ) // 部分重复的分组
    data.erase( 0, expecting_index_ - first_index );
  expecting_index_ += data.size();
  output_.writer().push( move( data ) );

  if ( is_last_substring ) {
    output_.writer().close(); // 关闭写端
    unordered_bytes_.clear(); // 清空缓冲区
    num_bytes_pending_ = 0;
  }
}

void Reassembler::cache_bytes( uint64_t first_index, string data, bool is_last_substring )
{
  auto end = unordered_bytes_.end();
  auto left = lower_bound( unordered_bytes_.begin(), end, first_index, []( auto&& e, uint64_t idx ) -> bool {
    return idx > ( get<0>( e ) + get<1>( e ).size() );
  } );
  auto right = upper_bound( left, end, first_index + data.size(), []( uint64_t nxt_idx, auto&& e ) -> bool {
    return nxt_idx < get<0>( e );
  } ); // 注意一下：right 指向的是待合并区间右端点的下一个元素

  if ( const uint64_t next_index = first_index + data.size(); left != end ) {
    auto& [l_point, dat, _] = *left;
    if ( const uint64_t r_point = l_point + dat.size(); first_index >= l_point && next_index <= r_point )
      return; // data 已经存在
    else if ( next_index < l_point ) {
      right = left;                                                      // data 和 dat 没有重叠部分
    } else if ( !( first_index <= l_point && r_point <= next_index ) ) { // 重叠了
      if ( first_index >= l_point ) {                                    // 并且 dat 没有被完全覆盖
        data.insert( 0, string_view( dat.c_str(), dat.size() - ( r_point - first_index ) ) );
      } else {
        data.resize( data.size() - ( next_index - l_point ) );
        data.append( dat ); // data 在前
      }
      first_index = min( first_index, l_point );
    }
  }

  if ( const uint64_t next_index = first_index + data.size(); right != left && !unordered_bytes_.empty() ) {
    // 如果 right 指向 left，表示两种可能：没有重叠区间、或者只需要合并 left 这个元素
    auto& [l_point, dat, _] = *prev( right );
    if ( const uint64_t r_point = l_point + dat.size(); r_point > next_index ) {
      data.resize( data.size() - ( next_index - l_point ) );
      data.append( dat );
    }
  }

  for ( ; left != right; left = unordered_bytes_.erase( left ) ) {
    num_bytes_pending_ -= get<1>( *left ).size();
    is_last_substring |= get<2>( *left );
  }
  num_bytes_pending_ += data.size();
  unordered_bytes_.insert( left, { first_index, move( data ), is_last_substring } );
}

void Reassembler::flush_buffer()
{
  while ( !unordered_bytes_.empty() ) {
    auto& [idx, dat, last] = unordered_bytes_.front();
    if ( idx > expecting_index_ )
      break;                          // 乱序的，不做任何动作
    num_bytes_pending_ -= dat.size(); // 数据已经被填补上了，立即推入写端
    push_bytes( idx, move( dat ), last );
    if ( !unordered_bytes_.empty() )
      unordered_bytes_.pop_front();
  }
}
