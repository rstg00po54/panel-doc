
#include <TFT_eSPI.h>
#include <CST816S.h>

CST816S touch(21, 22, 5, 32); // sda, scl, rst, irq

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;
void setup()
{

  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);
  Serial.begin(115200);
  touch.begin();
  tft.begin();
  tft.setRotation(0);

  tft.fillScreen(TFT_BLACK);

  // make the color selection boxes
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, TFT_RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, TFT_YELLOW);
  tft.fillRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, TFT_GREEN);
  tft.fillRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, TFT_CYAN);
  tft.fillRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, TFT_BLUE);
  tft.fillRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, TFT_MAGENTA);

  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
  currentcolor = TFT_RED;
}

void loop()
{

  if (touch.available())
  {
    // Serial.print(touch.gesture());
    // Serial.print("\t");
    // Serial.print(touch.data.points);
    // Serial.print("\t");
    // Serial.print(touch.data.event);
    // Serial.print("\t");
    // Serial.print(touch.data.x);
    // Serial.print("\t");
    // Serial.println(touch.data.y);

    // Retrieve a point
   // TS_Point p = ctp.getPoint();

    /*
     // Print out raw data from screen touch controller
     Serial.print("X = "); Serial.print(p.x);
     Serial.print("\tY = "); Serial.print(p.y);
     Serial.print(" -> ");
    */

    // flip it around to match the screen.
  //  p.x = map(p.x, 0, 240, 240, 0);
  //  p.y = map(p.y, 0, 320, 320, 0);

    // Print out the remapped (rotated) coordinates

    
    Serial.print("(");
    Serial.print(touch.data.x);
    Serial.print(", ");
    Serial.print(touch.data.y);
    Serial.println(")");

    if (touch.data.y < BOXSIZE)
    {
      oldcolor = currentcolor;

      if (touch.data.x < BOXSIZE)
      {
        currentcolor = TFT_RED;
        tft.drawRect(0, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
      }
      else if (touch.data.x < BOXSIZE * 2)
      {
        currentcolor = TFT_YELLOW;
        tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
      }
      else if (touch.data.x < BOXSIZE * 3)
      {
        currentcolor = TFT_GREEN;
        tft.drawRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
      }
      else if (touch.data.x < BOXSIZE * 4)
      {
        currentcolor = TFT_CYAN;
        tft.drawRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
      }
      else if (touch.data.x < BOXSIZE * 5)
      {
        currentcolor = TFT_BLUE;
        tft.drawRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
      }
      else if (touch.data.x <= BOXSIZE * 6)
      {
        currentcolor = TFT_MAGENTA;
        tft.drawRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
      }

      if (oldcolor != currentcolor)
      {
        if (oldcolor == TFT_RED)
          tft.fillRect(0, 0, BOXSIZE, BOXSIZE, TFT_RED);
        if (oldcolor == TFT_YELLOW)
          tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, TFT_YELLOW);
        if (oldcolor == TFT_GREEN)
          tft.fillRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, TFT_GREEN);
        if (oldcolor == TFT_CYAN)
          tft.fillRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, TFT_CYAN);
        if (oldcolor == TFT_BLUE)
          tft.fillRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, TFT_BLUE);
        if (oldcolor == TFT_MAGENTA)
          tft.fillRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, TFT_MAGENTA);
      }
    }
    if (((touch.data.y - PENRADIUS) > BOXSIZE) && ((touch.data.y + PENRADIUS) < tft.height()))
    {
      tft.fillCircle(touch.data.x, touch.data.y, PENRADIUS, currentcolor);
    }
  }
}
