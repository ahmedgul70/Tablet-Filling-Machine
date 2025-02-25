#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

class display
{
  public:

    void begin()
    {
      lcd.begin();
      lcd.backlight();
      lcd.clear();
    }

    void wait()
    {
      lcd.setCursor(0, 0);
      lcd.print(" Tablet Filling ");    //   Connecting
      lcd.setCursor(0, 1);
      lcd.print("     System     ");
    }

    void home(int Qty, int Stage, int Type)
    {
      static uint32_t ptime;

      if (millis() - ptime > 1000)
      {
        lcd.setCursor(0, 0);
        lcd.print("Bottles Qty: ");
        lcd.print(Qty);
        lcd.print((Qty < 10) ? "  " : " ");
        lcd.setCursor(0, 1);
        lcd.print("Stage: ");
        lcd.print(Stage);
        lcd.print(" Type: ");
        lcd.print(Type);

        ptime = millis();
      }
    }
};
display display;
