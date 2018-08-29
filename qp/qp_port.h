//////////////////////////////////////////////////////////////////////////////
// Product: QP/C++ port to ARM Cortex, selectable Vanilla/QK, mbed compiler
// Last Updated for Version: 4.5.02
// Date of the Last Update:  Sep 04, 2012
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Alternatively, this program may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GNU General Public License and are specifically designed for
// licensees interested in retaining the proprietary status of their code.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Contact information:
// Quantum Leaps Web sites: http://www.quantum-leaps.com
//                          http://www.state-machine.com
// e-mail:                  info@quantum-leaps.com
//////////////////////////////////////////////////////////////////////////////
#ifndef qp_port_h
#define qp_port_h

#include "qp_config.h"    // QP configuration defined at the application level

//............................................................................



                                                // QF interrupt disable/enable
#define QF_INT_DISABLE()           // __disable_irq()
#define QF_INT_ENABLE()            // __enable_irq()

                                             // QF critical section entry/exit
// QF_CRIT_STAT_TYPE not defined: "unconditional interrupt unlocking" policy
#define QF_CRIT_ENTRY(dummy)       // __disable_irq()
#define QF_CRIT_EXIT(dummy)        // __enable_irq()

#ifdef QK_PREEMPTIVE
                                                // QK interrupt entry and exit
    #define QK_ISR_ENTRY() do { \
        /*__disable_irq();*/ \
        ++QK_intNest_; \
        QF_QS_ISR_ENTRY(QK_intNest_, QK_currPrio_); \
        /*__enable_irq();*/ \
    } while (false)

    #define QK_ISR_EXIT()  do { \
        /*__disable_irq();*/ \
        QF_QS_ISR_EXIT(QK_intNest_, QK_currPrio_); \
        --QK_intNest_; \
          *Q_UINT2PTR_CAST(uint32_t, 0xE000ED04U) = \
              static_cast<uint32_t>(0x10000000U); \
         /* __enable_irq();*/ \
      } while (false)

#else
    #define QK_ISR_ENTRY() ((void)0)
    #define QK_ISR_EXIT()  ((void)0)
#endif

#ifdef Q_SPY
    #define QS_TIME_SIZE        4
    #define QS_OBJ_PTR_SIZE     4
    #define QS_FUN_PTR_SIZE     4
#endif

#include <stdint.h>    // mbed compiler supports standard exact-width integers

#include "qp.h"                           // QP platform-independent interface

#endif                                                            // qp_port_h
