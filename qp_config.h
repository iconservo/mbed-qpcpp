//////////////////////////////////////////////////////////////////////////////
// Product: QP-mbed configuration file
// Last Updated for Version: 4.5.02
// Date of the Last Update: Sep 04, 2012
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
#ifndef qp_config_h
#define qp_config_h

// enable the Q-SPY software tracing instrumentation
//#define Q_SPY

// enable preemptive QK kernel
#define QK_PREEMPTIVE

// The maximum number of active objects in the application (could be up to 63)
#define QF_MAX_ACTIVE           16

// Uncomment the following macros only if you want to change the given default
//#define Q_EVT_CTOR            1
//#define Q_SIGNAL_SIZE         2
//#define QF_MAX_EPOOL          3
//#define QF_MAX_EPOOL          3
//#define QF_EVENT_SIZ_SIZE     2
//#define QF_EQUEUE_CTR_SIZE    1
//#define QF_MPOOL_SIZ_SIZE     2
//#define QF_MPOOL_CTR_SIZE     2
//#define QF_TIMEEVT_CTR_SIZE   2 

#endif                                                          // qp_config_h
