#include "tcp_receiver.hh"
#include <iostream>

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  if ( message.RST ) {
    reassembler_.reader().set_error();
    return;
  }
  if ( message.SYN ) {
    ISN_ = message.seqno;
  }
  if ( ISN_.has_value() ) {
    uint64_t first_index = message.seqno.unwrap( ISN_.value(), reassembler_.writer().bytes_pushed() );
    if ( !message.SYN ) {
      first_index--;
    }
    reassembler_.insert( first_index, message.payload, message.FIN );
  }
}

TCPReceiverMessage TCPReceiver::send() const
{
  uint64_t seqno = reassembler_.writer().bytes_pushed() + 1;
  uint16_t window_size = reassembler_.writer().available_capacity() < UINT16_MAX
                           ? reassembler_.writer().available_capacity()
                           : UINT16_MAX;
  if ( !ISN_.has_value() )
    return { {}, window_size, reassembler_.reader().has_error() };
  else
    return { Wrap32::wrap( seqno + reassembler_.writer().is_closed(), ISN_.value() ),
             window_size,
             reassembler_.reader().has_error() };
}
