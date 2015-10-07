#ifndef PIN_ASSIGN_H
#define PIN_ASSIGN_H

// Ugly workaround to make the pretty GPIO macro work for OUT register
// (a control bit for TAxCCTLx uses the name 'OUT')
#undef OUT

// #define BOARD_WISP
#define BOARD_MSP_TS430

#define BIT_INNER(idx) BIT ## idx
#define BIT(idx) BIT_INNER(idx)

#define GPIO_INNER(port, reg) P ## port ## reg
#define GPIO(port, reg) GPIO_INNER(port, reg)

#if defined(BOARD_WISP)
#define     PORT_LED1           4
#define     PIN_LED1            0
#define     PORT_LED2           J
#define     PIN_LED2            6

#define     PORT_AUX            3
#define        PIN_AUX_1            4
#define        PIN_AUX_2            5


#define        PORT_AUX3            1
#define        PIN_AUX_3            4

#elif defined(BOARD_MSP_TS430)

#define     PORT_LED1           1
#define     PIN_LED1            1
#define     PORT_LED2           1
#define     PIN_LED2            2
#define     PORT_LED3           1
#define     PIN_LED3            0

#define     PORT_AUX            3
#define     PIN_AUX_1           4
#define     PIN_AUX_2           5


#define     PORT_AUX3           1
#define     PIN_AUX_3           4

#endif // BOARD_*

#endif
