#include <Arduino.h>
#include <Arduboy2.h>
#include <ATMlib.h>

Arduboy2 arduboy;
ATMsynth synth;
int numFrames;
int framesSurvived;

const uint8_t PROGMEM gunshot[] = {
  // Number of tracks
  0x03,

  // Addresses of tracks
  0x00, 0x00,
  0x03, 0x00,
  0x06, 0x00,

  // Channels
  0x00,
  0x00,
  0x00,
  // Channel 3, noise
  0x02,

  // Track 0 (mute channel): 3 bytes
  0x40, 0,
  0x9F,

  // Track 1 (unused): 3 bytes
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
  synth.play(gunshot);
}

const int MENU = 0;
const int START = 1;
const int MISTAKE = 2;
const int BEING_SHOT = 3;
const int GAME_OVER = 4;
int state = MENU;
int framesSinceLastState = 0;

uint8_t success[] = {
  A_BUTTON,
  B_BUTTON,
  UP_BUTTON,
  RIGHT_BUTTON,
  DOWN_BUTTON,
  LEFT_BUTTON,
};

void loop() {
  if (!(arduboy.nextFrame())) return;

  if (numFrames) {
    arduboy.clear();
    arduboy.print("Loading");
    if (numFrames > 260) {
      synth.playPause();
      numFrames = 0;
    } else {
      numFrames++;
    }
    arduboy.display();
    return;
  }

  arduboy.clear();
  arduboy.pollButtons();

  switch (state) {
  case MENU:
    arduboy.print("Daniel Shaver\nSimulator\n\nPress any button\nto play");

    for (int i = 0; i < 6; i++) {
      if (arduboy.justPressed(success[i])) {
        state = START;
        framesSinceLastState = 0;
        break;
      }
    }
    break;
  case MISTAKE:
    if (framesSinceLastState > 120) {
      framesSinceLastState = 0;
      state = BEING_SHOT;
      break;
    }
    arduboy.print("Officer Langley:\nDon't!");
    break;
  case BEING_SHOT:
    synth.playPause();
    framesSinceLastState = 0;
    state = GAME_OVER;
    break;
  case GAME_OVER:
    arduboy.print(
      "Officer Brailsford\n"
      "shot you five times\n"
      "with his AR-15\n\n"
      "You are dead");
    for (int i = 0; i < 6; i++) {
      if (arduboy.justPressed(success[i])) {
        state = MENU;
        framesSinceLastState = 0;
        break;
      }
    }
    break;
  default:
    if (arduboy.justPressed(B_BUTTON)) {
      state = MISTAKE;
      framesSinceLastState = 0;
    }
    if (arduboy.justPressed(A_BUTTON)) {
      state = MISTAKE;
      framesSinceLastState = 0;
    }
    arduboy.print(framesSurvived++);
    break;
  }

  framesSinceLastState++;
  arduboy.display();
}
