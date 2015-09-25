#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

#include <libchain/chain.h>

#include "pin_assign.h"

#define TASK_START_DURATION_ITERS 200000
#define BLINK_DURATION_ITERS      100000
#define NUM_BLINKS_PER_TASK       5

typedef struct {
    CHAN_FIELD(unsigned, blinks);
} msg_t;

CHANNEL(task_init, task_1, msg_t);
CHANNEL(task_1, task_2, msg_t);
CHANNEL(task_2, task_1, msg_t);

void task_1();
void task_2();

volatile unsigned work_x;

static void burn(uint32_t iters)
{
    uint32_t iter = iters;
    while (iter--)
        work_x++;
}

void init()
{
    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer

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
}

void task_init()
{
    CHAN_OUT(task_init, task_1, blinks, NUM_BLINKS_PER_TASK);
    transition_to(task_1);
}

void task_1()
{
    unsigned i;
    unsigned blinks;

    // Solid flash signifying beginning of task
    GPIO(PORT_LED1, OUT) |= BIT(PIN_LED1);
    burn(TASK_START_DURATION_ITERS);
    GPIO(PORT_LED1, OUT) &= ~BIT(PIN_LED1);
    burn(TASK_START_DURATION_ITERS);

    blinks = *CHAN_IN2(blinks, task_1, task_init, task_2);

    for (i = 0; i < blinks * 2; i++) {
        GPIO(PORT_LED1, OUT) ^= BIT(PIN_LED1);
        burn(BLINK_DURATION_ITERS);
    }

    blinks++;

    CHAN_OUT(task_1, task_2, blinks, blinks);

    transition_to(task_2);
}

void task_2()
{
    unsigned i;
    unsigned blinks;

    // Solid flash signifying beginning of task
    GPIO(PORT_LED2, OUT) |= BIT(PIN_LED2);
    burn(TASK_START_DURATION_ITERS);
    GPIO(PORT_LED2, OUT) &= ~BIT(PIN_LED2);
    burn(TASK_START_DURATION_ITERS);

    blinks = *CHAN_IN1(blinks, task_2, task_1);

    for (i = 0; i < blinks * 2; i++) {
        GPIO(PORT_LED2, OUT) ^= BIT(PIN_LED2);
        burn(BLINK_DURATION_ITERS);
    }

    blinks++;

    CHAN_OUT(task_2, task_1, blinks, blinks);

    transition_to(task_1);
}

ENTRY_TASK(task_init)
INIT_FUNC(init)
