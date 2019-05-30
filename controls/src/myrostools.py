#! /usr/bin/env python
import threading, rospy, string, sys
from sensor_msgs.msg 	import LaserScan
from std_msgs.msg 		import Int16
from geometry_msgs.msg 	import Twist
from inspect 			import currentframe, getframeinfo

import util

datamanager = util.DataManager()

##### initialization #####
twist = Twist();  # The data type of velocity command (Google 'ros Twist').
frameinfo = getframeinfo(currentframe());

##### Callbacl function for Subscribers ######
def callback_laser(msg):
	datamanager.laser_data = msg.ranges;

def callback_Rencoder(msg):
	datamanager.rencoder = msg.data;

def callback_Lencoder(msg):
	datamanager.lencoder = msg.data;

def manage_laserdata(ranges, decimal):
	laser_data = datamanager.laser_data

	if ranges == '0~180':
		message = '0~180 ';
		for i in range(180):
			message += (str(laser_data[i])[:decimal] + ' ');
		#print 'Send Laserdata 0~180';
	elif ranges  == '540~720':
		message = '540~720 ';
		for i in range(180):
			message += (str(laser_data[540+i])[:decimal] + ' ');
		#print 'Send Laserdata 540~720';
	else:
		print 'Error in ', frameinfo.filename, frameinfo.lineno, ': wrong ranges (0~180 or 540~720)'; 
	return message;

##### Thread for velocity command #####
# In order to keep calling 'pub_vel.publish(twist)' command, I create a thread for vel_com.
pub_vel = rospy.Publisher('cmd_vel', Twist, queue_size=1);

class thread_vel(threading.Thread):
	def __init__(self):
		threading.Thread.__init__(self);
		self.__flag = threading.Event();  # Flag for pausing or not;
		self.__flag.set(); 		# .set ~ True
		self.__running = threading.Event();  # Flag for stopping the thread.
		self.__running.set();
	def run(self):  # thread.start() would call this function.
		rate = rospy.Rate(10);
		while self.__running.isSet():
			self.__flag.wait();     # If flag==True, skip. Otherwise, the thread would be stucked here.
			global twist;  
			pub_vel.publish(twist);  # Publish arguments to 'cmd_vel'.
			rate.sleep();
	def pause(self):
		self.__flag.clear();    # Set ... to False.
	def resume(self):
		self.__flag.set();
	def stop(self):
		self.__flag.set();
		self.__running.clear();

