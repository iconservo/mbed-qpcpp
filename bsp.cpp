#include "qp_port.h"
#include "blinky.h"
#include "bsp.h"
//#include "LPC17xx.h"
#ifdef Q_SPY
    #include "mbed.h"             // mbed is used only for the built-in serial
#endif

//////////////////////////////////////////////////////////////////////////////
Q_DEFINE_THIS_FILE

/*enum ISR_Priorities {      // ISR priorities starting from the highest urgency
    GPIOPORTA_PRIO,
    SYSTICK_PRIO
    // ...
};*/

// Local-scope objects -------------------------------------------------------
//#define LED_PORT      LPC_GPIO1
#define LED1_BIT     (1U << 18)
#define LED2_BIT     (1U << 20)
#define LED3_BIT     (1U << 21)
#define LED4_BIT     (1U << 23)

#ifdef Q_SPY
    QP::QSTimeCtr l_tickTime;
    QP::QSTimeCtr l_tickPeriod;
    static uint8_t l_SysTick_Handler;

    #define QSPY_BAUD_RATE  115200U

    Serial l_qspy(USBTX, USBRX);
#endif

//............................................................................
void BSP_init(void) {
    /* set the system clock as specified in lm3s_config.h (20MHz from PLL)
    SystemInit();

                                                     // set LED port to output
    LED_PORT->FIODIR |= (LED1_BIT | LED2_BIT | LED3_BIT | LED4_BIT);
    
                                                             // clear the LEDs
    LED_PORT->FIOCLR  = (LED1_BIT | LED2_BIT | LED3_BIT | LED4_BIT);
 
                                      // initialize the QS software tracing...
    Q_ALLEGE(QS_INIT(static_cast<void *>(0)));*/
    QS_RESET();
    QS_OBJ_DICTIONARY(&l_SysTick_Handler);
}
//............................................................................
void BSP_terminate(int16_t const result) {
    (void)result;
}
//............................................................................
void BSP_ledOn(void) {
    //LED_PORT->FIOSET = LED1_BIT;
}
//............................................................................
void BSP_ledOff(void) {
   // LED_PORT->FIOCLR = LED1_BIT;
}

//............................................................................
extern "C" void Q_onAssert(char_t const * const file, int_t const line) {
    (void)file;                                      // avoid compiler warning
    (void)line;                                      // avoid compiler warning
    QF_INT_DISABLE();            // make sure that all interrupts are disabled
                                                          // light up all LEDs
    //LED_PORT->FIOSET = (LED1_BIT | LED2_BIT | LED3_BIT | LED4_BIT);

    for (;;) {          // NOTE: replace the loop with reset for final version
    }
}

