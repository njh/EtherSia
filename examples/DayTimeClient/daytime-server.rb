#!/usr/bin/env ruby

require "socket"  

Thread.abort_on_exception = true

server = TCPServer.new(13)  
loop do  
  Thread.new(server.accept) do |client|  
    family, remote_port, remote_ip  = client.peeraddr(:numeric)
    puts("Accepted connection from host '#{remote_ip}' port #{remote_port}")
    client.puts Time.now.localtime
    client.close  
  end  
end
