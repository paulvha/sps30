/************************************************************************************
 *  Copyright (c) January 2019, version 1.0     Paul van Haastrecht
 *
 *  Version 1.4.18 Paul van Haastrecht / January 2026
 *  - added support and this example21 for UNOQ
 *  
 *  =========================  Highlevel description ================================
 *
 *  This basic reading example sketch will connect to an SPS30 for getting data and
 *  display the available data on an UNOQ.
 *
 *  The SPS30 can be connected to Serial OR I2C
 *
 *  The output can be send to Serial (connect a seperate FTDI to pin 0, pin 1 and GND) OR
 *  to the bridge / Monitor 
 *
 *  =========================  Hardware connections =================================
 *  /////////////////////////////////////////////////////////////////////////////////
 *  ## SERIAL  SERIAL   SERIAL   SERIAL  SERIAL  SERIAL  SERIAL  SERIAL  SERIAL    ##
 *  /////////////////////////////////////////////////////////////////////////////////
 *
 *  SPS30 pin     UNOQ
 *  1 VCC -------- 5v
 *  2 RX  -------- TX  1
 *  3 TX  -------- RX  0
 *  4 Select      (NOT CONNECTED)
 *  5 GND -------- GND
 *
 *  The output / display can only be the BRIDGE / Monitor
 *
 *  //////////////////////////////////////////////////////////////////////////////////
 *  ## I2C I2C I2C  I2C I2C I2C  I2C I2C I2C  I2C I2C I2C  I2C I2C I2C  I2C I2C I2C ##
 *  //////////////////////////////////////////////////////////////////////////////////
 *
 *  SPS30 pin     UNOQ
 *  1 VCC -------- 5v        10k
 *  2 SDA -------- SDA------=====---- 3v3
 *  3 SCL -------- SCL------=====---- 3v3
 *  4 Select ----- GND (select I2c)
 *  5 GND -------- GND
 *
 *  Make sure to add pull-up resistors on both SDA and SCL to 3v3.
 *
 *  ================================= PARAMETERS =====================================
 *
 *  From line 68 there are configuration parameters for the program
 *
 *  ================================ Disclaimer ======================================
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *  ==================================================================================
 *
 *  NO support, delivered as is, have fun, good luck !!
 */

#include "sps30.h"
#include <Arduino_RouterBridge.h>   // special for UNO-Q

#define I2C 1
#define SERIAL 2
#define BRIDGE 3

/////////////////////////////////////////////////////////////
/* define communication channel to use for SPS30. valid options:
 *   I2C       use I2C communication
 *   SERIAL    use Serial
 */
/////////////////////////////////////////////////////////////
#define INPUT I2C

/////////////////////////////////////////////////////////////
/* define output / display channel to use. valid options:
 *   SERIAL     ONLY use in combination with I2C SPS30-communication
 *   BRIDGE     use in combination with I2C or SERIAL SPS30-communication
 */
/////////////////////////////////////////////////////////////
#define OUTPUT BRIDGE

/////////////////////////////////////////////////////////////
/* define driver debug ONLY when OUTPUT is SERIAL
 * 0 : no messages
 * 1 : request sending and receiving
 * 2 : request sending and receiving + show protocol errors */
 //////////////////////////////////////////////////////////////
#define DEBUG 0

///////////////////////////////////////////////////////////////
/////////// NO CHANGES BEYOND THIS POINT NEEDED ///////////////
///////////////////////////////////////////////////////////////

// check on correct defines
#if OUTPUT == SERIAL 
  #if INPUT == SERIAL
    #error "Can not use same Serial for input AND output"
  #else
    #define DISPLAY Serial
  #endif
#elif OUTPUT == BRIDGE
  #define DISPLAY Monitor
#else
  #error "Incorrect OUTPUT defined"
#endif

#if INPUT == SERIAL
  #define SP30_COMMS SERIALPORT
#elif INPUT == I2C
  #define SP30_COMMS I2C_COMMS
#else
  #error "Incorrect INPUT defined"
#endif

// display buffer
char out[300];

// create constructor
SPS30 sps30;

void setup() {

  DISPLAY.begin(115200);

#if INPUT == SERIAL
  serialTrigger((char *) "SPS30-Example21: Basic reading on UNOQ from SERIAL. press <enter> to start");
#else
  serialTrigger((char *) "SPS30-Example21: Basic reading on UNOQ from I2C. press <enter> to start");
#endif

  DISPLAY.print(F("Trying to connect\r\n"));

  // set driver debug level ONLY when OUTPUT is SERIAL
#if OUTPUT == SERIAL
  sps30.EnableDebugging(DEBUG);
#endif

  // Begin communication channel;
  if (! sps30.begin(SP30_COMMS))
    Errorloop((char *) "could not initialize communication channel.", 0);

  // check for SPS30 connection
  if (! sps30.probe()) Errorloop((char *) "could not probe / connect with SPS30.", 0);
  else  DISPLAY.print(F("Detected SPS30.\r\n"));

  // reset SPS30 connection
  if (! sps30.reset()) Errorloop((char *) "could not reset.\r", 0);

  // read device info
  GetDeviceInfo();

  // start measurement
  if (sps30.start()) DISPLAY.print(F("Measurement started\r\n"));
  else Errorloop((char *) "Could NOT start measurement\r", 0);

  serialTrigger((char *) "Hit <enter> to continue reading\r");

  if (SP30_COMMS == I2C_COMMS) {
    if (sps30.I2C_expect() == 4)
      DISPLAY.println(F(" !!! Due to I2C buffersize only the SPS30 MASS concentration is available !!!\r\n"));
  }
}

