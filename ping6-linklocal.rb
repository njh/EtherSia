#!/usr/bin/env ruby
#
# Ruby script to make it easier to ping an IPv6 link local address
#
# Usage: ./ping6-linklocal.rb <MAC> <INTERFACE>
#

require 'socket'

mac = ARGV[0] || '00:04:A3:2C:2B:B9'
ifname = ARGV[1]

if ifname.nil?
  ifname = Socket.getifaddrs.reject { |ifaddr|
    ifaddr.addr.nil? or
    ifaddr.addr.afamily != Socket::PF_INET6 or
    (ifaddr.flags & Socket::IFF_LOOPBACK) != 0
  }.first.name
end

parts = mac.strip.split(':').map {|m| m.to_i(16)}
ip6addr = sprintf("fe80::%2.2x%2.2x:%2.2xff:fe%2.2x:%2.2x%2.2x",
  parts[0] ^ 0x02, parts[1],
  parts[2], parts[3],
  parts[4], parts[5]
)

exec('ping6', '-I', ifname, ip6addr)
