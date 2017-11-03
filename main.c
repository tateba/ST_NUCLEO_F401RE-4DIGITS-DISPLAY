/**
 *
 * @file     main.c
 *
 * @brief    Real Time Clock demo for DS1307 driver file.
 *
 * @author   Theodore Ateba, tf.ateba@gmail.com
 *
 * @date 21  June 2015
 *
 */

/*==========================================================================*/
/* Include Files.                                                           */
/*==========================================================================*/

/* Standard files. */
#include <stdlib.h>

/* CbibiOS files. */
#include "ch.h"
#include "hal.h"

/* Driver files. */
#include "ds1307.h"

/*==========================================================================*/
/* Variables, drivers configurations structures.                            */
/*==========================================================================*/

//BaseSequentialStream* chp = (BaseSequentialStream*) &SD2;

/* Declaration of a calendar. */
rtcDriver_t calendar;

/*==========================================================================*/
/* Functions.                                                               */
/*==========================================================================*/

/**
 * @brief   RTC reader thread.
 */
static THD_WORKING_AREA(waRtcReadThread, 128);
static THD_FUNCTION(RtcReadThread, arg) {

  (void)arg;
  chRegSetThreadName("RTC reader");

  while (TRUE) {
    ds1307GetClock(&calendar);
    chThdSleepMilliseconds(800);
  }
}

/**
 * @brief   RTC printer thread.
 */
static THD_WORKING_AREA(waRtcPrintThread, 128);
static THD_FUNCTION(RtcPrintThread, arg) {
  (void)arg;
  chRegSetThreadName("RTC printer");

  while (TRUE) {
    ds1307PrintClock(&calendar);
    chThdSleepMilliseconds(1000);
  }
}

/**
 * @brief   Application entry point.
 */
int main (void) {

  /* System initializations. */
  halInit();
  chSysInit();

  /* Init the I2C interface 1. */
  ds1307InitInterface();

  /* Init the serial Driver. */
  sdStart(&SD2, NULL);
  print("\n\r Chibios Real Time Clock calendar with Nucleo and DS1307.\n\r");

  /* Used when you whant to set the calendar and clock. */
  /* TODO: Try to use the compilation time of the program to update the RTC. */
  /* TODO: Use the shell to update the RTC. */
  /* TODO: Use the shell to read the RTC. */

  /* Configure the RTC parameters. */
  calendar.rtc.seconds  = 0;
  calendar.rtc.minutes  = 32;
  calendar.rtc.hours    = 23;
  calendar.rtc.day      = 3;
  calendar.rtc.date     = 24;
  calendar.rtc.month    = 6;
  calendar.rtc.year     = 2015;
  calendar.refYear      = 2000;

  /* TODO: Add shell to read RTC data. */
  /* TODO: Use the Shell to set the RTC parameter. */

  //ds1307SetClock(&calendar);

  /* Create the thread used to read the RTC DS1307. */
  chThdCreateStatic(waRtcReadThread, sizeof(waRtcReadThread), NORMALPRIO,
                    RtcReadThread, NULL);

  /* Create the thread used to print the RTC data every seconds. */
  chThdCreateStatic(waRtcPrintThread, sizeof(waRtcPrintThread), NORMALPRIO,
                    RtcPrintThread, NULL);

  while (TRUE) {
    chThdSleepMilliseconds (1);
  }
}

