/*
  ###########################
          EXPERIMENTAL
  ###########################
  Audio player, non blocking.
  read 8bit mono .wav file
  Sample rate tested : 22050
  use Audacity to convert your audio file

  Author : AloyseTech
*/

#include <SD.h>
#include <SPI.h>
#include <zerodio.h>

// which Serial you prefer, for information.
#define SERIAL SerialUSB
//#define SERIAL Serial

// SD chip select pin (with ethernet shield : 4)
#define YOUR_SD_CS 6

//your wav file
const char *filename = "FX/FX1.wav";

//indicate sample rate here (use audacity to convert your wav)
const unsigned int sampleRate = 22050;

byte fx = 0;

bool state = true;

uint32_t timer = 0;

void setup()
{
  pinMode(13, OUTPUT);

  // debug output at 115200 baud
  delay(10);
  SERIAL.begin(115200);
  delay(500);

  // setup SD-card
  SERIAL.print("Initializing SD card...");
  if (!SD.begin(YOUR_SD_CS)) {
    SERIAL.println(" failed!");
    return;
  }
  SERIAL.println(" done.");

  // hi-speed SPI transfers
  // TODO: should be replaced by beginTx and endTx in SD lib...
  SPI.setClockDivider(4);

  AudioPlayer.begin(sampleRate);
  AudioPlayer.play(filename);
  SERIAL.println("Playing file.....");
}

void loop()
{
  if (millis() - timer > 500) {
    digitalWrite(13, state);
    state = !state;
    timer = millis();
  }

  if (SERIAL.available()) {
    char c = SERIAL.read();
    if (c == 'p') {
      fx++;
      if (fx == 11)
        fx = 1;
      String s = "";
      s += "FX/FX";
      s += fx;
      s += ".wav";

      AudioPlayer.play(s.c_str());
      SERIAL.print("Playing : ");
      SERIAL.println(s);
      SERIAL.println(millis());
    }
    else if ( c == 's') {
      AudioPlayer.play("FX/FX6.wav");
      SERIAL.println("Shoot !");
    }
  }

  if(digitalRead(

}
