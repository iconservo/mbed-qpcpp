#ifndef __mbed_qf_port_h
#define __mbed_qf_port_h

#include "mbed.h"

#if QF_PORT_DEBUG
#define QF_PORT_DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
#define QF_PORT_DEBUG_PRINTF(...)
#endif

#define QF_EQUEUE_TYPE QEQueue
#define QF_THREAD_TYPE int

#define QF_OS_OBJECT_TYPE EventFlags

#define QF_MAX_ACTIVE 32

// various QF object sizes configuration for this port
#define QF_EVENT_SIZ_SIZE    4
#define QF_EQUEUE_CTR_SIZE   4
#define QF_MPOOL_SIZ_SIZE    4
#define QF_MPOOL_CTR_SIZE    4
#define QF_TIMEEVT_CTR_SIZE  4

#define QF_INT_DISABLE() \
    { CriticalSectionLock::enable(); }
#define QF_INT_ENABLE() \
    { CriticalSectionLock::disable(); }

class OneSlotSemaphore : public Semaphore {
   public:   
    OneSlotSemaphore() : Semaphore(1) {}
};

// QF critical section
#define QF_CRIT_STAT_TYPE OneSlotSemaphore
#define QF_CRIT_ENTRY(semaphore) semaphore.wait()
#define QF_CRIT_EXIT(semaphore)  semaphore.release()

#include "qep_port.h"  // QEP port
#include "qequeue.h"   // this QP port uses the native QF event queue
#include "qmpool.h"    // this QP port uses the native QF memory pool
#include "qf.h"        // QF platform-independent public interface

namespace QP {

// set clock tick rate and p-thread priority
void QF_setTickRate(uint32_t ticksPerSec, int_t tickPrio);

// clock tick callback (provided in the app)
void QF_onClockTick(void);

}  // namespace QP

#ifdef QP_IMPL

#define QF_PORT_EQUEUE_DEBUG_PRINTF(...)

#define QACTIVE_EQUEUE_WAIT_(me_)                                 \
    while ((me_)->m_eQueue.m_frontEvt == static_cast<QEvt*>(0)) { \
        QF_PORT_EQUEUE_DEBUG_PRINTF("QACTIVE_EQUEUE_WAIT_\r\n"); \
        (me_)->m_osObject.wait_all(0x01);  \
    }

#define QACTIVE_EQUEUE_SIGNAL_(me_)  \
    do {                             \
        QF_PORT_EQUEUE_DEBUG_PRINTF("QACTIVE_EQUEUE_SIGNAL_\r\n"); \
        (me_)->m_osObject.set(0x01);  \
    } while (0)



// Mbed-specific scheduler locking (not used at this point)
#define QF_SCHED_STAT_
#define QF_SCHED_LOCK_(dummy) ((void)0)
#define QF_SCHED_UNLOCK_() ((void)0)

#define QF_EPOOL_TYPE_ QMPool
#define QF_EPOOL_INIT_(p_, poolSto_, poolSize_, evtSize_) (p_).init((poolSto_), (poolSize_), (evtSize_))
#define QF_EPOOL_EVENT_SIZE_(p_) static_cast<uint_fast16_t>((p_).getBlockSize())
#define QF_EPOOL_GET_(p_, e_, m_) ((e_) = static_cast<QEvt*>((p_).get((m_))))
#define QF_EPOOL_PUT_(p_, e_) ((p_).put(e_))

#endif /* ifdef QP_IMPL */

#endif