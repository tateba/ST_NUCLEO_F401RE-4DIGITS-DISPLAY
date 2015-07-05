/*
* @file     main.c 
* @brief    Control a 4-Digit Display by I2C
* @author   Theodore ATEBA
* @version  1.0
* @date 21  June 2015
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
    struct ds1307_t calendar;


//=============================================================================
// Functions
//=============================================================================

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
        chThdSleepMilliseconds( 800 );
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
        chThdSleepMilliseconds ( 1000 );
    }
    return 0;
}

/*
// GPIOC_PIN8 Toggle 50us Thread
static WORKING_AREA (waToggleThread, 128);
static msg_t ToggleThread (void *arg)
{
    (void)arg;
    chRegSetThreadName ("GPIOC_PIN8 commuter");
    while(TRUE)
    {
        palTogglePad (GPIOC, DIO_PIN);
        //bitDelay ();
        usDelay(1);
    }
    return 0;
}
*/

/*
* @fn       int main (void)
* @brief    Application entry point.
*/
int main (void) 
{
    uint16_t i, j;

    // System initializations.
    halInit ();
    chSysInit ();

    // Init the I2C interface 1
    ds1307InterfaceInit ();
        
    // init the serial Driver
    //serialDriver2Init ();
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
    chThdCreateStatic(waRtcReadThread, sizeof(waRtcReadThread), NORMALPRIO, 
    RtcReadThread, NULL);
    
    // Create the thread used to print the RTC data every seconds
    chThdCreateStatic(waRtcPrintThread, sizeof(waRtcPrintThread), NORMALPRIO, 
    RtcPrintThread, NULL);

    // Create toggle thread
    //chThdCreateStatic(waToggleThread, sizeof(waToggleThread), NORMALPRIO+3, 
    //ToggleThread, NULL);
    
    uint8_t data[] = { 0xFF, 0xFF, 0xFF, 0xFF};
    data[0] = encodeDigit(1);
    data[1] = encodeDigit(2);
    data[2] = encodeDigit(3);
    data[3] = encodeDigit(15);
    
    setBrightness(0x0F);

    setSegments (data, 4, 0);
    chThdSleepMilliseconds(3000);

    bool lz = false;
    for (j = 0; j < 2; j++)
    {
        for (i = 0; i < 1000; i++)
        {
            showNumberDec(i, lz, 4, 0);
            chThdSleepMilliseconds(1000);
        }

        lz = true;
    }

    while (1)
    {
        chThdSleepMilliseconds (1);
    }
}
