//$file${.::table.cpp} #######################################################
//
// Model: dpp.qm
// File:  ${.::table.cpp}
//
// This code has been generated by QM 4.3.0 (https://www.state-machine.com/qm).
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//
//$endhead${.::table.cpp} ####################################################
#include "qpcpp.h"
#include "dpp.h"
#include "bsp.h"

Q_DEFINE_THIS_FILE

// Active object class -------------------------------------------------------
//$declare${AOs::Table} ######################################################
namespace DPP {

//${AOs::Table} ..............................................................
class Table : public QP::QActive {
private:
    uint8_t m_fork[N_PHILO];
    bool m_isHungry[N_PHILO];

public:
    Table();

protected:
    static QP::QState initial(Table * const me, QP::QEvt const * const e);
    static QP::QState active(Table * const me, QP::QEvt const * const e);
    static QP::QState serving(Table * const me, QP::QEvt const * const e);
    static QP::QState paused(Table * const me, QP::QEvt const * const e);
};

} // namespace DPP
//$enddecl${AOs::Table} ######################################################

namespace DPP {

// helper function to provide the RIGHT neighbour of a Philo[n]
inline uint8_t RIGHT(uint8_t const n) {
    return static_cast<uint8_t>((n + (N_PHILO - 1U)) % N_PHILO);
}

// helper function to provide the LEFT neighbour of a Philo[n]
inline uint8_t LEFT(uint8_t const n) {
    return static_cast<uint8_t>((n + 1U) % N_PHILO);
}

static uint8_t const FREE = static_cast<uint8_t>(0);
static uint8_t const USED = static_cast<uint8_t>(1);

static char_t const * const THINKING = &"thinking"[0];
static char_t const * const HUNGRY   = &"hungry  "[0];
static char_t const * const EATING   = &"eating  "[0];

// Local objects -------------------------------------------------------------
static Table l_table; // the single instance of the Table active object

// Global-scope objects ------------------------------------------------------
QP::QActive * const AO_Table = &l_table; // "opaque" AO pointer

} // namespace DPP

//............................................................................
// Check for the minimum required QP version
#if (QP_VERSION < 630U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 6.3.0 or higher required
#endif

