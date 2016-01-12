/**************************************************
   This is a MIDI controller program that controls
   multiple midi CC's on seperate MIDI channels using
   the ILI9341 TFT shield touchscreen.
*/


#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>
#include <Wire.h>      // this is needed even tho we aren't using it
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>
#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// The STMPE610 uses hardware SPI on the shield, and #8
#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
#define TS_MINX 450
#define TS_MINY 330
#define TS_MAXX 3550
#define TS_MAXY 3750
#define VERSION "MIDIStick 0.3.7"
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// Controls flow speed of MIDI messages
#define REDUCER 0
#define BAUD 115200

// Maximum level for 8-bit messages
#define MIDI_MAX 127


int buttonState = 0, controlState = 0, reducerCount = 0;
int pressureChangeCount = 0, pressureLearnCount = 0;
int nv1 = 0, nv2 = 0, nv3 = 0, nv4 = 0, nv5 = 0;
int ov1 = 0, ov2 = 0, ov3 = 0, ov4 = 0, ov5 = 0;

void drawFourArrows(void) {
  tft.drawLine(160, 120, 240, 120, ILI9341_WHITE);
  tft.drawLine(240, 120, 230, 110, ILI9341_WHITE);
  tft.drawLine(240, 120, 230, 130, ILI9341_WHITE);
  tft.drawLine(160, 120, 80, 120, ILI9341_WHITE);
  tft.drawLine(80, 120, 90, 110, ILI9341_WHITE);
  tft.drawLine(80, 120, 90, 130, ILI9341_WHITE);
  tft.drawLine(160, 120, 160, 200, ILI9341_WHITE);
  tft.drawLine(160, 200, 150, 190, ILI9341_WHITE);
  tft.drawLine(160, 200, 170, 190, ILI9341_WHITE);
  tft.drawLine(160, 120, 160, 40, ILI9341_WHITE);
  tft.drawLine(160, 40, 150, 50, ILI9341_WHITE);
  tft.drawLine(160, 40, 170, 50, ILI9341_WHITE);
}

void drawTwoArrows(void) {
  tft.drawLine(160, 120, 240, 120, ILI9341_WHITE);
  tft.drawLine(240, 120, 230, 110, ILI9341_WHITE);
  tft.drawLine(240, 120, 230, 130, ILI9341_WHITE);
  tft.drawLine(160, 120, 80, 120, ILI9341_WHITE);
  tft.drawLine(160, 120, 160, 200, ILI9341_WHITE);
  tft.drawLine(160, 200, 150, 190, ILI9341_WHITE);
  tft.drawLine(160, 200, 170, 190, ILI9341_WHITE);
  tft.drawLine(160, 120, 160, 40, ILI9341_WHITE);
}

void changePhase(void) {
  pressureLearnCount = 0;
  if (controlState == 1) {
    controlState = 0;
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor(50, 218);
    tft.println("Hold arrows to send learnable signal...");
    tft.fillCircle(20, 220, 17, ILI9341_WHITE);
    tft.setCursor(9, 216);
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_BLACK);
    tft.println("Ctrl");
  }
  else {
    controlState = 1;
    tft.fillRect(50, 218, 250, 230, ILI9341_BLACK);
    tft.fillCircle(20, 220, 17, ILI9341_WHITE);
    tft.setCursor(7, 216);
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_BLACK);
    tft.println("Learn");
  }
}

void changeMode(void) {
  pressureChangeCount = 0;
  if (buttonState == 4) buttonState = 0;
  buttonState++;
  switch (buttonState) {
    //4 Way Stick Mode
    case 1 :
      controlState = 1;
      tft.fillScreen(ILI9341_BLACK);
      drawFourArrows();
      tft.fillCircle(160, 120, 40, ILI9341_GREEN);
      tft.setTextSize(2);
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(0, 0);
      tft.println("4-Way S Mode");
      tft.setTextSize(1);
      tft.setCursor(220, 0);
      tft.println(VERSION);
      tft.fillCircle(20, 220, 17, ILI9341_WHITE);
      tft.setCursor(7, 216);
      tft.setTextColor(ILI9341_BLACK);
      tft.println("Learn");
      break;
    //4 Way Free Mode
    case 2 :
      controlState = 1;
      tft.fillScreen(ILI9341_BLACK);
      drawFourArrows();
      tft.fillCircle(160, 120, 40, ILI9341_GREEN);
      tft.fillCircle(160, 120, 20, ILI9341_DARKGREEN);
      tft.setTextSize(2);
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(0, 0);
      tft.println("4-Way F Mode");
      tft.setTextSize(1);
      tft.setCursor(220, 0);
      tft.println(VERSION);
      tft.fillCircle(20, 220, 17, ILI9341_WHITE);
      tft.setCursor(7, 216);
      tft.setTextColor(ILI9341_BLACK);
      tft.println("Learn");
      break;
    //2 Way Stick Mode
    case 3 :
      controlState = 1;
      tft.fillScreen(ILI9341_BLACK);
      drawTwoArrows();
      tft.fillCircle(160, 120, 40, ILI9341_RED);
      tft.setTextSize(2);
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(0, 0);
      tft.println("2-Way S Mode");
      tft.setTextSize(1);
      tft.setCursor(220, 0);
      tft.println(VERSION);
      tft.fillCircle(20, 220, 17, ILI9341_WHITE);
      tft.setCursor(7, 216);
      tft.setTextColor(ILI9341_BLACK);
      tft.println("Learn");
      break;
    //2 Way Free Mode
    case 4 :
      controlState = 1;
      tft.fillScreen(ILI9341_BLACK);
      drawTwoArrows();
      tft.fillCircle(160, 120, 40, ILI9341_RED);
      tft.fillCircle(160, 120, 20, ILI9341_MAROON);
      tft.setTextSize(2);
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(0, 0);
      tft.println("2-Way F Mode");
      tft.setTextSize(1);
      tft.setCursor(220, 0);
      tft.println(VERSION);
      tft.fillCircle(20, 220, 17, ILI9341_WHITE);
      tft.setCursor(7, 216);
      tft.setTextColor(ILI9341_BLACK);
      tft.println("Learn");
      break;
      //case 5 :
      //  tft.fillScreen(ILI9341_BLACK);
      //  drawFourArrows();
      //  tft.fillCircle(160, 120, 20, ILI9341_PURPLE);
      //  tft.setCursor(0, 0);
      //  tft.println("4-Way P Mode");
      //  break;
  }
}

