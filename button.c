/* Inkluderingsdirektiv: */
#include "button.h"

/************************************************************************************
* button_new: Konstruktor för strukten button, initierar tryckknapp.
************************************************************************************/
void button_new(struct button* restrict self, const uint8_t pin, const char* restrict alias)
{
   self->line = gpiod_line_new(pin, alias, GPIO_DIRECTION_IN);
   self->last_value = 0;
   return;
}

/************************************************************************************
* button_delete: Destruktor för strukten button, inaktiverar tryckknapp.
************************************************************************************/
void button_delete(struct button* self)
{
   gpiod_line_delete(&self->line);
   self->last_value = 0;
   return;
}

/************************************************************************************
* button_ptr_new: Returnerar pekare till en ny heapallokerad tryckknapp.
************************************************************************************/
struct button* button_ptr_new(const uint8_t pin, const char* restrict alias)
{
   struct button* self = (struct button*)malloc(sizeof(struct button));
   if (!self) return 0;
   self->line = gpiod_line_new(pin, alias, GPIO_DIRECTION_IN);
   self->last_value = 0;
   return self;
}

/************************************************************************************
* button_ptr_delete: Inaktiverar och frigör minne för heapallokerad tryckknapp.
************************************************************************************/
void button_ptr_delete(struct button** restrict self)
{
   button_delete(*self);
   free(*self);
   *self = 0;
   return;
}

/************************************************************************************
* button_pressed: Indikerar ifall tryckknapp är nedtryckt eller inte.
************************************************************************************/
bool button_pressed(struct button* restrict self)
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

/************************************************************************************
* button_event_detected: Indikerar ifall ett given event har ägt rum gällande
*                        insignalen på en tryckknapp.
************************************************************************************/
bool button_event_detected(struct button* restrict self, const enum gpio_event event)
{
   return gpiod_line_event_detected(self->line, event, &self->last_value);
}
