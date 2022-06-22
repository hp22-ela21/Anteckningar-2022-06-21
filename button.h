#ifndef BUTTON_H_
#define BUTTON_H_

/* Inkluderingsdirektiv: */
#include "gpio.h"

/************************************************************************************
* button: Strukt för enkel implentering av tryckknappar eller andra digital
*         inenheter där avläsning samt eventdetektering skall kunna genomföras.
************************************************************************************/
struct button
{
   struct gpiod_line* line; /* Pekare till aktuell GPIO-linje. */
   uint8_t last_value; /* Senast avlästa insignal. */
};

/* Externa funktioner: */
void button_new(struct button* restrict self, const uint8_t pin, const char* restrict alias);
void button_delete(struct button* self);
struct button* button_ptr_new(const uint8_t pin, const char* restrict alias);
void button_ptr_delete(struct button** restrict self);
bool button_pressed(struct button* restrict self);
bool button_event_detected(struct button* restrict self, const enum gpio_event event);

#endif /* BUTTON_H_ */
