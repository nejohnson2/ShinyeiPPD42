# ShinyeiPPD42 Python Module
Python module to use the ShinyeiPPD42 dust particle sensor on a Raspberry Pi.  The module is still in development.  There is a second module using the [pigpio](http://abyz.co.uk/rpi/pigpio/examples.html) library though the code does not yet implement dual pin usage.

## Usage

```python
# RPi.GPIO version
from ShinyeiPPD42 import Shinyei

GPIO.setmode(GPIO.BCM)  
t = Shinyei(18)         # GPIO pin 18
	
while True:
  print t.read(30)      # supply integration time in seconds

GPIO.cleanup()	
```

```python
# pigpio version
pi = pigpio.pi() # connect to pi
s = Shinyei(pi, 23)
while True:
	time.sleep(5) # use 30 for properly calibrated reading

	g, r, c = s.read()

	print "GPIO=%s Ratio=%s Concenration=%s pcs per 0.01 cubic foot" %(g, r, int(c))

pi.stop() # Disconnect from pi
```

## About the ShinyeiPPD42

```
Shinyei Pin Output
------------------
Ground
Output (P2)
Input (+5VDC, 90mA)
Output (P1)
Threshold
```

P1 measures particles largeer than 1 micron and P2 measures particles larger than 2.5 microns.  The detectable range of the sensor is approximately 0 - 8000 pcs/0.01cf.

Connector: [S5B-EH(JST)](http://www.digikey.com/product-detail/en/jst-sales-america-inc/S5B-EH/455-1031-ND/527255)

> Note: The ShinyeiPPD42 draws approximately 90mA.  On a Raspberry Pi, the GPIO 5v pin should be connected to the USB bus and will therefore supply up to 1A minus whatever the RPi draws.  This should allow for proper usage though this needs to be confirmed.  When using an Arduino Uno, the Uno should not be powered over USB as the USB passes through the ATMEGA chip and will therefore not supply enough power.  Use the DC jack with >7v power supply.

## Credits
The code is largely derived from the [work by Chris Nafis ](http://www.howmuchsnow.com/arduino/airquality/grovedust/).  Further experiments are posted on the [indiaairquality](https://indiaairquality.com/) blog which have been tremendously useful.

# MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
