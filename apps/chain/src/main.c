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

typedef struct {
    CHAN_FIELD(unsigned, duty_cycle);
} msg_duty_cycle;

TASK(0, task_init)
TASK(1, task_1)
TASK(2, task_2)
TASK(3, task_3)

CHANNEL(task_init, task_1, msg_blinks);
CHANNEL(task_init, task_3, msg_tick);
CHANNEL(task_1, task_2, msg_blinks);
CHANNEL(task_2, task_1, msg_blinks);
SELF_CHANNEL(task_3, msg_tick);
MULTICAST_CHANNEL(msg_duty_cycle, ch_duty_cycle, task_init, task_1, task_2);

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

static void blink_led1(unsigned blinks, unsigned duty_cycle) {
    unsigned i;

    for (i = 0; i < blinks; ++i) {
        GPIO(PORT_LED1, OUT) |= BIT(PIN_LED1);
        burn(BLINK_DURATION_ITERS * 2 * duty_cycle / 100);

        GPIO(PORT_LED1, OUT) &= ~BIT(PIN_LED1);
        burn(BLINK_DURATION_ITERS * 2 * (100 - duty_cycle) / 100);
    }
}

static void blink_led2(unsigned blinks, unsigned duty_cycle) {
    unsigned i;

    for (i = 0; i < blinks; ++i) {
        GPIO(PORT_LED2, OUT) |= BIT(PIN_LED2);
        burn(BLINK_DURATION_ITERS * 2 * duty_cycle / 100);

        GPIO(PORT_LED2, OUT) &= ~BIT(PIN_LED2);
        burn(BLINK_DURATION_ITERS * 2 * (100 - duty_cycle) / 100);
    }
}

void task_init()
{
    CHAN_OUT(blinks, NUM_BLINKS_PER_TASK, CH(task_init, task_1));
    CHAN_OUT(tick, 0, CH(task_init, task_3));
    CHAN_OUT(duty_cycle, 75, MC_OUT_CH(ch_duty_cycle, task_init, task_1, task_2));
    TRANSITION_TO(task_1);
}

void task_1()
{
    unsigned blinks;
    unsigned duty_cycle;

    // Solid flash signifying beginning of task
    GPIO(PORT_LED1, OUT) |= BIT(PIN_LED1);
    burn(TASK_START_DURATION_ITERS);
    GPIO(PORT_LED1, OUT) &= ~BIT(PIN_LED1);
    burn(TASK_START_DURATION_ITERS);

    blinks = *CHAN_IN(blinks, CH(task_init, task_1), CH(task_2, task_1));
    duty_cycle = *CHAN_IN1(duty_cycle, MC_IN_CH(ch_duty_cycle, task_init, task_1));

    blink_led1(blinks, duty_cycle);
    blinks++;

    CHAN_OUT(blinks, blinks, CH(task_1, task_2));

    TRANSITION_TO(task_2);
}

void task_2()
{
    unsigned blinks;
    unsigned duty_cycle;

    // Solid flash signifying beginning of task
    GPIO(PORT_LED2, OUT) |= BIT(PIN_LED2);
    burn(TASK_START_DURATION_ITERS);
    GPIO(PORT_LED2, OUT) &= ~BIT(PIN_LED2);
    burn(TASK_START_DURATION_ITERS);

    blinks = *CHAN_IN1(blinks, CH(task_1, task_2));
    duty_cycle = *CHAN_IN1(duty_cycle, MC_IN_CH(ch_duty_cycle, task_init, task_2));

    blink_led2(blinks, duty_cycle);
    blinks++;

    CHAN_OUT(blinks, blinks, CH(task_2, task_1));

    TRANSITION_TO(task_3);
}

void task_3()
{
    unsigned wait_tick = *CHAN_IN(tick, CH(task_init, task_3), SELF_IN_CH(task_3));

    GPIO(PORT_LED1, OUT) |= BIT(PIN_LED1);
    GPIO(PORT_LED2, OUT) |= BIT(PIN_LED2);
    burn(WAIT_TICK_DURATION_ITERS);
    GPIO(PORT_LED1, OUT) &= ~BIT(PIN_LED1);
    GPIO(PORT_LED2, OUT) &= ~BIT(PIN_LED2);
    burn(WAIT_TICK_DURATION_ITERS);

    if (++wait_tick < WAIT_TICKS) {
        CHAN_OUT(tick, wait_tick, SELF_OUT_CH(task_3));
        TRANSITION_TO(task_3);
    } else {
        CHAN_OUT(tick, 0, SELF_OUT_CH(task_3));
        TRANSITION_TO(task_1);
    }
}

ENTRY_TASK(task_init)
INIT_FUNC(init)
