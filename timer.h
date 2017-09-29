#define BIT(n) (1 << n)

#define TIMER_ISTATUS BIT(2) // if timer is asserted, ie: reached value
#define TIMER_IMASK BIT(1)
#define TIMER_ENABLE BIT(0)

// CNTP_CTL
uint32_t getTimerCtl();

void setTimerCtl(uint32_t val);

// CNTP_TVAL, PL1 Physical TimerValue
// downcounter
void setTimerVal(uint32_t val);