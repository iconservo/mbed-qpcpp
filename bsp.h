#ifndef bsp_h
#define bsp_h
                                                // System clock tick rate [Hz]
uint32_t const BSP_TICKS_PER_SEC = static_cast<uint32_t>(50);

void BSP_init(void);
void BSP_ledOn(void);
void BSP_ledOff(void);

#endif                                                                // bsp_h


