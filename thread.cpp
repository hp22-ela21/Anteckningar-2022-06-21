/*****************************************************************************************
* Implementering av flertr�dning f�r att kunna k�ra tv� processer f�r styrning av tre 
* lysdioder anslutna till PIN 17, 22 och 23, vars alias s�tts till led1 - led3. 
* Tv� tryckknappar anslutna till PIN 24 och 27, vars alias s�tts till button1 - button2,
* anv�nds f�r att toggla lysdioderna mellan att vara sl�ckta och blinka. Vid nedtryckning 
* button1 toggla led1 mellan att vara sl�ckt samt blinka var 500:e millisekund. Annars 
* om button2 trycks ned s� togglas lysdioderna led2 - led3  mellan att vara sl�ckta 
* samt blinka var 100:e ms. Via en tredje process kan respektive upps�ttning lysdioder
* togglas fr�n tangentbordet genom att mata in "led1" samt "led2". Inmatning av dessa
* alias har d�rmed sammt funktion som respektive tryckknapp.
*****************************************************************************************/

// Inkluderingsdirektiv:
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <gpiod.h>
#include <unistd.h>

// Enumerationsklasser:
enum class gpio_direction { in, out };
enum class gpio_event { rising_edge, falling_edge, both_edges };

/*****************************************************************************************
* gpiod_line_new: Returnerar initierad GPIO-linjepekare.
*****************************************************************************************/
static gpiod_line* gpiod_line_new(const std::uint8_t pin, const gpio_direction direction, const char* alias = nullptr)
{
   static gpiod_chip* chip0 = nullptr;
   if (!chip0) chip0 = gpiod_chip_open("/dev/gpiochip0");
   gpiod_line* self = gpiod_chip_get_line(chip0, pin);

   if (direction == gpio_direction::out)
   {
      gpiod_line_request_output(self, alias, 0);
   }
   else
   {
      gpiod_line_request_input(self, alias);
   }

   return self;
}

/*****************************************************************************************
* gpiod_line_delete: Frig�r aktuell GPIO-linje och s�tter tillh�rande pekare till null.
*****************************************************************************************/
static void gpiod_line_delete(gpiod_line** self)
{
   gpiod_line_release(*self);
   *self = nullptr;
   return;
}

/*****************************************************************************************
* delay: Genererar f�rdr�jning m�tt i millisekunder.
*****************************************************************************************/
static void delay(const std::size_t delay_time)
{
   usleep(delay_time * 1000);
   return;
}

/*****************************************************************************************
* gpiod_line_event_detected: Indikerar event p� en given GPIO-linje och uppdaterar det 
*                   senaste avl�sta v�rdet. Vid event genereras en f�rdr�jning p� 50 ms 
*                   f�r att minska p�verkan av kontaktstudsar.
*****************************************************************************************/
static bool gpiod_line_event_detected(gpiod_line* self, const gpio_event event, std::uint8_t& last_value)
{
   const auto previous_value = last_value;
   const auto current_value = gpiod_line_get_value(self);
   last_value = current_value;
   if (current_value == previous_value) return false;
   delay(50);

   if (event == gpio_event::rising_edge && (current_value && !previous_value))
   {
      return true;
   }
   else if (event == gpio_event::falling_edge && (!current_value && previous_value))
   {
      return true;
   }
   else if (event == gpio_event::both_edges && (current_value != previous_value))
   {
      return true;
   }
   else
   {
      return false;
   }
}

/*****************************************************************************************
* gpiod_lines_blink: Blinkar lysdioder lagrade i en vektor med valbar blinkhastighet.
*****************************************************************************************/
static void gpiod_lines_blink(const std::vector<gpiod_line*>& leds, const std::size_t delay_time)
{
   for (auto& i : leds)
   {
      gpiod_line_set_value(i, 1);
      delay(delay_time);
      gpiod_line_set_value(i, 0);
   }

   if (leds.size() == 1)
   {
      delay(delay_time);
   }
   return;
}

/*****************************************************************************************
* led_process: Separat process f�r styrning av lysdioder via en tryckknapp samt en
*              enable-signal, som ocks� kan uppdateras externt.
*****************************************************************************************/
static void led_process(const std::vector<gpiod_line*>& leds, gpiod_line* button, const std::size_t delay_time, bool& enabled)
{
   std::uint8_t last_value = 0;

   while (true)
   {
      if (gpiod_line_event_detected(button, gpio_event::rising_edge, last_value))
      {
         enabled = !enabled;
      }

      if (enabled)
      {
         gpiod_lines_blink(leds, delay_time);
      }
   }
   return;
}

/*****************************************************************************************
* keyboard_process: Separat process f�r styrning av lysdioder via tangentbordet. Genom
*                   att mata in alias p� tv� lysdioder kan dessa togglas, vilket 
*                   �stadkommes via en annan process.
*****************************************************************************************/
static void keyboard_process(bool& enabled1, bool& enabled2, const char* alias1, const char* alias2)
{
   std::cout << "Enter " << alias1 << " or " << alias2 << " to toggle the leds!\n\n";
   std::string s;

   while (true)
   {
      std::getline(std::cin, s);

      if (s == alias1)
      {
         enabled1 = !enabled1;
      }
      else
      {
         enabled2 = !enabled2;
      }
   }

   return;
}

/*****************************************************************************************
* main: Lagrar GPIO-linjepekare f�r lysdioder anslutna till PIN 17, 23 och 23 i tv� 
*       vektorer och deklarerar GPIO-linjepekare f�r tv� tryckknappar anslutna till 
*       PIN 24 och 27. Tv� enabled-signaler deklareras f�r anv�ndning i var sin process.
*       Tre tr�dar skapad f�r parallell styrning av lysdioder, b�de via tryckknapparna
*       samt via inmatning fr�n tangentbordet. Samtliga tr�dar exekverar kontinuerligt,
*       men synkroniseras �nd�, s� att om koden �ndras s� v�ntar varje avslutad tr�d in 
*       att �vriga tr�dar avslutas innan programmet fortg�r..
*****************************************************************************************/
int main(void)
{
   std::vector<gpiod_line*> leds1 =
   {
      gpiod_line_new(17, gpio_direction::out, "led1")
   };

   std::vector<gpiod_line*> leds2 =
   {
      gpiod_line_new(22, gpio_direction::out, "led2"),
      gpiod_line_new(23, gpio_direction::out, "led3")
   };

   auto button1 = gpiod_line_new(24, gpio_direction::in, "button1");
   auto button2 = gpiod_line_new(27, gpio_direction::in, "button2");
   auto enabled1 = false;
   auto enabled2 = false;

   std::thread t1(led_process, std::ref(leds1), button1, 500, std::ref(enabled1));
   std::thread t2(led_process, std::ref(leds2), button2, 100, std::ref(enabled2));
   std::thread t3(keyboard_process, std::ref(enabled1), std::ref(enabled2), 
                  gpiod_line_consumer(leds1[0]), gpiod_line_consumer(leds2[0]));
   t1.join();
   t2.join();
   t3.join();

   return 0;
}
