#pragma once
#ifndef _BOARD_H_
#define _BOARD_H_

// Onboard RGB LED
#define HUB_LED_PIN 40

// Port pwm pins
#define PORT_A_PWM_1 21
#define PORT_A_PWM_2 10

#define PORT_B_PWM_1 7
#define PORT_B_PWM_2 6

#define PORT_C_PWM_1 48
#define PORT_C_PWM_2 47

#define PORT_D_PWM_1 4
#define PORT_D_PWM_2 5

// Port ID pins
#define PORT_A_ID_1 15
#define PORT_A_ID_2 16

#define PORT_B_ID_1 17
#define PORT_B_ID_2 18

#define PORT_C_ID_1 13
#define PORT_C_ID_2 12

#define PORT_D_ID_1 11
#define PORT_D_ID_2 14

#define PORT_A_PWM_UNIT mcpwm_unit_t(0)
#define PORT_A_PWM_TIMER mcpwm_timer_t(0)

#define PORT_B_PWM_UNIT mcpwm_unit_t(0)
#define PORT_B_PWM_TIMER mcpwm_timer_t(1)

#define PORT_C_PWM_UNIT mcpwm_unit_t(0)
#define PORT_C_PWM_TIMER mcpwm_timer_t(2)

#define PORT_D_PWM_UNIT mcpwm_unit_t(1)
#define PORT_D_PWM_TIMER mcpwm_timer_t(0)

// Port Hardware serial numbers
#define PORT_A_HWS 1
#define PORT_B_HWS 2
#define PORT_C_HWS 0
#define PORT_D_HWS -1 // software

#define I2C_SDA 8
#define I2C_SCL 42
#define I2C_HW Wire1

#define BNO08X_INT -1
#define BNO08X_RST -1

// The SC16IS750's pin used to disable TX outputs when the port is in UART mode. Connected to the #OE pin of the TX output buffer.
#define PORT_D_TX_DISABLE 2

#endif