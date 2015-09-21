// #define ENABLE_WATCHDOG


#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

#include "pin_assign.h"

/* Variable placement in nonvolatile memory; linker puts this in right place */
#define __fram __attribute__((section(".fram_vars")))

// TODO: the interval calculation doesn't add up for some reason
#define WATCHDOG_INTERVAL_BITS WDTIS__8192K // by trial and error: yields a few seconds

// Sentinel value that indicates where non-volatile state was initialized
#define NV_STATE_MAGIC 0xdeadbeef

volatile __fram uint32_t nv_state_magic;
volatile __fram unsigned nv_iter;

volatile unsigned work_x;

static void burn(uint32_t iters)
{
    uint32_t iter = iters;
    while (iter--)
        work_x++;
}

static void init_hw()
{
#ifndef ENABLE_WATCHDOG
    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer
#endif

    PM5CTL0 &= ~LOCKLPM5;

    P1OUT = 0x00;
    P2OUT = 0x00;
    P3OUT = 0x00;
    P4OUT = 0x00;
    PJOUT = 0x00;
    P1DIR = BIT(PIN_AUX_3);
    PJDIR = BIT(PIN_LED2);
    P2DIR = 0x00;
    P3DIR = BIT(PIN_AUX_1) | BIT(PIN_AUX_2);
    P4DIR = BIT(PIN_LED1);

    // set clock speed to 4 MHz
    CSCTL0_H = 0xA5;
    CSCTL1 = DCOFSEL0 | DCOFSEL1;
    CSCTL2 = SELA_0 | SELS_3 | SELM_3;
    CSCTL3 = DIVA_0 | DIVS_0 | DIVM_0;

#ifdef ENABLE_WATCHDOG
    WDTCTL = WDTPW | WATCHDOG_INTERVAL_BITS | WDTCNTCL;
#endif
}

static void init_nv_state()
{
    if (nv_state_magic != NV_STATE_MAGIC) {
        nv_state_magic = NV_STATE_MAGIC;
        nv_iter = 0;
    }
}

int main() {
    init_hw();

    init_nv_state();

    while(1) {
#ifdef ENABLE_WATCHDOG
        WDTCTL = WDTPW | WATCHDOG_INTERVAL_BITS | WDTCNTCL;  // poke watchdog
#endif

        GPIO(PORT_LED1, OUT) ^= BIT(PIN_LED1);
        burn(50000);

        GPIO(PORT_LED2, OUT) ^= BIT(PIN_LED2);
        burn(50000);

        ++nv_iter;
    }

    return 0;
}
