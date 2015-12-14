/*
 * @file     main.c 
 * @brief    Control a 4-Digit Display by software like I2C by using DIO
 * @author   Theodore ATEBA
 * @version  1.0
 * @date 04  July 2015
 * TODO:
 *      add a LDR to control the 4-Digit Display intensity
 *      Resolve the calendar.year problem
 */

//=============================================================================
// Include Files
//=============================================================================
  #include <stdlib.h>
  #include "ch.h"
  #include "hal.h"
  #include "dslib.h" // RTC Library
  #include "tmlib.h" // 4 Digit Display Library
  #include "chprintf.h"	// Library to send data over Serial

BaseSequentialStream* pc = (BaseSequentialStream*) &SD2;

//=============================================================================
// Global variables, I2C TX and RX buffers, I2C and Serial Configurations
//=============================================================================

  // Declaration of a structure of calendar
  struct ds1307_t calendar; // We will use the time for 4-digit Display

  // Double Point state
  bool dp_flag = false;

  // Data to print on the 4-Digit Display
  // 0 ==> minutes:secondes
  // 1 ==> hours:minutes
  // 2 ==> date
  // 3 ==> day
  // 4 ==> month
  // 5 ==> year
  uint8_t data2Print = 0;

//=============================================================================
// Functions
//=============================================================================

// RTC reader thread
static THD_WORKING_AREA (waRtcReadThread, 128);
static THD_FUNCTION(RtcReadThread, arg){
  (void)arg;
  chRegSetThreadName ("RTC reader");
  while (TRUE){
    calendar = ds1307GetDate ();
    chThdSleepMilliseconds (1000);
  }
}

// RTC printer thread
static THD_WORKING_AREA (waRtcPrintThread, 128);
static THD_FUNCTION(RtcPrintThread, arg)
{
  (void)arg;
  chRegSetThreadName ("RTC printer");
  while (TRUE){
    ds1307Print (calendar); // Print RTC data on the console.
    dp_flag ^=1;
    showTime (calendar, dp_flag, data2Print); // Pirnt RTC data on the 4-Digit display
    chThdSleepMilliseconds (1000);
  }
}

// Trigger the User Button
static void extcb(EXTDriver *extp, expchannel_t channel){
  (void)extp;
  (void)channel;

  data2Print++;
  if(data2Print == 6)
    data2Print = 0;
}

static const EXTConfig extConfig = {
  {
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC, extcb},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
  }
};

/*
* @fn       int main (void)
* @brief    Application entry point.
*/
int main (void){
  // System initializations.
  halInit ();
  chSysInit ();

  // Init the I2C interface 1
  ds1307InterfaceInit ();
        
  // init the serial Driver 2 to print at the same time the date.
   sdStart (&SD2, NULL);
    
  // Set the GPIOs pin used
  palSetPadMode (GPIOC, DIO_PIN, PAL_MODE_INPUT_PULLUP);
  palClearPad (GPIOC, DIO_PIN);
  palSetPadMode (GPIOC, CLK_PIN, PAL_MODE_INPUT_PULLUP);
  palClearPad (GPIOC, CLK_PIN);

  // Activates the Button Interruption
  extStart(&EXTD1, &extConfig);
  chprintf (pc, "\n\r Chibios Real Time Clock calendar with Nucleo and DS1307 printing also on 4Digit Display.\n\r");
    
  // Used when you whant to set the calendar and clock
  calendar.seconds    = 0;
  calendar.minutes    = 54;
  calendar.hours      = 11; 
  calendar.day        = 2;
  calendar.date       = 6;
  calendar.month      = 10;
  calendar.year       = 2015;
  //ds1307SetDate(calendar);

  // Create the thread used to read the RTC DS1307
  chThdCreateStatic (waRtcReadThread, sizeof(waRtcReadThread), NORMALPRIO+1, 
  RtcReadThread, NULL);
    
  // Create the thread used to print the RTC data every seconds
  chThdCreateStatic (waRtcPrintThread, sizeof(waRtcPrintThread), NORMALPRIO, 
  RtcPrintThread, NULL);
  
  // Set the display intensity before sending data to print.
  setBrightness (0x0F);

  while (1){
    chThdSleepMilliseconds (500);
  }
}
