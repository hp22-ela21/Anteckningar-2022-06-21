/* Inkluderingsdirektiv: */
#include "led.h"

/************************************************************************************
* led_new: Konstruktor för strukten led, initierar ny lysdiod.
************************************************************************************/
void led_new(struct led* restrict self, const uint8_t pin, const char* restrict alias)
{
   self->line = gpiod_line_new(pin, alias, GPIO_DIRECTION_OUT);
   return;
}

/************************************************************************************
* led_delete: Destruktor för strukten led, inaktiverar given lysdiod.
************************************************************************************/
void led_delete(struct led* restrict self)
{
   gpiod_line_set_value(self->line, 0);
   gpiod_line_delete(&self->line);
   return;
}

/************************************************************************************
* led_ptr_new: Returnerar pekare till en ny heapallokerad lysdiod.
************************************************************************************/
struct led* led_ptr_new(const uint8_t pin, const char* restrict alias)
{
   struct led* self = (struct led*)malloc(sizeof(struct led));
   if (!self) return 0;
   self->line = gpiod_line_new(pin, alias, GPIO_DIRECTION_OUT);
   return self;
}

/************************************************************************************
* led_ptr_delete: Inaktiverar och frigör minne för heapallokerad lysdiod.
************************************************************************************/
void led_ptr_delete(struct led** restrict self)
{
   led_delete(*self);
   free(*self);
   *self = 0;
   return;
}

/************************************************************************************
* led_on: Tänder lysdiod.
************************************************************************************/
void led_on(struct led* restrict self)
{
   gpiod_line_set_value(self->line, 1);
   return;
}

/************************************************************************************
* led_off: Släcker lysdiod.
************************************************************************************/
void led_off(struct led* restrict self)
{
   gpiod_line_set_value(self->line, 0);
   return;
}

/************************************************************************************
* led_toggle: Togglar lysdiod.
************************************************************************************/
void led_toggle(struct led* restrict self)
{
   gpiod_line_toggle(self->line);
   return;
}

/************************************************************************************
* led_blink: Blinkar lysdiod med valbar fördröjningstid.
************************************************************************************/
void led_blink(struct led* restrict self, const size_t delay_time)
{
   gpiod_line_blink(self->line, delay_time);
   return;
}

/************************************************************************************
* led_enabled: Indikerar ifall given lysdiod är tänd eller inte.
************************************************************************************/
bool led_enabled(struct led* restrict self)
{
   if (gpiod_line_get_value(self->line) == 1)
   {
      return true;
   }
   else
   {
      return false;
   }
}
