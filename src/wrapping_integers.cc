#include "wrapping_integers.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.

  return zero_point + n;
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.
  uint64_t offset = raw_value_ - zero_point.raw_value_;
  if ( offset + ( 1ul << 31 ) < checkpoint ) {
    offset += ( ( checkpoint - offset + ( 1ul << 31 ) ) / ( 1ul << 32 ) ) * ( 1ul << 32 );
  }
  return offset;
}
