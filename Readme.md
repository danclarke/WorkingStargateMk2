# Working Stargate Mk2 Control Code

Contained within is code for [Glitch's Working Stargate Mk2](https://www.thingiverse.com/thing:1603423). This code is specifically for the ESP32, but could work with other Arduino-compatible platforms. You'll need [VS Micro](http://www.visualmicro.com/) to compile the code as-is, but it should be possible to compile within the Arudino IDE... in theory at least.

## Requirements
- ESP32 board in Uno form factor with buck converter, I used a Wemos R32
- [Adafruit Motor Shield v2](https://www.adafruit.com/product/1438)
- [Adafruit Music Maker Shield](https://www.adafruit.com/product/1788)
 - [Custom PCB](https://easyeda.com/boogleoogle/Stargate-Control), or alternatively wire up yourself using stripboard and the like
 - 2.5" Speaker
 - Everything else required by the standard build from Glitch, unless replaced above

### Motor Shield
By using the Wemos ESP32 board, we can use 12V directly without the board getting hot. This is due to the board containing a buck converter instead of the standard linear regulator. The Motor Shield can be powered via 12V directly from the VIN pin. Install the VIN jumper, cut the 5v trace and bridge the 3v pads. You can now power directly via the barrel jack for a cleaner installation. See the [Adafruit documentation](https://learn.adafruit.com/adafruit-motor-shield-v2-for-arduino/overview) for more information. If wiring manually only the VIN, Ground, and SCL/SDA pins need to be connected.

### Audio Shield
The audio shield can be used as-is, just place the audio files directly into the root of the SD card you use. If wiring manually, the following pins must be connected:

 - 5V, GND
 - SCK/MISO/MOSI [13, 12, 11]
 - MCS, DCS, CCS, DREQ [7, 6, 4, 3]

The 2.5" speaker can just about fit in the rear of the ramp, facing to the side with the rear to the stepper motor. A small amount of hot melt glue will ensure the speaker doesn't move around.

### Custom PCB

The custom PCB has all connections and required components marked in the silkscreen. Pins with a * are used directly by the board for LEDS. Pins marked with **a** are used by the Audio Shield. The following components are required to be soldered directly to the PCB:

 - 11x BC847 SOT23 [C8547](https://lcsc.com/product-detail/Transistors-NPN-PNP_BC847A-1E_C8574.html)
 - 10x 470ohm 0805 Resistor [C114747](https://lcsc.com/product-detail/Chip-Resistor-Surface-Mount_470R-471-5_C114747.html)
 - 2x 10k ohm 0805 Resistor [C84376](https://lcsc.com/product-detail/Chip-Resistor-Surface-Mount_10KR-1002-1_C84376.html)
 - 1x 10uF 0805 Capacitor [C1713](https://lcsc.com/product-detail/Multilayer-Ceramic-Capacitors-MLCC-SMD-SMT_SAMSUNG_CL21A106KOQNNNE_10uF-106-10-16V_C1713.html)
 - 2x 74HC595 SOIC16 [C5947](https://lcsc.com/product-detail/74-Series_Nexperia_74HC595D-118_74HC595D-118_C5947.html)

The board will be at the top of the stack, so you can use standard male header. There's space for either the Motor or Audio shields between the ESP32 and the custom PCB. The other shield will need to go into the vertical mounting position.

### LEDs
The chevron LEDs are all powered via 12V, with ground going to the custom PCB. Resistors are already on the board. The calibration LED is also powered via 12V, with ground to the board - resistor also on the board. The **LDR** must be powered via 3.3v, not 12v.

Finally the Ramp / Gantry LEDs can be wired in series on each side, with no resistor. The 12v divided by the 4 LEDs results in 3V to each LED. This should nicely dim the white LEDs resulting in a well-lit but not overwhelmingly bright ramp. Ground should go to the custom PCB. This is also significantly easier to wire up than a resistor and power line to each individual LED.

I used 1206 surface mount LEDs from [Bright Components](http://bright-components.co.uk/), however any LEDs of the right colour will work. For example [C110588](https://lcsc.com/product-detail/Light-Emitting-Diodes-LED_LED-26-21-UYC-S530-A3-TR8_C110588.html) and [C71796](https://lcsc.com/product-detail/Light-Emitting-Diodes-LED_white1206-Non-warm-tones-of-white_C71796.html) from LCSC will probably work fine too. The 1206 form factor is much easier to solder than the 0604s used in Glitch's original instructions and fit fine.

Be very careful of the pads on the LEDs, they're very easy to rip off in this application. Ensure all wires have strain relief to prevent a pad from being accidentally ripped off. I used small dots of hotmelt glue to hold wires in place. Be very careful with hotmelt and PLA since it will soften and even melt the PLA.

### Setup
Configure the settings in config.h as per the documentation in the file. Then follow the instructions in Stargate.ino to set up the chevron LED ordering and calibrate the ring itself.
