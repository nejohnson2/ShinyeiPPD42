# Copyright (c) 2016 Nicholas Johnson
# Author: Nicholas Johnson
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import RPi.GPIO as GPIO
import time

__author__ = "Nicholas Johnson <nejohnson2@gmail.com>"
__copyright__ = "Copyright (C) 2016 Nicholas Johnson"
__license__ = "MIT"
__version__ = "v1.0"

'''History
v1.0 - First Release
'''
	
class Shinyei(object):

	def __init__(self, pin):
		self.pin = pin 			# pin for GPIO connection
		GPIO.setup(self.pin, GPIO.IN)
		
	def _start(self):
		'''Set default values and 
		attach event detection'''
		self.lpo = 0				# duration of low pulses
		self.start_ltime = 0		# initialize to zero to prevent start with low pulse
		GPIO.add_event_detect(self.pin, GPIO.BOTH, self._get_lpo)

	def _reset(self):
		'''Remove event detection'''
		GPIO.remove_event_detect(self.pin)	# prevents an interrupt during the rest of the code		

	def _get_lpo(self, channel):
		'''Callback function when pin 
		changes	high or low'''

		current_time = time.time()	# get time when event happened

		if not GPIO.input(channel):
			'''Reading went low'''
			self.start_ltime = current_time					# start timer for low reading
		else:
			'''Reading went high'''
			if self.start_ltime != 0:
				duration = current_time - self.start_ltime		# add time that was low
				self.lpo += duration

	def _calc_ratio(self, duration):
		'''calculate ratio of low pulse time to total time'''
		if self.lpo != 0:
			ratio = float(self.lpo) / float(duration)		# calculate percentage of pulses being low
		else:
			ratio = 0

		return ratio

	def _calc_concentration(self, ratio):
		'''calculate particles per 0.01 cubic feet'''
		concentration = (1.1 * ratio**3) - (3.8 * ratio**2) + (520 * ratio) + 0.62

		return concentration

	def read(self, duration):
		'''Output results every 30s
		otherwise do nothing'''
		self._start()
		start_time = time.time()

		while time.time() - start_time < duration:
			pass			# do nothing 
		else:
			r = self._calc_ratio(duration)
			c = self._calc_concentration(r)

			self._reset()	# remove event detect
			return [self.lpo, r, c]
		

if __name__ == '__main__':
	GPIO.setmode(GPIO.BCM)
	t = Shinyei(18)
	while True:
		print t.read(30)

	GPIO.cleanup()	

