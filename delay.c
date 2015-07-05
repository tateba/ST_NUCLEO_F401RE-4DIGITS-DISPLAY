/*
* @file     delay.c 
* @brief    Delay file
* @author   Theodore ATEBA
* @version  1.0
* @date 04  Jully 2015
*/

//=============================================================================
// Include Files
//=============================================================================
    #include "delay.h"

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
