#include "systick.h"

static void(*systick_cbfn)(void) = 0;

// Implement a 10 Hz SysTick timer event
void systick_init(void(*cbfn)(void)) {
    // register the callback function
    systick_cbfn = cbfn;

    // Trying to configure a 10 Hz SysTick event or 100 ms period.
    // systick uses the external clock source: HCLK / 8.
    //
    // System clock = 48 MHz
    // systick clock = 48 MHz / 8 = 6 MHz
    //
    // wanted systick period = 100 ms = 0.1 seconds
    // timer ticks = 6000000 × 0.1 = 600000
    //
    // The systick RVR stores (N - 1),
    // where N is the number of clock ticks before the counter reaches zero.
    // This is specified in the STM32F042K6 Programming Manual (Section 4.4.2).
    SYSTICK->RVR = 600000-1;
    // Switch to the "external clock source" (HCLK/8 = 6 MHz), 
    // enable the counter,
    // and enable an exception request when the counter reaches 0
    SYSTICK->CSR = (SYSTICK_CSR_ENABLE | SYSTICK_CSR_TICKINT);
}

void SysTick_Handler(void) {
    if( systick_cbfn) 
        systick_cbfn();
}