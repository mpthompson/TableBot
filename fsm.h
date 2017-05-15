/*
    Copyright (c) 2006 Michael P. Thompson <mpthompson@gmail.com>

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.

    $Id:$

    Finite State Machine helper macros.

    Based on code by Massimo Manca, Micron Engineering di M. Manca 2006.

    Further modified by Mike Thompson to eliminate less efficient
    case statement implementation and instead use goto labels and 
    goto statements.  This code depends on special label features in 
    GNU GCC C compiler and may not work with other compilers.
*/



#ifndef _FSM_H_
#define _FSM_H_ 1

#define FSM_EXIT_STATE                      0
#define FSM_LABLE(line)                     pstate ## line
#define FSM_PSTATE(line)                    FSM_LABLE(line)
#define FSM_BEGIN(first_state)              static uint16_t fsm_state = (uint16_t) &&first_state;   \
                                            static uint16_t fsm_first = (uint16_t) &&first_state;   \
                                            uint8_t fsm_suspend = 1;                                \
                                            (void) fsm_first; (void) fsm_suspend;                   \
                                            if (fsm_state) goto *((void*)fsm_state); else goto fsm_end;
#define FSM_END                             fsm_end:                                                \
                                            return fsm_state;
#define FSM_STATE_BEGIN(this_state)         this_state :                    
#define FSM_STATE_END                       return fsm_state;               

#define fsm_restart()                               \
    fsm_state = fsm_first;                          \
    return fsm_state;

#define fsm_return()                                \
    return fsm_state;

#define fsm_change_state(cond, next_state)          \
    if (cond) {                                     \
        fsm_state = (uint16_t) &&next_state;        \
        return fsm_state;                           \
    }

#define fsm_abort()                                 \
    fsm_state = FSM_EXIT_STATE;                     \
    return fsm_state;

#define fsm_exit()                                  \
    fsm_state = (uint16_t) &&fsm_first;             \
    return FSM_EXIT_STATE;

#define fsm_suspend()                               \
    fsm_suspend = 0;                                \
    fsm_state = (uint16_t) &&FSM_PSTATE(__LINE__);  \
    FSM_PSTATE(__LINE__):                           \
    if (!fsm_suspend) return fsm_state;

#define fsm_wait_until(condition)                   \
    fsm_state = (uint16_t) &&FSM_PSTATE(__LINE__);  \
    FSM_PSTATE(__LINE__):                           \
    if (!(condition)) return fsm_state;

#define fsm_wait_while(condition)                   \
    fsm_state = (uint16_t) &&FSM_PSTATE(__LINE__);  \
    FSM_PSTATE(__LINE__):                           \
    if (condition) return fsm_state;

#define fsm_checkpoint()                            \
    fsm_state = (uint16_t) &&FSM_PSTATE(__LINE__);  \
    FSM_PSTATE(__LINE__):

#define fsm_is_running(fsm)                         \
    (fsm != FSM_EXIT_STATE) 

#define fsm_wait_exit(fsm)                          \
    while (fsm_is_running(fsm))

/*
 * Obsolete Finite State Machine code based on case statements.
 *
#define FSM_DECLARE(name_args)      int name_args
#define FSM_FIRST_STATE             -127
#define FSM_EXIT_STATE              0
#define FSM_STATES                  enum {
#define FSM_BEGIN                   }; static int16_t fsm_state = FSM_FIRST_STATE; int16_t fsm_suspend = 1; fsm_suspend = fsm_suspend; switch(fsm_state) {
#define FSM_END                     default: ; } return fsm_state;
#define FSM_STATE_BEGIN(state_name) case state_name :
#define FSM_STATE_END               break;

#define fsm_restart()                               \
    fsm_state = FSM_FIRST_STATE

#define fsm_return()                                \
    return fsm_state

#define fsm_change_state(cond, next_state)          \
    if (cond) {                                     \
        fsm_state = next_state;                     \
        return fsm_state;                           \
    }

#define fsm_abort()                                 \
    do {                                            \
        fsm_state = FSM_EXIT_STATE;                 \
        return FSM_EXIT_STATE;                      \
    } while(0)

#define fsm_exit()                                  \
    do {                                            \
        fsm_state = FSM_FIRST_STATE;                \
        return FSM_EXIT_STATE;                      \
    } while(0)
   
#define fsm_suspend()                               \
    do {                                            \
        fsm_suspend = 0;                            \
        fsm_state = __LINE__;                       \
        case __LINE__:                              \
        if (!fsm_suspend) return fsm_state;         \
   } while(0)

#define fsm_wait_until(condition)                   \
    do {                                            \
        fsm_state = __LINE__;                       \
        case __LINE__:                              \
        if (!(condition)) return fsm_state;         \
    } while(0)
  
#define fsm_wait_while(condition)                   \
    do {                                            \
        fsm_state = __LINE__;                       \
        case __LINE__:                              \
        if (condition) return fsm_state;            \
    } while(0)


#define fsm_checkpoint()                            \
    do {                                            \
        fsm_state = __LINE__;                       \
        case __LINE__:;                             \
    } while(0)


#define fsm_is_running(fsm)                         \
    (fsm != FSM_EXIT_STATE)

#define fsm_wait_exit(fsm)                          \
    while (fsm_is_running(fsm))
#endif
*/

#endif // _FSM_H_
