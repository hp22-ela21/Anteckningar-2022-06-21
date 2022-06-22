/* Inkluderingsdirektiv: */
#include "gpio.h"

/************************************************************************************
* gpiod_line_new: Initierar ny GPIO-linjepekare. Ingående argument utgörs av
*                 PIN-nummer, datariktning och alias. Efter att GPIO-linjepekaren
*                 har initieras så returneras pekaren.
************************************************************************************/
struct gpiod_line* gpiod_line_new(const uint8_t pin, const char* restrict alias,
                                  const enum gpio_direction direction)
{
   static struct gpiod_chip* chip0 = 0;
   if (!chip0) chip0 = gpiod_chip_open("/dev/gpiochip0");

   struct gpiod_line* self = gpiod_chip_get_line(chip0, pin);

   if (direction == GPIO_DIRECTION_OUT)
   {
      gpiod_line_request_output(self, alias, 0);
   }
   else
   {
      gpiod_line_request_input(self, alias);
   }

   return self;
}

/************************************************************************************
* gpiod_line_delete: Tar bort reservation av given GPIO-linje och sätter
*                    motsvarande linjepekare till null.
************************************************************************************/
void gpiod_line_delete(struct gpiod_line** restrict self)
{
   gpiod_line_release(*self);
   *self = 0;
   return;
}

/************************************************************************************
* gpiod_line_toggle: Togglar utsignal på given GPIO-linje.
************************************************************************************/
void gpiod_line_toggle(struct gpiod_line* restrict self)
{
   if (gpiod_line_get_value(self) == 1)
   {
      gpiod_line_set_value(self, 0);
   }
   else
   {
      gpiod_line_set_value(self, 1);
   }
   return;
}

/************************************************************************************
* gpiod_line_blink: Blinkar utsignal på given GPIO-linje med valbar fördröjningstid.
************************************************************************************/
void gpiod_line_blink(struct gpiod_line* restrict self, const size_t delay_time)
{
   gpiod_line_toggle(self);
   gpio_delay(delay_time);
   return;
}

/************************************************************************************
* gpiod_line_event_detected: Indikerar ifall ett specifikt event har ägt rum på
*                            given GPIO-linje och uppdaterar senast lästa insignal.
*                            Ifall ett event har ägt rum så genereras en fördröjning
*                            på 50 ms för att minska påverkan av kontaktstudsar.
************************************************************************************/
bool gpiod_line_event_detected(struct gpiod_line* restrict self, const enum gpio_event event,
                               uint8_t* restrict last_value)
{
   const uint8_t previous_value = *last_value;
   const uint8_t current_value = gpiod_line_get_value(self);
   *last_value = current_value;
   if (current_value == previous_value) return false;
   gpio_delay(50);

   if (event == GPIO_EVENT_RISING_EDGE && (current_value && !previous_value))
   {
      return true;
   }
   else if (event == GPIO_EVENT_FALLING_EDGE && (!current_value && previous_value))
   {
      return true;
   }
   else if (event == GPIO_EVENT_BOTH_EDGES && (current_value != previous_value))
   {
      return true;
   }
   else
   {
      return false;
   }
}

/************************************************************************************
* gpio_delay: Genererar fördröjning mätt i millisekunder.
************************************************************************************/
void gpio_delay(const size_t delay_time)
{
   usleep(delay_time * 1000);
   return;
}
