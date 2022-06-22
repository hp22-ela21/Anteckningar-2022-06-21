#ifndef GPIO_H_
#define GPIO_H_

/* Inkluderingsdirektiv: */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <gpiod.h>
#include <unistd.h>

/* Enumerationer: */
enum gpio_direction { GPIO_DIRECTION_IN, GPIO_DIRECTION_OUT };
enum gpio_event { GPIO_EVENT_RISING_EDGE, GPIO_EVENT_FALLING_EDGE, GPIO_EVENT_BOTH_EDGES };

/* Externa funktioner: */
struct gpiod_line* gpiod_line_new(const uint8_t pin, const char* restrict alias,
                                  const enum gpio_direction direction);
void gpiod_line_delete(struct gpiod_line** restrict self);
void gpiod_line_toggle(struct gpiod_line* restrict self);
void gpiod_line_blink(struct gpiod_line* restrict self, const size_t delay_time);
bool gpiod_line_event_detected(struct gpiod_line* restrict self, const enum gpio_event event,
                               uint8_t* restrict last_value);
void gpio_delay(const size_t delay_time);

#endif /* GPIO_H_ */
