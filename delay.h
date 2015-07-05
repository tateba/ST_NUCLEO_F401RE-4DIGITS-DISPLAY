/*
* @file     delay.h
* @brief    Delay file
* @author   Theodore ATEBA
* @version  1.0
* @date 04  Jully 2015
*/

#ifndef _DELAY_H_
#define _DELAY_H_

//=============================================================================
// Include Files
//=============================================================================
    #include <stdlib.h>
    #include "ch.h"
    #include "hal.h"

//=============================================================================
// Functions
//=============================================================================

/*
 * @fn void usDelay(uint16_t delay)
 * @brief Micro-seconds Delay function
 * @param[in] delay micro second to wait
 */
void usDelay (uint16_t delay);

#endif // _DELAY_H_
