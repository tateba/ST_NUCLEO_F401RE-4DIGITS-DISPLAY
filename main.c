/*
* @file     main.c 
* @brief    Control a 4-Digit Display by software like I2C by using DIO
* @author   Theodore ATEBA
* @version  1.0
* @date 04  July 2015
*/

//=============================================================================
// Include Files
//=============================================================================
    #include <stdlib.h>
    #include "ch.h"
    #include "hal.h"
    #include "dslib.h"
    #include "tmlib.h"
    #include "delay.h"

//=============================================================================
// Global variables, I2C TX and RX buffers, I2C and Serial Configurations
//=============================================================================

    // Declaration of a structure of calendar
    struct ds1307_t calendar; // We will use the time for 4-digit Display
    
    // Data to send to 4-digit Display
    uint8_t data[4];

    // Double Point state
    bool dp_flag = true;

//=============================================================================
// Functions
//=============================================================================

void showTime (struct ds1307_t clock, bool dp)
{
    uint8_t d1, d2, d3, d4;

    d1 = clock.minutes%10;
    d2 = (clock.minutes - d1)/10;
    d3 = clock.hours%10;
    d4 = (clock.hours - d3)/10;
    data[0] = encodeDigit(d4);
    if (dp)
        data[1] = 0x80 | encodeDigit(d3);
    else
        data[1] = encodeDigit(d3);
    data[2] = encodeDigit(d2);
    data[3] = encodeDigit(d1);
    setSegments(data, 4, 0);
}

// RTC reader thread
static WORKING_AREA ( waRtcReadThread, 128 );
static msg_t RtcReadThread (void *arg)
{
    msg_t status = RDY_OK;
    systime_t timeOut = MS2ST ( 4 );

    (void)arg;
    chRegSetThreadName ( "RTC reader" );
    while ( TRUE )
    {
        calendar = getDs1307Date ( &status, &timeOut );
        chThdSleepMilliseconds( 1000 );
    }
    return 0;
}

// RTC printer thread
static WORKING_AREA ( waRtcPrintThread, 128 );
static msg_t RtcPrintThread (void *arg)
{
    (void)arg;
    chRegSetThreadName ( "RTC printer" );
    while ( TRUE )
    {
        ds1307Print ( calendar );
        dp_flag ^=1;
        showTime(calendar, dp_flag);
        //showNumberDec (calendar.seconds%10, false, 4, 0);
        //showNumberDec (calendar.minutes, false, 4, 0);
        //showNumberDec (calendar.hours, false, 4, 0);
        //showNumberDec (calendar.day, false, 4, 0);
        //showNumberDec (calendar.date, false, 4, 0);
        //showNumberDec (calendar.month, false, 4, 0);
        //showNumberDec (calendar.year, false, 4, 0); // TODO: Print this data on the display  
        chThdSleepMilliseconds ( 1000 );
    }
    return 0;
}

/*
* @fn       int main (void)
* @brief    Application entry point.
*/
int main (void) 
{
    // System initializations.
    halInit ();
    chSysInit ();

    // Init the I2C interface 1
    ds1307InterfaceInit ();
        
    // init the serial Driver 2 to print at the same time the date.
     sdStart(&SD2, NULL);
    
    // Set the GPIOs pin used
    palSetPadMode(GPIOC, DIO_PIN, PAL_MODE_INPUT_PULLUP);
    palClearPad(GPIOC, DIO_PIN);
    palSetPadMode(GPIOC, CLK_PIN, PAL_MODE_INPUT_PULLUP);
    palClearPad(GPIOC, CLK_PIN);
    
    print("\n\r Chibios Real Time Clock calendar with Nucleo and DS1307.\n\r");
    
    // Used when you whant to set the calendar and clock
    calendar.seconds    = 0;
    calendar.minutes    = 32;
    calendar.hours      = 23; 
    calendar.day        = 3;
    calendar.date       = 24;
    calendar.month      = 6;
    calendar.year       = 2015;
    //setDs1307Date( &status, &tmo, calendar);

    // Create the thread used to read the RTC DS1307
    chThdCreateStatic(waRtcReadThread, sizeof(waRtcReadThread), NORMALPRIO+1, 
    RtcReadThread, NULL);
    
    // Create the thread used to print the RTC data every seconds
    chThdCreateStatic(waRtcPrintThread, sizeof(waRtcPrintThread), NORMALPRIO, 
    RtcPrintThread, NULL);
  
    // Set the display intensity before sending data to print.
    setBrightness(0x0F);

    while (1)
    {
        chThdSleepMilliseconds (1);
    }
}