void sendMidiMessages(void) {
  if (reducerCount == REDUCER) {
    reducerCount = 0;
    if (nv1 != ov1) {
      MIDI.sendControlChange(16, nv1, 1);
    }
    if (nv2 != ov2) {
      MIDI.sendControlChange(17, nv2, 2);
    }
    //If mode is 4 way
    if (buttonState == 1 || buttonState == 2) {
      if (nv3 != ov3) {
        MIDI.sendControlChange(18, nv3, 3);
      }
      if (nv4 != ov4) {
        MIDI.sendControlChange(19, nv4, 4);
      }
    }
  }
  else {
    reducerCount++;
  }
}

void sendLearnMessage(int CC) {
  MIDI.sendControlChange(CC, 0, CC - 15);
}

void setup(void) {    

  MIDI.begin();
  Serial.begin(BAUD);

  tft.begin();

  if (!ts.begin()) {
    while (1);
  }

  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(1);
  tft.setTextSize(3);

  //Initial mode setup
  changeMode();
}

void loop()
{
  ov1 = nv1;
  ov2 = nv2;
  ov3 = nv3;
  ov4 = nv4;

  if (!ts.touched()) {
    while (!ts.bufferEmpty()) {
      ts.getPoint();
    }
    if ((buttonState == 1 || buttonState == 3) && controlState == 1) {
      nv1 = 0;
      nv2 = 0;
      nv3 = 0;
      nv4 = 0;
      reducerCount = REDUCER;
      sendMidiMessages();
      return;
    }
    return;
  }

  TS_Point p = ts.getPoint();
  int coordx = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  int coordy = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

  if (controlState == 1) {
    if (buttonState == 1 || buttonState == 2) {
      int midiX = map(p.x, TS_MINX, TS_MAXX, 0, 254);
      midiX = constrain(midiX, 0, 254);
      int midiY = map(p.y, TS_MINY, TS_MAXX, 0, 254);
      midiY = constrain(midiY, 0, 254);

      if (midiX < 128) {
        nv1 = MIDI_MAX - midiX;
        nv2 = 0;
      }
      else {
        nv1 = 0;
        nv2 = midiX - MIDI_MAX;
      }

      if (midiY < 128) {
        nv3 = MIDI_MAX - midiY;
        nv4 = 0;
      }
      else {
        nv3 = 0;
        nv4 = midiY - MIDI_MAX;
      }
    }
    else {
      int midiX = map(p.x, TS_MINX, TS_MAXX, 0, MIDI_MAX);
      midiX = constrain(midiX, 0, MIDI_MAX);
      int midiY = map(p.y, TS_MINY, TS_MAXY, 0, MIDI_MAX);
      midiY = constrain(midiY, 0, MIDI_MAX);

      nv1 = midiX;
      nv2 = midiY;
    }

    if (p.z > 20) {
      sendMidiMessages();
    }
  }
  else {
    if (buttonState == 1 || buttonState == 2) {
      if (coordx < 120 && coordx > 30 && coordy < 140 && coordy > 100) {
        sendLearnMessage(16);
      }
      if (coordx < 300 && coordx > 220 && coordy < 140 && coordy > 100) {
        sendLearnMessage(17);
      }
      if (coordx < 180 && coordx > 140 && coordy < 100 && coordy > 30) {
        sendLearnMessage(18);
      }
      if (coordx < 180 && coordx > 140 && coordy < 220 && coordy > 140) {
        sendLearnMessage(19);
      }
    }
    else {
      if ((coordx < 120 && coordx > 30 && coordy < 140 && coordy > 100) ||
         (coordx < 300 && coordx > 220 && coordy < 140 && coordy > 100)) {
        sendLearnMessage(17);
      }
      if ((coordx < 180 && coordx > 140 && coordy < 100 && coordy > 30) ||
         (coordx < 180 && coordx > 140 && coordy < 220 && coordy > 140)) {
        sendLearnMessage(16);
      }
    }
  }

  if (p.z < 20 && coordx < 23 && coordx > 0 && coordy < 23 && coordy > 0) pressureLearnCount++;
  if (p.z < 20 && coordx < 180 && coordx > 140 && coordy < 140 && coordy > 100) pressureChangeCount++;
  if (p.z < 20 && pressureLearnCount > 1000 && coordx < 23 && coordx > 0 && coordy < 23 && coordy > 0) {
    changePhase();
  }
  if (p.z < 20 && pressureChangeCount > 1000 && coordx < 180 && coordx > 140 && coordy < 140 && coordy > 100) {
    changeMode();
  }
}
