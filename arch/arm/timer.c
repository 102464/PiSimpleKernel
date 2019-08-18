#include "timer.h"
static arm_timer_t *armtimer = (arm_timer_t *)ARMTIMER_BASE;
arm_timer_t *GetTimer(void) { return armtimer; }