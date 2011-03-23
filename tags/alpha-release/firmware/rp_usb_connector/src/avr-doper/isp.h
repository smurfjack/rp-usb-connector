/*
 * Name: isp.h
 * Project: AVR-Doper
 * Author: Christian Starkjohann <cs@obdev.at>
 * Creation Date: 2006-06-21
 * Tabsize: 4
 * Copyright: (c) 2006 by Christian Starkjohann, all rights reserved.
 * License: GNU GPL v2 (see License.txt) or proprietary (CommercialLicense.txt)
 * Revision: $Id: isp.h 280 2007-03-20 12:03:11Z cs $
 */

/*
General Description:
This module implements the STK500v2 primitives for In System Programming.
Functions accept parameters directly from the input data stream and prepare
results for the output data stream, where appropriate.
*/

#ifndef __isp_h_included__
#define __isp_h_included__

#include "../stk500protocol.h"


_u8   ispEnterProgmode(stkEnterProgIsp_t *param);
void  ispLeaveProgmode(stkLeaveProgIsp_t *param);
_u8   ispChipErase(stkChipEraseIsp_t *param);
_u8   ispProgramMemory(stkProgramFlashIsp_t *param, _u8 isEeprom);
_u16  ispReadMemory(stkReadFlashIsp_t *param, stkReadFlashIspResult_t *result, _u8 isEeprom);
_u8   ispProgramFuse(stkProgramFuseIsp_t *param);
_u8   ispReadFuse(stkReadFuseIsp_t *param);
_u16  ispMulti(stkMultiIsp_t *param, stkMultiIspResult_t *result);


#endif  /* __isp_h_included__ */
