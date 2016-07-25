import pigpio
import time

class Shinyei(object):
	def __init__(self, pi, gpio):
		self.pi = pi
		self.gpio = gpio

		self._start_tick = None
		self._last_tick = None
		self._low_ticks = 0
		self._high_ticks = 0

		pi.set_mode(gpio, pigpio.INPUT)

		self._cb = pi.callback(gpio, pigpio.EITHER_EDGE, self._cbf)

	def read(self):
		duration = self._low_ticks + self._high_ticks

		if duration > 0:
			ratio = float(self._low_ticks) / float(duration) * 100.0
			conc = 1.1 * pow(ratio,3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62
		else:
			ratio = 0
			conc = 0

		self._start_tick = None
		self._last_tick = None
		self._low_ticks = 0
		self._high_ticks = 0

		return (self.gpio, ratio, conc)

	def _cbf(self, gpio, level, tick):

		if self._start_tick is not None:

			ticks = pigpio.tickDiff(self._last_tick, tick)

			self._last_tick = tick

			if level == 0: # falling edge
				self._high_ticks = self._high_ticks + ticks

			elif level == 1: # Rising edge
				self._low_ticks = self._low_ticks + ticks

			else: # timeout level, not used
				pass

		else:
			self._start_tick = tick
			self._last_tick = tick

if __name__ == '__main__':
	pi = pigpio.pi() # connect to pi
	s = Shinyei(pi, 23)
	while True:
		time.sleep(5) # use 30 for properly calibrated reading

		g, r, c = s.read()

		print "GPIO=%s Ratio=%s Concenration=%s pcs per 0.01 cubic foot" %(g, r, int(c))

	pi.stop() # Disconnect from pi
