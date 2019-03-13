#! /usr/bin/env python
import socket, sys, threading
import rospy
import string
from sensor_msgs.msg import LaserScan
from geometry_msgs.msg import Twist
from std_msgs.msg import Int16
import myrostools
import vin_tcpip

######## Set up ROS subcribers and publushers ####### 
rospy.init_node('scan_values')	# This node name.
rate = rospy.Rate(10);	# For publishers.
sub_laser = rospy.Subscriber('/scan', LaserScan, myrostools.callback_laser); 
sub_Renc = rospy.Subscriber('/Rencoder', Int16, myrostools.callback_Rencoder);
sub_Lenc = rospy.Subscriber('/Lencoder', Int16, myrostools.callback_Lencoder);
#pub_vel = rospy.Publisher('cmd_vel', Twist, queue_size=1);
pub_vel = myrostools.pub_vel;

move_vel_thread = myrostools.thread_vel();
move_vel_thread.start();

####### Main Function ########
def job(sock_name):	# job for thread
	##### Create Server #####
	if sock_name == 'laser':
		sock_laser = vin_tcpip.ServerInitialization( port=8001, mode='laser');
		while True:
			csock, address = sock_laser.accept();
			while True:
				cl_msg = csock.recv(1024);
				cl_msg = cl_msg.rstrip(' \t\r\n\0');
				cmsg = cl_msg.split();
				if cmsg[0] == 'las':
					message = myrostools.manage_laserdata('0~180', 5);
					csock.send(message);
					message = myrostools.manage_laserdata('540~720', 5);
					csock.send(message);

	elif sock_name == 'enc':
		sock_enc = vin_tcpip.ServerInitialization( port=8002, mode='enc');
		while True:
			csock, address = sock_enc.accept();
			while True:
				cl_msg = csock.recv(1024);
				cl_msg = cl_msg.rstrip(' \t\r\n\0');
				cmsg = cl_msg.split();
				if cmsg[0] == 'enc':
					message_enc = 'enc ';
					message_enc += (str(myrostools.Rencoder) + ' ' + str(myrostools.Lencoder));
					csock.send(message_enc);

	elif sock_name == 'vel':
		sock_vel = vin_tcpip.ServerInitialization( port=8003, mode='vel');
		while True:
			csock, address = sock_vel.accept();
			while True:
				cl_msg = csock.recv(1024);
				cl_msg = cl_msg.rstrip(' \t\r\n\0');
				cmsg = cl_msg.split();
				if cmsg[0] == 'vel':
					# data type: [vel x y th speed angular_speed]
					try:	
						global move_vel_thread;
						move_vel_thread.pause();
						speed = float(cmsg[4]);
						turn = float(cmsg[5])
						myrostools.twist.linear.x = float(cmsg[1]) * speed;
						myrostools.twist.linear.y = float(cmsg[2]) * speed;
						myrostools.twist.linear.z = 0;
						myrostools.twist.angular.x = 0;
						myrostools.twist.angular.y = 0;
						myrostools.twist.angular.z = float(cmsg[3]) * turn;
						print 'moving...'
						move_vel_thread.resume();
					except Exception as e:
						print e;

				elif cmsg[0] == 'stop':
					twist1 = Twist();
					print 'stop!!'
					move_vel_thread.pause();
					pub_vel.publish(twist1);
	else:
		raise ValueError('Invalid TCPIP mode name.')

server_laser = threading.Thread( target=job, args=('laser', ));
server_enc = threading.Thread( target=job, args=('enc', ));
server_vel = threading.Thread( target=job, args=('vel', ));
server_laser.start();
server_enc.start();
server_vel.start();
rospy.spin();