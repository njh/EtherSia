/* 
  node.js server implication of the Daytime protocol (RFC867)
  
  https://tools.ietf.org/html/rfc867
  
  License: Unlicense <http://unlicense.org/>
*/


// Load the TCP Library
net = require('net');

// Put a friendly message on the terminal of the server
console.log("Starting Daytime Protocol server\n");

// Start a TCP Server
var server = net.createServer(function (socket) {

  socket.on('error', function (err) {
    console.log(err);
  });

  console.log("Handling connection from: " + socket.remoteAddress + ":" + socket.remotePort);

  // Get the current date/time
  var now = new Date();
  
  // Send it to the client as an ISO 8601 string and close the socket
  socket.end(now.toISOString() + "\n");

})

server.on('error', function (err) {
  console.log(err);
});

server.listen(13);
