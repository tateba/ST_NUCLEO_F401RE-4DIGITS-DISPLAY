/*
* @file     tmlib.h
* @brief    TM1637 C library
* @author   Theodore ATEBA
* @version  1.0
* @date 04  Jully 2015
*/

#ifndef _TM1637_H_
#define _TM1637_H_

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
//uint8_t brightness;

/*
// Brightness Enumeration
enum brightness_t{
    BRIGHT_DARKEST  = 0,
    BRIGHT_TYPICAL  = 2,
	BRIGHTEST       = 7
};
*/


//=============================================================================
// Functions
//=============================================================================

/*
 * @fn      bitDelay
 * @brief   Width of a bit on DIO bus
 */
void bitDelay(void);

/*
 * @fn      start
 * @brief   Generation of the start condition on the DIO bus.
 */
void start(void);

/*
 * @fnn     stop
 * @brief   Generation of the stop condition on the DIO bus.
 */
void stop(void);

/*
 * @fn          encodeDigit
 * @brief       Encode the data to print on the 4-digit Display
 * @param[in]   digit the data to convert for the Display
 */
uint8_t encodeDigit(uint8_t digit);

/*
 * @fn          digitBrightness
 * @brief       set the brightness of the 4-Digit Display.
 * @note        0 is the dimmest and 255 is the brightest
 * @note        set before calling four_digit_display
 * @param[in]   brightness The level of the brightness we want to set
 */
void setBrightness(uint8_t  bright);
    
/*
 * @fn          writeByte
 * @brief       write a byte to the GPIO pin used as DIO
 * @param[in]   byte Data to write on the DIO pin
 */
bool writeByte(uint8_t byte);

//void four_digit_clear (void)

/*
 * @fn          setSegments
 * @brief       print the data on the segments
 * @param[in]   segments array of data to print on the 4-digit Display
 * @param[in]   length Size of the array of data to print
 * @param[in]   pos Position of the first digit to update
 */
void setSegments(const uint8_t segments[], uint8_t length, uint8_t pos);

/*
 * @fn          showNumberDec
 * @brief       Print Decimal number on the 4-Digit Display
 * @param[in]   num number to print
 * @param[in]   leading_zero 
 * @param[in]   length
 * @param[in]   pos position of the digit
 */
void showNumberDec(uint16_t num, bool leading_zero, uint8_t length, uint8_t pos);

/*
 * @fn          showTime
 * @brief       Convert and print data on 4Digit-Display
 * @param[in]   clock data date structure.
 * @param[in]   dp flag of double point for clock
 */
void showTime (struct ds1307_t clock, bool dp, uint8_t msg);

#endif // _TM1637_H_