//////////////////////////////////////////////////////////////////////////////
namespace QP {

//............................................................................
void QF::onStartup(void) {
                 // set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate
    //(void)SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

                          // set priorities of all interrupts in the system...
    //NVIC_SetPriority(SysTick_IRQn, SYSTICK_PRIO);
    //NVIC_SetPriority(EINT0_IRQn,   GPIOPORTA_PRIO);

    //NVIC_EnableIRQ(EINT0_IRQn);
}
//............................................................................
void QF::onCleanup(void) {
}
//............................................................................
#ifdef QK_PREEMPTIVE

void QK::onIdle(void) {

    QF_INT_DISABLE();
    //LED_PORT->FIOSET = LED4_BIT;                           // turn the LED4 on
    //__NOP();                        // delay a bit to see some light intensity
    //__NOP();
    //__NOP();
    //__NOP();
    //LED_PORT->FIOCLR = LED4_BIT;                          // turn the LED4 off
    QF_INT_ENABLE();

#ifdef Q_SPY
    if (DPP::l_qspy.writeable()) {
    
        QF_INT_DISABLE();
        uint16_t b = QS::getByte();
        QF_INT_ENABLE();

        if (b != QS_EOD) {
            DPP::l_qspy.putc((uint8_t)b);
        }
    }
#else    
    // Put the CPU and peripherals to the low-power mode. You might need to
    // customize the clock management for your application, see the datasheet
    // for your particular Cortex-M3 MCU.
    //
    // Specifially for the mbed board, see the articles:
    // * "Power Management" http://mbed.org/cookbook/Power-Management; and
    // * "Interface Powerdown" at 
    // http://mbed.org/users/simon/notebook/interface-powerdown/
    // 
//    __WFI();
#endif
}

#else                                         // non-preemptive Vanilla kernel

void QF::onIdle(void) {               // NOTE: called with interrupts DISABLED

    //LED_PORT->FIOSET = LED4_BIT;                           // turn the LED4 on
    //__NOP();                        // delay a bit to see some light intensity
    //__NOP();
    //__NOP();
    //__NOP();
    //LED_PORT->FIOCLR = LED4_BIT;                          // turn the LED4 off

#ifdef Q_SPY
    QF_INT_ENABLE();
    if (DPP::l_qspy.writeable()) {
    
        QF_INT_DISABLE();
        uint16_t b = QS::getByte();
        QF_INT_ENABLE();

        if (b != QS_EOD) {
            DPP::l_qspy.putc((uint8_t)b);
        }
    }
#else    
    // Put the CPU and peripherals to the low-power mode. You might need to
    // customize the clock management for your application, see the datasheet
    // for your particular Cortex-M3 MCU.
    //
    // Specifially for the mbed board, see the articles:
    // * "Power Management" http://mbed.org/cookbook/Power-Management; and
    // * "Interface Powerdown" at 
    //    http://mbed.org/users/simon/notebook/interface-powerdown/
    // 
    __WFI();
    QF_INT_ENABLE();
#endif
}

#endif                                                        // QK_PREEMPTIVE

//----------------------------------------------------------------------------
#ifdef Q_SPY
//............................................................................
bool QS::onStartup(void const *arg) {
    static uint8_t qsBuf[6*256];                     // buffer for Quantum Spy
    initBuf(qsBuf, sizeof(qsBuf));
    
    DPP::l_qspy.baud(QSPY_BAUD_RATE);
    
    DPP::l_tickPeriod = SystemCoreClock / DPP::BSP_TICKS_PER_SEC;
    DPP::l_tickTime   = DPP::l_tickPeriod;   // to start the timestamp at zero

                                                    // setup the QS filters...
    QS_FILTER_ON(QS_ALL_RECORDS);

//    QS_FILTER_OFF(QS_QEP_STATE_EMPTY);
//    QS_FILTER_OFF(QS_QEP_STATE_ENTRY);
//    QS_FILTER_OFF(QS_QEP_STATE_EXIT);
//    QS_FILTER_OFF(QS_QEP_STATE_INIT);
//    QS_FILTER_OFF(QS_QEP_INIT_TRAN);
//    QS_FILTER_OFF(QS_QEP_INTERN_TRAN);
//    QS_FILTER_OFF(QS_QEP_TRAN);
//    QS_FILTER_OFF(QS_QEP_IGNORED);

//    QS_FILTER_OFF(QS_QF_ACTIVE_ADD);
//    QS_FILTER_OFF(QS_QF_ACTIVE_REMOVE);
//    QS_FILTER_OFF(QS_QF_ACTIVE_SUBSCRIBE);
//    QS_FILTER_OFF(QS_QF_ACTIVE_UNSUBSCRIBE);
//    QS_FILTER_OFF(QS_QF_ACTIVE_POST_FIFO);
//    QS_FILTER_OFF(QS_QF_ACTIVE_POST_LIFO);
//    QS_FILTER_OFF(QS_QF_ACTIVE_GET);
//    QS_FILTER_OFF(QS_QF_ACTIVE_GET_LAST);
//    QS_FILTER_OFF(QS_QF_EQUEUE_INIT);
//    QS_FILTER_OFF(QS_QF_EQUEUE_POST_FIFO);
//    QS_FILTER_OFF(QS_QF_EQUEUE_POST_LIFO);
//    QS_FILTER_OFF(QS_QF_EQUEUE_GET);
//    QS_FILTER_OFF(QS_QF_EQUEUE_GET_LAST);
//    QS_FILTER_OFF(QS_QF_MPOOL_INIT);
//    QS_FILTER_OFF(QS_QF_MPOOL_GET);
//    QS_FILTER_OFF(QS_QF_MPOOL_PUT);
//    QS_FILTER_OFF(QS_QF_PUBLISH);
//    QS_FILTER_OFF(QS_QF_NEW);
//    QS_FILTER_OFF(QS_QF_GC_ATTEMPT);
//    QS_FILTER_OFF(QS_QF_GC);
//    QS_FILTER_OFF(QS_QF_TICK);
//    QS_FILTER_OFF(QS_QF_TIMEEVT_ARM);
//    QS_FILTER_OFF(QS_QF_TIMEEVT_AUTO_DISARM);
//    QS_FILTER_OFF(QS_QF_TIMEEVT_DISARM_ATTEMPT);
//    QS_FILTER_OFF(QS_QF_TIMEEVT_DISARM);
//    QS_FILTER_OFF(QS_QF_TIMEEVT_REARM);
//    QS_FILTER_OFF(QS_QF_TIMEEVT_POST);
    QS_FILTER_OFF(QS_QF_CRIT_ENTRY);
    QS_FILTER_OFF(QS_QF_CRIT_EXIT);
    QS_FILTER_OFF(QS_QF_ISR_ENTRY);
    QS_FILTER_OFF(QS_QF_ISR_EXIT);

    return true;                                             // return success
}
//............................................................................
void QS::onCleanup(void) {
}
//............................................................................
QSTimeCtr QS::onGetTime(void) {              // invoked with interrupts locked
    if ((SysTick->CTRL & 0x00000100U) == 0U) {            // COUNTFLAG no set?
        return DPP::l_tickTime - (QSTimeCtr)SysTick->VAL;
    }
    else {        // the rollover occured, but the SysTick_ISR did not run yet
        return DPP::l_tickTime + DPP::l_tickPeriod - (QSTimeCtr)SysTick->VAL;
    }
}
//............................................................................
void QS::onFlush(void) {
    uint16_t b;
    QF_INT_DISABLE();
    while ((b = QS::getByte()) != QS_EOD) {
        while (!DPP::l_qspy.writeable()) {    // wait until serial is writable
        }
        DPP::l_qspy.putc((uint8_t)b);
    }
    QF_INT_ENABLE();
}
#endif                                                                // Q_SPY
//----------------------------------------------------------------------------

}                                                              // namespace QP
