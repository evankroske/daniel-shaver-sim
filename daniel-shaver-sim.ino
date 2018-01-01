#include <Arduino.h>
#include <Arduboy2.h>
#include <ATMlib.h>

Arduboy2 arduboy;
ATMsynth ATM;
int numFrames;
int framesSurvived;
int framesLeft;

const uint8_t PROGMEM music[] = {
  // Number of tracks
  0x03,

  // Addresses of tracks
  0x00, 0x00,
  0x03, 0x00,
  0x09, 0x00,

  // Channels
  0x00,
  0x00,
  0x00,
  // Channel 3, noise
  0x02,

  // Track 0 (mute channel)
  0x40, 0,
  0x9F,

  // Track 1 (unused)
  0x9F,
  0x9F,
  0x9F,

  // Track 2
  // Set tempo to 50
  0x9D, 50,
  // Set volume to 200
  0x40, 200,
  // Start volume slide
  0x41, -16,
  // wait 8 ticks
  0x9F + 8,
  // Slow volume slide
  0x41, -8,

  // wait 128 ticks
  0x9F + 64,
  0x9F + 64,

  // stop channel
  0x9F,
};

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(60);
  arduboy.audio.on();

  numFrames = 1;
  ATM.play(music);
}

void loop() {
  if (!(arduboy.nextFrame())) return;

  if (numFrames) {
    arduboy.clear();
    arduboy.print("Loading");
    if (numFrames > 260) {
      ATM.playPause();
      numFrames = 0;
    } else {
      numFrames++;
    }
    arduboy.display();
    return;
  }

  arduboy.pollButtons();
  arduboy.clear();
  if (arduboy.justPressed(B_BUTTON)) {
    ATM.playPause();
  }
  if (arduboy.justPressed(A_BUTTON)) {
    ATM.playPause();
  }
  arduboy.print(framesSurvived++);
  arduboy.display();
}
