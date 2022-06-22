#ifndef LED_H_
#define LED_H_

/* Inkluderingsdirektiv: */
#include "gpio.h"

/************************************************************************************
* led: Strukt för enkel implentering av lysdioder och andra digitala utenheter.
************************************************************************************/
struct led
{
   struct gpiod_line* line; /* Pekare till aktuell GPIO-linje. */
};

/* Externa funktioner: */
void led_new(struct led* restrict self, const uint8_t pin, const char* restrict alias);
void led_delete(struct led* restrict self);
struct led* led_ptr_new(const uint8_t pin, const char* restrict alias);
void led_ptr_delete(struct led** restrict self);
void led_on(struct led* restrict self);
void led_off(struct led* restrict self);
void led_toggle(struct led* restrict self);
void led_blink(struct led* restrict self, const size_t delay_time);
bool led_enabled(struct led* restrict self);

#endif /* LED_H_ */
