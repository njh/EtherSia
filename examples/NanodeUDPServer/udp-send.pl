#!/usr/bin/perl
#
# Perl Script to turn the RED LED on the Nanode On and Off
#

use IO::Socket::INET6;
use Socket6;

my $socket = new IO::Socket::INET6(
    PeerAddr   => "fe80::0204:a3ff:fe2c:2bb9%en0",
    PeerPort   => 1234,
    Proto      => 'udp'
) or die "ERROR in Socket Creation: $!\n";

for (my $i=1; $i <= 10; $i++) {

    $socket->send("on") or die "Error ending packet: $!\n";

    sleep(1);

    $socket->send("off") or die "Error ending packet: $!\n";

    sleep(1);

}

$socket->close();
