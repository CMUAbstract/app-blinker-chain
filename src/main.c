#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <libwispbase/wisp-base.h>
#include <libchain/chain.h>
#include <libio/log.h>

#ifdef CONFIG_LIBEDB_PRINTF
#include <libedb/edb.h>
#endif

#include "pin_assign.h"

#define INIT_TASK_DURATION_ITERS  400000
#define TASK_START_DURATION_ITERS 1600000
#define BLINK_DURATION_ITERS      400000
#define WAIT_TICK_DURATION_ITERS  300000
#define NUM_BLINKS_PER_TASK       5
#define WAIT_TICKS                3

// If you link-in wisp-base, then you have to define some symbols.
uint8_t usrBank[USRBANK_SIZE];

struct msg_blinks {
    CHAN_FIELD(unsigned, blinks);
};

struct msg_tick {
    CHAN_FIELD(unsigned, tick);
};

struct msg_self_tick {
    SELF_CHAN_FIELD(unsigned, tick);
};
#define FIELD_INIT_msg_self_tick { \
    SELF_FIELD_INITIALIZER \
}

struct msg_duty_cycle {
    CHAN_FIELD(unsigned, duty_cycle);
};

TASK(1, task_init)
TASK(2, task_1)
TASK(3, task_2)
TASK(4, task_3)

CHANNEL(task_init, task_1, msg_blinks);
CHANNEL(task_init, task_3, msg_tick);
CHANNEL(task_1, task_2, msg_blinks);
CHANNEL(task_2, task_1, msg_blinks);
SELF_CHANNEL(task_3, msg_self_tick);
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
    WISP_init();

    GPIO(PORT_LED_1, DIR) |= BIT(PIN_LED_1);
    GPIO(PORT_LED_2, DIR) |= BIT(PIN_LED_2);
#if defined(PORT_LED_3)
    GPIO(PORT_LED_3, DIR) |= BIT(PIN_LED_3);
#endif

    INIT_CONSOLE();

    __enable_interrupt();

#if defined(PORT_LED_3) // when available, this LED indicates power-on
    GPIO(PORT_LED_3, OUT) |= BIT(PIN_LED_3);
#endif

    LOG("chain app booted\r\n");
}

static void blink_led1(unsigned blinks, unsigned duty_cycle) {
    unsigned i;

    for (i = 0; i < blinks; ++i) {
        GPIO(PORT_LED_1, OUT) |= BIT(PIN_LED_1);
        burn(BLINK_DURATION_ITERS * 2 * duty_cycle / 100);

        GPIO(PORT_LED_1, OUT) &= ~BIT(PIN_LED_1);
        burn(BLINK_DURATION_ITERS * 2 * (100 - duty_cycle) / 100);
    }
}

static void blink_led2(unsigned blinks, unsigned duty_cycle) {
    unsigned i;

    for (i = 0; i < blinks; ++i) {
        GPIO(PORT_LED_2, OUT) |= BIT(PIN_LED_2);
        burn(BLINK_DURATION_ITERS * 2 * duty_cycle / 100);

        GPIO(PORT_LED_2, OUT) &= ~BIT(PIN_LED_2);
        burn(BLINK_DURATION_ITERS * 2 * (100 - duty_cycle) / 100);
    }
}

void task_init()
{
    task_prologue();

    LOG("init\r\n");

    // Solid flash signifying beginning of task
    GPIO(PORT_LED_1, OUT) |= BIT(PIN_LED_1);
    GPIO(PORT_LED_2, OUT) |= BIT(PIN_LED_2);
    burn(INIT_TASK_DURATION_ITERS);
    GPIO(PORT_LED_1, OUT) &= ~BIT(PIN_LED_1);
    GPIO(PORT_LED_2, OUT) &= ~BIT(PIN_LED_2);
    burn(INIT_TASK_DURATION_ITERS);

    unsigned blinks = NUM_BLINKS_PER_TASK;
    CHAN_OUT1(unsigned, blinks, blinks, CH(task_init, task_1));
    unsigned tick = 0;
    CHAN_OUT1(unsigned, tick, tick, CH(task_init, task_3));
    unsigned duty_cycle = 75;
    CHAN_OUT1(unsigned, duty_cycle, duty_cycle,
             MC_OUT_CH(ch_duty_cycle, task_init, task_1, task_2));

    TRANSITION_TO(task_3);
}

void task_1()
{
    task_prologue();

    unsigned blinks;
    unsigned duty_cycle;

    LOG("task 1\r\n");

    // Solid flash signifying beginning of task
    GPIO(PORT_LED_1, OUT) |= BIT(PIN_LED_1);
    burn(TASK_START_DURATION_ITERS);
    GPIO(PORT_LED_1, OUT) &= ~BIT(PIN_LED_1);
    burn(TASK_START_DURATION_ITERS);

    blinks = *CHAN_IN2(unsigned, blinks, CH(task_init, task_1), CH(task_2, task_1));
    duty_cycle = *CHAN_IN1(unsigned, duty_cycle,
                           MC_IN_CH(ch_duty_cycle, task_init, task_1));

    LOG("task 1: blinks %u dc %u\r\n", blinks, duty_cycle);

    blink_led1(blinks, duty_cycle);
    blinks++;

    CHAN_OUT1(unsigned, blinks, blinks, CH(task_1, task_2));

    TRANSITION_TO(task_2);
}

void task_2()
{
    task_prologue();

    unsigned blinks;
    unsigned duty_cycle;

    LOG("task 2\r\n");

    // Solid flash signifying beginning of task
    GPIO(PORT_LED_2, OUT) |= BIT(PIN_LED_2);
    burn(TASK_START_DURATION_ITERS);
    GPIO(PORT_LED_2, OUT) &= ~BIT(PIN_LED_2);
    burn(TASK_START_DURATION_ITERS);

    blinks = *CHAN_IN1(unsigned, blinks, CH(task_1, task_2));
    duty_cycle = *CHAN_IN1(unsigned, duty_cycle,
                           MC_IN_CH(ch_duty_cycle, task_init, task_2));

    LOG("task 2: blinks %u dc %u\r\n", blinks, duty_cycle);

    blink_led2(blinks, duty_cycle);
    blinks++;

    CHAN_OUT1(unsigned, blinks, blinks, CH(task_2, task_1));

    TRANSITION_TO(task_3);
}

void task_3()
{
    task_prologue();

    unsigned wait_tick = *CHAN_IN2(unsigned, tick, CH(task_init, task_3),
                                                   SELF_IN_CH(task_3));

    LOG("task 3: wait tick %u\r\n", wait_tick);

    GPIO(PORT_LED_1, OUT) |= BIT(PIN_LED_1);
    GPIO(PORT_LED_2, OUT) |= BIT(PIN_LED_2);
    burn(WAIT_TICK_DURATION_ITERS);
    GPIO(PORT_LED_1, OUT) &= ~BIT(PIN_LED_1);
    GPIO(PORT_LED_2, OUT) &= ~BIT(PIN_LED_2);
    burn(WAIT_TICK_DURATION_ITERS);

    if (++wait_tick < WAIT_TICKS) {
        CHAN_OUT1(unsigned, tick, wait_tick, SELF_OUT_CH(task_3));
        TRANSITION_TO(task_3);
    } else {
        unsigned tick = 0;
        CHAN_OUT1(unsigned, tick, tick, SELF_OUT_CH(task_3));
        TRANSITION_TO(task_1);
    }
}

ENTRY_TASK(task_init)
INIT_FUNC(init)
