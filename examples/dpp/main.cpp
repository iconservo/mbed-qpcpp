#include "mbed.h"
#include "SeggerRTT.h"
#include "qpcpp.h"
#include "dpp.h"
#include "bsp.h"
#include "blinky.h"

using namespace DPP;

FileHandle* mbed::mbed_override_console(int fd) {
    static SeggerRTT rtt;
    return &rtt;
}

void tick_callback(void) {
    static int tick_cntr = 0;
    printf("tick_cntr: %d\r\n", tick_cntr++);
}
    
int main(int argc, char *argv[]) {
    static QP::QEvt const *tableQueueSto[N_PHILO];
    static QP::QEvt const *philoQueueSto[N_PHILO][N_PHILO];
    static QP::QSubscrList subscrSto[DPP::MAX_PUB_SIG];
    static QF_MPOOL_EL(DPP::TableEvt) smlPoolSto[2*N_PHILO];

    printf("\r\nQP/C++ DPP example init\r\n");

#if 0    
    mbed::mbed_event_queue()->call_every(1000, tick_callback);
#endif
    
    QP::QF::init();  // initialize the framework and the underlying RT kernel

    DPP::BSP::init(argc, argv); // initialize the BSP

    QP::QF::psInit(subscrSto, Q_DIM(subscrSto)); // init publish-subscribe

    // initialize event pools...
    QP::QF::poolInit(smlPoolSto,
                     sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // object dictionaries...
    QS_OBJ_DICTIONARY(AO_Table);
    QS_OBJ_DICTIONARY(AO_Philo[0]);
    QS_OBJ_DICTIONARY(AO_Philo[1]);
    QS_OBJ_DICTIONARY(AO_Philo[2]);
    QS_OBJ_DICTIONARY(AO_Philo[3]);
    QS_OBJ_DICTIONARY(AO_Philo[4]);

    // signal dictionaries...
    QS_SIG_DICTIONARY(DONE_SIG,    (void *)0);
    QS_SIG_DICTIONARY(EAT_SIG,     (void *)0);
    QS_SIG_DICTIONARY(PAUSE_SIG,   (void *)0);
    QS_SIG_DICTIONARY(SERVE_SIG,   (void *)0);
    QS_SIG_DICTIONARY(TEST_SIG,    (void *)0);
    QS_SIG_DICTIONARY(HUNGRY_SIG,  (void *)0);
    QS_SIG_DICTIONARY(TIMEOUT_SIG, (void *)0);

    // start the active objects...
    for (uint8_t n = 0U; n < N_PHILO; ++n) {
        AO_Philo[n]->start(osPriorityNormal + n + 1U,
        //AO_Philo[n]->start(osPriorityNormal,
                           philoQueueSto[n], Q_DIM(philoQueueSto[n]),
                           (void *)0, 0U);
    }
    AO_Table->start(osPriorityNormal + N_PHILO + 1U,
    //AO_Table->start(osPriorityNormal,
                    tableQueueSto, Q_DIM(tableQueueSto),
                    (void *)0, 0U);
    
#if 0
    static QEvt const* blinkyQSto[10];  // Event queue storage for Blinky
    
    // instantiate and start the active objects...
    AO_Blinky->start(osPriorityNormal,               // priority
                     blinkyQSto, Q_DIM(blinkyQSto),  // event queue
                     (void*)0, 0U);                  // stack (unused)    
#endif
    
    return QP::QF::run(); // run the QF application
}
