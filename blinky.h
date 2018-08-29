#ifndef blinky_h
#define blinky_h

enum BlinkySignals {
    TIMEOUT_SIG = QP::Q_USER_SIG,
    MAX_PUB_SIG,              // the last published signal

    MAX_SIG                   // the last signal
};

extern QP::QActive * const AO_Blinky;

#endif                        // blinky_h
