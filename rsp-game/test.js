var dgram = require('dgram');
var socket = dgram.createSocket('udp4');

let port = 8080;
socket.bind(port);
socket.on('listening', function(){
	console.log('listening event');
});

socket.on('message', function(msg, rinfo){
	console.log('Message received', rinfo.address, msg.toString());
});

socket.on('close', function(){
	console.log('close event');
});


