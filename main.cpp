#include "qp_port.h"
#include "blinky.h"
#include "bsp.h"

//............................................................................
int main(void) {
    static QP::QEvt const *blinkyQueueSto[5];
    //static QP::QSubscrList   subscrSto[MAX_PUB_SIG];
    //static QF_MPOOL_EL(QP::QEvt) smlPoolSto[10];         // small pool

    QP::QF::init();   // initialize the framework and the underlying RT kernel

    BSP_init();                                          // initialize the BSP
    
    //QP::QF::psInit(&subscrSto[0], Q_DIM(subscrSto)); // init publish-subscribe

                                                  // initialize event pools...
    //QP::QF::poolInit(&smlPoolSto[0], sizeof(smlPoolSto),
    //                                 sizeof(smlPoolSto[0]));

                                                // start the active objects...
    AO_Blinky->start(1U,
                    &blinkyQueueSto[0], Q_DIM(blinkyQueueSto),
                    static_cast<void *>(0), 0U);

    return QP::QF::run();                            // run the QF application
}