void loop() {
  read_all();
  // give it 3 seconds
  delay(3000);
}

/**
 * @brief : read and display device info
 */
void GetDeviceInfo()
{
  char buf[32];
  uint8_t ret;
  SPS30_version v;

  // try to read serial number
  ret = sps30.GetSerialNumber(buf, 32);
  if (ret == SPS30_ERR_OK) {
    if(strlen(buf) > 0)  snprintf(out, sizeof(out),"Serial number : %s\r\n", buf);
    else snprintf(out, sizeof(out),"Serial number : not available\r\n");
    
    DISPLAY.print(out);
  }
  else
    ErrtoMess((char *) "could not get serial number. ", ret);

  // try to get product name
  ret = sps30.GetProductName(buf, 32);
  if (ret == SPS30_ERR_OK)  {
    if(strlen(buf) > 0) snprintf(out, sizeof(out),"Product name : %s\r\n", buf);
    else snprintf(out, sizeof(out),"Product name : not available\r\n");
    
    DISPLAY.print(out);
  }
  else
    ErrtoMess((char *) "could not get product name. ", ret);

  // try to get version info
  ret = sps30.GetVersion(&v);
  if (ret != SPS30_ERR_OK) {
    DISPLAY.println(F("Can not read version info\r\n"));
    return;
  }

  // try to get version info
  ret = sps30.GetVersion(&v);
  if (ret != SPS30_ERR_OK) {
    DISPLAY.println(F("Can not read version info\r\n"));
    return;
  }
  
  if (SP30_COMMS != I2C_COMMS) {
    snprintf(out,sizeof(out),"Firmware level: %d.%d, Hardware level: %d, SHDLC protocol: %d, Library level: %d.%d\r\n ", v.major, v.minor, v.HW_version, v.SHDLC_major, v.DRV_major, v.DRV_minor );
  }
  else {
    snprintf(out,sizeof(out),"Firmware level: %d.%d, Library level: %d.%d\r\n", v.major, v.minor, v.DRV_major, v.DRV_minor);
  }

  DISPLAY.println(out);
}

/**
 * @brief : read and display all values
 */
bool read_all()
{
  static bool header = true;
  uint8_t ret, error_cnt = 0;
  struct sps_values val;
 
  // loop to get data
  do {

    ret = sps30.GetValues(&val);

    // data might not have been ready
    if (ret == SPS30_ERR_DATALENGTH){

        if (error_cnt++ > 3) {
          ErrtoMess((char *) "Error during reading values: ",ret);
          return(false);
        }
        delay(1000);
    }

    // if other error
    else if(ret != SPS30_ERR_OK) {
      ErrtoMess((char *) "Error during reading values: ", ret);
      return(false);
    }

  } while (ret != SPS30_ERR_OK);

  // only print header first time
  if (header) {
    snprintf(out, sizeof(out),"%s\r\n%s\r\n%s\r\n",
    "-------------Mass -----------    ------------- Number --------------   -Average-",
    "     Concentration [μg/m3]             Concentration [#/cm3]             [μm]",
    "P1.0\tP2.5\tP4.0\tP10\tP0.5\tP1.0\tP2.5\tP4.0\tP10\tPartSize");
    DISPLAY.print(out);
    header = false;
  }

  snprintf(out, sizeof(out), "%2.2f\t%2.2f\t%2.2f\t%2.2f\t%2.2f\t%2.2f\t%2.2f\t%2.2f\t%2.2f\t%2.2f\r\n",
  val.MassPM1,val.MassPM2,val.MassPM4, val.MassPM10, val.NumPM0,val.NumPM1, val.NumPM2, val.NumPM4,val.NumPM10, val.PartSize);
  DISPLAY.print(out);
 
  return(true);
}

/**
 *  @brief : continued loop after fatal error
 *  @param mess : message to display
 *  @param r : error code
 *
 *  if r is zero, it will only display the message
 */
void Errorloop(char *mess, uint8_t r)
{
  char buf[80] = "";
  if (r) sps30.GetErrDescription(r, buf, 80);
  snprintf(out,sizeof(out), "%s %s %s\r\n", mess, buf, " Program on hold");
  for(;;) delay(100);
}

/**
 *  @brief : display error message
 *  @param mess : message to display
 *  @param r : error code
 */
void ErrtoMess(char *mess, uint8_t r)
{
  char buf[80];
  sps30.GetErrDescription(r, buf, 80);
  snprintf(out, sizeof(out), "%s,%s\r\n", mess, buf);
}

/**
 * serialTrigger prints repeated message, then waits for enter
 * to come in from the serial port.
 */
void serialTrigger(char * mess)
{
  DISPLAY.println();

  while (!DISPLAY.available()) {
    DISPLAY.println(mess);
    delay(2000);
  }

  while (DISPLAY.available()) // flush
    DISPLAY.read();
}
