#pragma once
#ifndef _BOARD_H_
#define _BOARD_H_

// Onboard RGB LED
#define HUB_LED_RGB_PIN_R 41 // red pin
#define HUB_LED_RGB_PIN_G 40 // blue pin
#define HUB_LED_RGB_PIN_B 42 // green pin

// Port pwm pins
#define PORT_A_PWM_1 21
#define PORT_A_PWM_2 10

#define PORT_B_PWM_1 4
#define PORT_B_PWM_2 5

#define PORT_C_PWM_1 48
#define PORT_C_PWM_2 47

#define PORT_D_PWM_1 6
#define PORT_D_PWM_2 7

// Port ID pins
#define PORT_A_ID_1 15
#define PORT_A_ID_2 16

#define PORT_B_ID_1 17
#define PORT_B_ID_2 18

#define PORT_C_ID_1 13
#define PORT_C_ID_2 12

#define PORT_D_ID_1 11
#define PORT_D_ID_2 14

// ledc channels for the onboard rgb led
#define HUB_LED_RGB_CAHNNEL_R 0
#define HUB_LED_RGB_CAHNNEL_G 1
#define HUB_LED_RGB_CAHNNEL_B 2

// ledc channels for the ports
#include "driver/mcpwm.h"

#define PORT_A_PWM_UNIT mcpwm_unit_t(0)
#define PORT_A_PWM_TIMER mcpwm_timer_t(0)

#define PORT_B_PWM_UNIT mcpwm_unit_t(0)
#define PORT_B_PWM_TIMER mcpwm_timer_t(1)

#define PORT_C_PWM_UNIT mcpwm_unit_t(0)
#define PORT_C_PWM_TIMER mcpwm_timer_t(3)

#define PORT_D_PWM_UNIT mcpwm_unit_t(1)
#define PORT_D_PWM_TIMER mcpwm_timer_t(0)

// Port Hardware serial numbers
#define PORT_A_HWS 1
#define PORT_B_HWS 2
#define PORT_C_HWS 0
#define PORT_D_HWS -1 // software

#endif