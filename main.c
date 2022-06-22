/************************************************************************************
* Implementering av struktar för in- och utenheter via struktar led samt button.
* I detta fall realiseras en tillståndsmaskin, där aktuellt tillstånd implementeras
* via två lysdioder och byte till nästa tillstånd sker på fallande flank på en
* tryckknapp (fallande flank). Nyckelordet restrict används på pekare där adressen
* som pekas på inte kommer pekas på av någon annan pekare. Genom att använda detta
* nyckelord kan kompilatorn optimera koden, så den inte behöver ladda om minnet på
* adressen, då innehållet inte kommer ändras förutom av restrict-pekaren.
*
* Kompilera koden och skapa en körbar fil döpt main via följande kommando:
* $ gcc main.c gpio.c led.c button.c -o main -Wall -l gpiod
*
* Kör sedan programmet via följande kommando:
* $ ./main
************************************************************************************/

/* Inkluderingsdirektiv: */
#include "led.h"
#include "button.h"

/************************************************************************************
* main: Ansluter två lysdioder till PIN 17 och 22 samt en tryckknapp till PIN 27,
*       som används för att realisera en tillståndsmaskin innehållande fyra
*       tillstånd, vars sanningstabell presenteras nedan (0 = släckt, 1 = tänd):
*
*       state   led1   led2
*         0      0      0
*         1      0      1
*         2      1      0
*         3      1      1
*
*       Eventdetektering sker på tryckknappens insignal, där fallande flank medför
*       uppdatering till nästa tillstånd (0 => 1 => 2 => 3 => 0 => 1 => 2 osv.).
************************************************************************************/
int main(void)
{
   struct led led1, led2;
   struct button button1;

   led_new(&led1, 17, "led1");
   led_new(&led2, 22, "led2");
   button_new(&button1, 27, "button1");

   uint8_t state = 0;

   while (1)
   {
      if (button_event_detected(&button1, GPIO_EVENT_FALLING_EDGE))
      {
         if (++state > 3) state = 0;
      }

      switch (state)
      {
         case 0:
         {
            led_off(&led1);
            led_off(&led2);
            break;
         }
         case 1:
         {
            led_off(&led1);
            led_on(&led2);
            break;
         }
         case 2:
         {
            led_on(&led1);
            led_off(&led2);
            break;
         }
         case 3:
         {
            led_on(&led1);
            led_on(&led2);
            break;
         }
         default:
         {
            led_off(&led1);
            led_off(&led2);
            state = 0;
            break;
         }
      }
   }

   return 0;
}
