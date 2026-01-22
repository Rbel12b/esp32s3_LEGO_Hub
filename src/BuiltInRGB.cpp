#include "BuiltInRGB.h"
#include "Board.h"
#include "Arduino.h"

void BuitlInRGB_init()
{
    ledcSetup(HUB_LED_RGB_CAHNNEL_R, 1000U, 8);
    ledcAttachPin(HUB_LED_RGB_PIN_R, HUB_LED_RGB_CAHNNEL_R);
    ledcSetup(HUB_LED_RGB_CAHNNEL_G, 1000U, 8);
    ledcAttachPin(HUB_LED_RGB_PIN_G, HUB_LED_RGB_CAHNNEL_G);
    ledcSetup(HUB_LED_RGB_CAHNNEL_B, 1000U, 8);
    ledcAttachPin(HUB_LED_RGB_PIN_B, HUB_LED_RGB_CAHNNEL_B);
}

void BuitlInRGB_setColor(uint8_t r, uint8_t g, uint8_t b)
{
    ledcWrite(HUB_LED_RGB_CAHNNEL_R, r);
    ledcWrite(HUB_LED_RGB_CAHNNEL_G, g);
    ledcWrite(HUB_LED_RGB_CAHNNEL_B, b);
}
