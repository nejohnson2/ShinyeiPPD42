# ShinyeiPPD42 Python Module
Python module to use the ShinyeiPPD42 dust particle sensor on a Raspberry Pi

## Usage

```python
from ShinyeiPPD42 import Shinyei

GPIO.setmode(GPIO.BCM)  
t = Shinyei(18)         # GPIO pin 18
	
while True:
  print t.read(30)      # supply integration time in seconds

GPIO.cleanup()	
```
## About the ShinyeiPPD42

Ground
Output (P2)
Input (+5VDC, 90mA)
Output (P1)
Threshold

P1 measures particles largeer than 1 micron and P2 measures particles larger than 2.5 microns.  The detectable range of the sensor is approximately 0 - 8000 pcs/0.01cf.

Connector: [S5B-EH(JST)](http://www.digikey.com/product-detail/en/jst-sales-america-inc/S5B-EH/455-1031-ND/527255)

## Credits
The code is largely derived from the [work by Chris Nafis ](http://www.howmuchsnow.com/arduino/airquality/grovedust/).

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
