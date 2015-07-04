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

//=============================================================================
// Global variables, I2C TX and RX buffers, I2C and Serial Configurations
//=============================================================================

    // Declaration of a structure of calendar
    struct ds1307_t calendar;

    // Declare DIO Pin
    #define DIO_PIN GPIOC_PIN8
    #define CLK_PIN GPIOC_PIN9

    // TM1637 Address
    #define TM1637_I2C_COMM1    0x40
    #define TM1637_I2C_COMM2    0xC0
    #define TM1637_I2C_COMM3    0x80

    // Clock Point Definition
    #define POINT_ON    TRUE
    #define POINT_OFF   FALSE

    // Declaration of brightness
    uint8_t brightness;

/*
    // Brightness Enumeration
    enum brightness_t
    {
        BRIGHT_DARKEST  = 0,
        BRIGHT_TYPICAL  = 2,
        BRIGHTEST       = 7
    };
*/

const uint8_t digitToSegments[] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F, // 9
    0x77, // A
    0x7C, // b
    0x39, // C
    0x5E, // d
    0x79, // E
    0x71, // F
};

//=============================================================================
// Functions
//=============================================================================

/*
 * @fn void usDelay(uint16_t delay)
 * @brief Micro-seconds Delay function
 * @param[in] delay micro second to wait
 */
void usDelay (uint16_t delay)
{
    halrtcnt_t start = halGetCounterValue ();
    halrtcnt_t timeout = start + US2RTT (delay);
    while(halIsCounterWithin (start, timeout));
}

void bitDelay (void)
{
    usDelay (50);
}

void start (void)
{
    palSetPadMode(GPIOC, DIO_PIN, PAL_MODE_OUTPUT_PUSHPULL);
    bitDelay();
}

void stop (void)
{
    palSetPadMode(GPIOC, DIO_PIN, PAL_MODE_OUTPUT_PUSHPULL);
    bitDelay();
    palSetPadMode(GPIOC, CLK_PIN, PAL_MODE_INPUT_PULLUP);
    bitDelay();
    palSetPadMode(GPIOC, DIO_PIN, PAL_MODE_INPUT_PULLUP);
    bitDelay();
}

uint8_t encodeDigit(uint8_t digit)
{
    return digitToSegments [digit & 0x0F];
}

/*
 * @fn void digitBrightness(uint8_t brightness)
 * @brief set the brightness of the 4-Digit Display.
 * @note 0 is the dimmest and 255 is the brightest
 * @note set before calling four_digit_display
 * @param[in] brightness The level of the brightness we want to set
 */
void setBrightness (uint8_t  bright)
{
    brightness = bright;
}
    

bool writeByte(uint8_t byte)
{
    uint8_t data = byte;
    uint8_t i;

    // 8 Data bits
    for(i = 0; i < 8; i++)
    {
        // CLK low
        palSetPadMode(GPIOC, CLK_PIN, PAL_MODE_OUTPUT_PUSHPULL);
        bitDelay ();

        // Set data bit
        if(data & 0x01)
            palSetPadMode(GPIOC, DIO_PIN, PAL_MODE_INPUT_PULLUP);
        else
            palSetPadMode(GPIOC, DIO_PIN, PAL_MODE_OUTPUT_PUSHPULL);

        bitDelay ();

        // CLK high
        palSetPadMode(GPIOC, CLK_PIN, PAL_MODE_INPUT_PULLUP);
        bitDelay ();
        data = data >> 1;
    }

    // Wait for Acknowlege
    // CLK to Zero
    palSetPadMode(GPIOC, CLK_PIN, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOC, DIO_PIN, PAL_MODE_INPUT_PULLUP);
    bitDelay ();

    // CLK high
    palSetPadMode(GPIOC, CLK_PIN, PAL_MODE_INPUT_PULLUP);
    bitDelay ();
    uint8_t ack = palReadPad(GPIOC, DIO_PIN);
    if(ack == 0)
        palSetPadMode(GPIOC, DIO_PIN, PAL_MODE_OUTPUT_PUSHPULL);

    bitDelay ();
    palSetPadMode(GPIOC, CLK_PIN, PAL_MODE_OUTPUT_PUSHPULL);
    bitDelay();

    return ack;
}

    //void four_digit_clear (void)
    void setSegments (const uint8_t segments[], uint8_t length, uint8_t pos)
    {
        uint8_t i;

        // Write COMM1
        start ();
        writeByte (TM1637_I2C_COMM1);
        stop ();

        // Wirte COMM2 + first digit address
        start ();
        writeByte (TM1637_I2C_COMM2 + (pos & 0x03));
        
        // Write the data byte
        for(i = 0; i < length; i++)
            writeByte (segments [i]);
        stop ();

        // Write COMM3 + Brightness
        start ();
        writeByte (TM1637_I2C_COMM3 + (brightness & 0x0F));
        stop ();
    }

void showNumberDec(uint16_t num, bool leading_zero, uint8_t length, uint8_t pos)
{
    uint8_t digits[4], i;
    const uint16_t divisors[] = {1, 10, 100, 1000};
    bool leading = true;

    for(i = 0; i < 4; i++)
    {
        uint8_t divisor = divisors[4 - 1 - i];
        uint8_t d = num / divisor;

        if( d == 0)
        {
            if( leading_zero || !leading || (i == 3))
                digits[i] = encodeDigit(d);
            else
                digits[i] = 0;
        }
        else
        {
            digits[i] = encodeDigit(d);
            num -= d * divisor;
            leading = false;
        }
    }

    setSegments (digits + (4 - length), length, pos);
}
    /*
     * @note set before calling four_digit_display
     */
    //four_digit_set_point (bool point)
/*
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
*/
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
    //chThdCreateStatic(waRtcReadThread, sizeof(waRtcReadThread), NORMALPRIO, 
    //RtcReadThread, NULL);
    
    // Create the thread used to print the RTC data every seconds
    //chThdCreateStatic(waRtcPrintThread, sizeof(waRtcPrintThread), NORMALPRIO, 
    //RtcPrintThread, NULL);

    // Create toggle thread
    //chThdCreateStatic(waToggleThread, sizeof(waToggleThread), NORMALPRIO+3, 
    //ToggleThread, NULL);
    
    uint8_t data[] = { 0xFF, 0xFF, 0xFF, 0xFF};
    data[0] = encodeDigit(1);
    data[1] = encodeDigit(2);
    data[2] = encodeDigit(3);
    data[3] = encodeDigit('A');
    
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
