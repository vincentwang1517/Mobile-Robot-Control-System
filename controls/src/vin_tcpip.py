#! /usr/bin/enc python

import socket, sys, threading

def ServerInitialization( port, mode ):

	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
	if sock < 0:
		print 'Error: Cannot Create a Socket';
	host = '';
	sock.bind((host, port));
	sock.listen(5);

	if mode == 'laser':
		print 'Server of laser stands by...';
	elif mode == 'enc':
		print 'Server of encoder stands by...';
	elif mode == 'vel':
		print 'Server of velocity stands by...';
	else :
		print 'Error: Invalid mode!';
		return False; 
	return sock;
