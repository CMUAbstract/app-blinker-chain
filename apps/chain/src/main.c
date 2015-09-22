#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

#include <libchain/chain.h>

#include "pin_assign.h"

void task_1();
void task_2();

volatile unsigned work_x;

static void burn(uint32_t iters)
{
    uint32_t iter = iters;
    while (iter--)
        work_x++;
}

static void init_hw()
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

void task_1()
{
    GPIO(PORT_LED1, OUT) ^= BIT(PIN_LED1);
    burn(50000);
    transition_to(task_2);
}

void task_2()
{
    GPIO(PORT_LED2, OUT) ^= BIT(PIN_LED2);
    burn(50000);
    transition_to(task_1);
}

int main() {
    init_hw();
    transition_to(task_1);
    return 0; // TODO: write our own entry point and get rid of this
}
