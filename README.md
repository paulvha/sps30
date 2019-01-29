# Sensirion SPS30

## ===========================================================

A program to set instructions and get information from an SPS30. It has been
tested to run either UART or I2C communcation on ESP32, MEGA2560 and UNO.
<br> A detailed description of the options and findings are in SPS30.odt

## Getting Started
As part of a larger project I am looking at analyzing and understanding the air quality.
I have done a number of projects on air-sensors. The SP30 sensor is a new kid on the block
that looks interesting. This is the first draft version of a working driver + examples.
More work to be done to create examples and compare against other sensors

A word of warning: the SPS30 needs a female plug of ZHR-5 from JST Sales America Inc.
I have not been able to find a good source for that and was glad to buy the Sparkfun version
(https://www.sparkfun.com/products/15103) which does include a cable with this plug.

## Prerequisites
Examples 4 and 5 have a dependency on other libraries

## Software installation
Obtain the zip and install like any other

## Program usage
### Program options
Please see the description in the top of the sketch and read the documentation (odt)

## Versioning

### version 1.0 / January 2019
 * Initial version Arduino, ESP32, UNO

### version 1.0.1 / January 2019
 * Added examples 4 (with DS18x20) and 5 (with BME280)

## Author
* Paul van Haastrecht (paulvha@hotmail.com)

## License
This project is licensed under the GNU GENERAL PUBLIC LICENSE 3.0

## Acknowledgements
Make sure to read the datasheet from Sensirion. While draft and not 100% correct yet,
it does provide good starting point.
