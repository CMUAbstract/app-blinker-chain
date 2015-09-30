#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

#include <libchain/chain.h>

#include "pin_assign.h"

#define TASK_START_DURATION_ITERS 200000
#define BLINK_DURATION_ITERS      100000
#define WAIT_TICK_DURATION_ITERS  300000
#define NUM_BLINKS_PER_TASK       5
#define WAIT_TICKS                3

typedef struct {
    CHAN_FIELD(unsigned, blinks);
} msg_blinks;

typedef struct {
    CHAN_FIELD(unsigned, tick);
} msg_tick;

CHANNEL(task_init, task_1, msg_blinks);
CHANNEL(task_1, task_2, msg_blinks);
CHANNEL(task_2, task_1, msg_blinks);
SELF_CHANNEL(task_3, msg_tick);

void task_1();
void task_2();
void task_3();

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
    CHAN_OUT(blinks, NUM_BLINKS_PER_TASK, CH(task_init, task_1));
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

    blinks = *CHAN_IN(blinks, CH(task_init, task_1), CH(task_2, task_1));

    for (i = 0; i < blinks * 2; i++) {
        GPIO(PORT_LED1, OUT) ^= BIT(PIN_LED1);
        burn(BLINK_DURATION_ITERS);
    }

    blinks++;

    CHAN_OUT(blinks, blinks, CH(task_1, task_2));

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

    blinks = *CHAN_IN1(blinks, CH(task_1, task_2));

    for (i = 0; i < blinks * 2; i++) {
        GPIO(PORT_LED2, OUT) ^= BIT(PIN_LED2);
        burn(BLINK_DURATION_ITERS);
    }

    blinks++;

    CHAN_OUT(blinks, blinks, CH(task_2, task_1));

    transition_to(task_3);
}

void task_3()
{
    unsigned wait_tick = *CHAN_IN1(tick, SELF_CH(task_3));

    GPIO(PORT_LED1, OUT) |= BIT(PIN_LED1);
    GPIO(PORT_LED2, OUT) |= BIT(PIN_LED2);
    burn(WAIT_TICK_DURATION_ITERS);
    GPIO(PORT_LED1, OUT) &= ~BIT(PIN_LED1);
    GPIO(PORT_LED2, OUT) &= ~BIT(PIN_LED2);
    burn(WAIT_TICK_DURATION_ITERS);

    if (++wait_tick < WAIT_TICKS) {
        CHAN_OUT(tick, wait_tick, SELF_CH(task_3));
        transition_to(task_3);
    } else {
        CHAN_OUT(tick, 0, SELF_CH(task_3));
        transition_to(task_1);
    }
}

ENTRY_TASK(task_init)
INIT_FUNC(init)