//$define${AOs::Table} #######################################################
namespace DPP {

//${AOs::Table} ..............................................................
//${AOs::Table::Table} .......................................................
Table::Table()
  : QActive(Q_STATE_CAST(&Table::initial))
{
    for (uint8_t n = 0U; n < N_PHILO; ++n) {
        m_fork[n] = FREE;
        m_isHungry[n] = false;
    }
}
//${AOs::Table::SM} ..........................................................
QP::QState Table::initial(Table * const me, QP::QEvt const * const e) {
    //${AOs::Table::SM::initial}
    (void)e; // suppress the compiler warning about unused parameter

    me->subscribe(DONE_SIG);
    me->subscribe(PAUSE_SIG);
    me->subscribe(SERVE_SIG);
    me->subscribe(TEST_SIG);

    for (uint8_t n = 0U; n < N_PHILO; ++n) {
        me->m_fork[n] = FREE;
        me->m_isHungry[n] = false;
        BSP::displayPhilStat(n, THINKING);
    }

    QS_FUN_DICTIONARY(&active);
    QS_FUN_DICTIONARY(&serving);
    QS_FUN_DICTIONARY(&paused);

    return Q_TRAN(&serving);
}
//${AOs::Table::SM::active} ..................................................
QP::QState Table::active(Table * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Table::SM::active::TEST}
        case TEST_SIG: {
            status_ = Q_HANDLED();
            break;
        }
        //${AOs::Table::SM::active::EAT}
        case EAT_SIG: {
            Q_ERROR();
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&top);
            break;
        }
    }
    return status_;
}
//${AOs::Table::SM::active::serving} .........................................
QP::QState Table::serving(Table * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Table::SM::active::serving}
        case Q_ENTRY_SIG: {
            for (uint8_t n = 0U; n < N_PHILO; ++n) { // give permissions to eat...
                if (me->m_isHungry[n]
                    && (me->m_fork[LEFT(n)] == FREE)
                    && (me->m_fork[n] == FREE))
                {
                    me->m_fork[LEFT(n)] = USED;
                    me->m_fork[n] = USED;
                    TableEvt *te = Q_NEW(TableEvt, EAT_SIG);
                    te->philoNum = n;
                    QP::QF::PUBLISH(te, me);
                    me->m_isHungry[n] = false;
                    BSP::displayPhilStat(n, EATING);
                }
            }
            status_ = Q_HANDLED();
            break;
        }
        //${AOs::Table::SM::active::serving::HUNGRY}
        case HUNGRY_SIG: {
            uint8_t n = Q_EVT_CAST(TableEvt)->philoNum;
            // phil ID must be in range and he must be not hungry
            Q_ASSERT((n < N_PHILO) && (!me->m_isHungry[n]));

            BSP::displayPhilStat(n, HUNGRY);
            uint8_t m = LEFT(n);
            //${AOs::Table::SM::active::serving::HUNGRY::[bothfree]}
            if ((me->m_fork[m] == FREE) && (me->m_fork[n] == FREE)) {
                me->m_fork[m] = USED;
                me->m_fork[n] = USED;
                TableEvt *pe = Q_NEW(TableEvt, EAT_SIG);
                pe->philoNum = n;
                QP::QF::PUBLISH(pe, me);
                BSP::displayPhilStat(n, EATING);
                status_ = Q_HANDLED();
            }
            //${AOs::Table::SM::active::serving::HUNGRY::[else]}
            else {
                me->m_isHungry[n] = true;
                status_ = Q_HANDLED();
            }
            break;
        }
        //${AOs::Table::SM::active::serving::DONE}
        case DONE_SIG: {
            uint8_t n = Q_EVT_CAST(TableEvt)->philoNum;
            // phil ID must be in range and he must be not hungry
            Q_ASSERT((n < N_PHILO) && (!me->m_isHungry[n]));

            BSP::displayPhilStat(n, THINKING);
            uint8_t m = LEFT(n);
            // both forks of Phil[n] must be used
            Q_ASSERT((me->m_fork[n] == USED) && (me->m_fork[m] == USED));

            me->m_fork[m] = FREE;
            me->m_fork[n] = FREE;
            m = RIGHT(n); // check the right neighbor

            if (me->m_isHungry[m] && (me->m_fork[m] == FREE)) {
                me->m_fork[n] = USED;
                me->m_fork[m] = USED;
                me->m_isHungry[m] = false;
                TableEvt *pe = Q_NEW(TableEvt, EAT_SIG);
                pe->philoNum = m;
                QP::QF::PUBLISH(pe, me);
                BSP::displayPhilStat(m, EATING);
            }
            m = LEFT(n); // check the left neighbor
            n = LEFT(m); // left fork of the left neighbor
            if (me->m_isHungry[m] && (me->m_fork[n] == FREE)) {
                me->m_fork[m] = USED;
                me->m_fork[n] = USED;
                me->m_isHungry[m] = false;
                TableEvt *pe = Q_NEW(TableEvt, EAT_SIG);
                pe->philoNum = m;
                QP::QF::PUBLISH(pe, me);
                BSP::displayPhilStat(m, EATING);
            }
            status_ = Q_HANDLED();
            break;
        }
        //${AOs::Table::SM::active::serving::EAT}
        case EAT_SIG: {
            Q_ERROR();
            status_ = Q_HANDLED();
            break;
        }
        //${AOs::Table::SM::active::serving::PAUSE}
        case PAUSE_SIG: {
            status_ = Q_TRAN(&paused);
            break;
        }
        default: {
            status_ = Q_SUPER(&active);
            break;
        }
    }
    return status_;
}
//${AOs::Table::SM::active::paused} ..........................................
QP::QState Table::paused(Table * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Table::SM::active::paused}
        case Q_ENTRY_SIG: {
            BSP::displayPaused(1U);
            status_ = Q_HANDLED();
            break;
        }
        //${AOs::Table::SM::active::paused}
        case Q_EXIT_SIG: {
            BSP::displayPaused(0U);
            status_ = Q_HANDLED();
            break;
        }
        //${AOs::Table::SM::active::paused::SERVE}
        case SERVE_SIG: {
            status_ = Q_TRAN(&serving);
            break;
        }
        //${AOs::Table::SM::active::paused::HUNGRY}
        case HUNGRY_SIG: {
            uint8_t n = Q_EVT_CAST(TableEvt)->philoNum;
            // philo ID must be in range and he must be not hungry
            Q_ASSERT((n < N_PHILO) && (!me->m_isHungry[n]));
            me->m_isHungry[n] = true;
            BSP::displayPhilStat(n, HUNGRY);
            status_ = Q_HANDLED();
            break;
        }
        //${AOs::Table::SM::active::paused::DONE}
        case DONE_SIG: {
            uint8_t n = Q_EVT_CAST(TableEvt)->philoNum;
            // phil ID must be in range and he must be not hungry
            Q_ASSERT((n < N_PHILO) && (!me->m_isHungry[n]));

            BSP::displayPhilStat(n, THINKING);
            uint8_t m = LEFT(n);
            /* both forks of Phil[n] must be used */
            Q_ASSERT((me->m_fork[n] == USED) && (me->m_fork[m] == USED));

            me->m_fork[m] = FREE;
            me->m_fork[n] = FREE;
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&active);
            break;
        }
    }
    return status_;
}

} // namespace DPP
//$enddef${AOs::Table} #######################################################
