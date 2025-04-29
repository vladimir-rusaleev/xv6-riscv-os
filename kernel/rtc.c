#include "types.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

static volatile uint32* rtc_low = (volatile uint32*) RTC_LOW;
static volatile uint32* rtc_high = (volatile uint32*) RTC_HIGH;

uint32 rtc_read_low(void)
{
    return *rtc_low;
}

uint32 rtc_read_high(void)
{
    return *rtc_high;
}

uint64 rtc_read_time(void)
{
    uint32 low = rtc_read_low();
    uint32 high = rtc_read_high();
    return ((uint64)high << 32) | low;
}